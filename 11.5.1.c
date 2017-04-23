#include"apue.h"
#include<pthread.h>
void* thr_fn1(void* arg){
	printf("thread 1 returning\n");
	return (void*)1;
}
void* thr_fn2(void* arg){
	printf("thread 2 returning\n");
	return (void*)2;
}
int main(){
	int err;
	pthread_t tid1,tid2;
	void* tret;
	if((err=pthread_create(&tid1,NULL,thr_fn1,NULL))!=0)
		err_exit(err,"can't create thread 1");
	if((err=pthread_create(&tid2,NULL,thr_fn2,NULL))!=0)
		err_exit(err,"can't create thread 2");
	if((err=pthread_join(tid1,&tret))!=0)
		err_exit(err,"can't join with thread 1");
	printf("thread 1 exit code %ld\n",(long)tret);
	if((err=pthread_join(tid2,&tret))!=0)
		err_exit(err,"can't join with thread2");
	printf("thread 1 exit code %ld\n",(long)tret);
	exit(0);
}


