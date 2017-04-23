#include<errno.h>
#include"apue.h"
#include<sys/time.h>

#ifdef MACOS
#include<sys/syslimits.h>
#elif defined(SOLARIS)
#include<limits.h>
#elif defined(BSD)
#include<sys/param.h>
#endif

unsigned long long count;
struct timeval end;
void checktime(char* str){
	struct timeval tv;
	gettimeofday(&tv,NULL);
	if(tv.tv_sec >= end.tv_sec && tv.tv_usec >=end.tv_usec){
		printf("%s count =%lld\n",str,count);
		exit(0);
	}
}

int main(int argc, char** argv){
	pid_t pid;
	char* s;
	int nzero,ret;
	int adj=0;
	//setbuf(stdout,NULL);
#ifdef NZERO
	nzero=NZERO;
#elif defined(_SC_NZERO)
	nzero=sysconf(_SC_NZERO);
#else
#error NZERO undefined
#endif
	printf("NZERO=%d\n",nzero);
	if(argc==2)
		adj=atoi(argv[1]);
	gettimeofday(&end,NULL);
	end.tv_sec+=10;
	if((pid=fork())<0)
		err_sys("fork failed");
	else if(pid==0){
		s="child";
		printf("Current nice value in child is %d,adjusting by %d\n",nice(0)+nzero,adj);
		errno=0;
		if((ret=nice(adj))==-1&&errno!=0)
			err_sys("child set scheduling priority");
		printf("now child nice value is %d\n",ret+nzero);
		}else {
			s="parent";
			printf("Current nice value in child is %d\n",nice(0)+nzero);
		}
	while(1){
		if(++count==0)
			err_quit("%s counter wrap",s);
		checktime(s);
	}
	exit(0);
}
