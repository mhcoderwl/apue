#include<stdlib.h>
#include<pthread.h>
const int NHASH=29;
#define HASH(id) (((unsigned long)id)%NHASH)
pthread_mutex_t hashlock=PTHREAD_MUTEX_INITIALIZER;
struct foo{
	int f_count;
	pthread_mutex_t f_lock;
	int f_id;
	struct foo* f_next;//protected by hashlock
};
struct foo* fh[NHASH];

struct foo* foo_alloc(int id){
	struct foo* fp;
	int idx;
	if((fp=(struct foo*)malloc(sizeof(struct foo)))!=NULL){
		fp->f_count=1;
		fp->f_id=id;
		if(pthread_mutex_init(&fp->f_lock,NULL)!=0){
			free(fp);
			return NULL;
		}
		idx=HASH(id);
		pthread_mutex_lock(&hashlock);
		fp->f_next=fh[idx];
		fh[idx]=fp;
		pthread_mutex_lock(&fp->f_lock);
		pthread_mutex_unlock(&hashlock);
	}
	return fp;
}
void foo_hold(struct foo* fp){
	pthread_mutex_lock(&fp->f_lock);
	fp->f_count++;
	pthread_mutex_unlock(&fp->f_lock);
}
struct foo* foo_find(int id){
	struct foo* fp;
	pthread_mutex_lock(&hashlock);
	for(fp=fh[HASH(id)];fp!=NULL;fp=fp->f_next){
		if(fp->f_id==id){
			fp->f_count++;
			break;
		}
	}
	pthread_mutex_unlock(&hashlock);
	return fp;
}
//第一个版本的释放函数,严格按照每种互斥量的功能来锁住临界区,由于需要考虑互斥量使用顺序,会导致代码变得复杂.(foo_rele函数中先上锁在观察是否需要修改fh散列表,如果需要修改则必须先解锁在按顺序上锁)
void foo_rele(struct foo* fp){
	struct foo* tfp;
	int idx;
	pthread_mutex_lock(&fp->f_lock);
	if(fp->f_count==1){
		pthread_mutex_unlock(&fp->f_lock);
		pthread_mutex_lock(&hashlock);
		pthread_mutex_lock(&fp->f_lock);
		if(fp->f_count!=1){
			fp->f_count--;
			pthread_mutex_unlock(&fp->f_lock);
			pthread_mutex_unlock(&hashlock);
			return;
		}else{
			idx=HASH(fp->f_id);
			tfp=fh[idx];
			if(tfp==fp){
				fh[idx]=fp->f_next;
			}else{
				while(tfp->f_next!=fp)
					tfp=tfp->f_next;
				tfp->f_next=fp->f_next;
				}
		}
		pthread_mutex_unlock(&fp->f_lock);
		pthread_mutex_unlock(&hashlock);
		pthread_mutex_destory(&fp->f_lock);
		free(fp);
	}else{
		fp->f_count--;
		pthread_mutex_unlock(&fp->f_lock);
	}
}
//第二个版本的释放函数.直接用hashlock来锁住对fp的操作,避免了两个互斥量使用的顺序问题.简化了代码.但是线程阻塞的概率会更大,因为hashlock使用的范围更大.需要权衡两者的使用.
void foo_rele2(struct foo* fp){
	struct foo* tfp;
	int idx;
	pthread_mutex_lock(&hashlock);
	if(--fp->f_count==0){
		idx=HASH(fp->f_id);
		tfp=fh[idx];
		if(tfp==fp){
			fh[idx]=fp->f_next;
		}else{
			while(tfp->f_next!=fp)
				tfp=tfp->f_next;
			tfp->f_next=fp->f_next;
		}
		pthread_mutex_unlock(&hashlock);
		pthread_mutex_destory(&fp->f_lock);
		free(fp);
	}else
		pthread_mutex_unlock(&hashlock);
}


