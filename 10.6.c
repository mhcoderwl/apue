#include<pwd.h>
#include"apue.h"
 static void my_alarm(int signal){
	 struct passwd* rootptr;
	 printf("in signal handler\n");
	 if((rootptr=getpwnam("root"))==NULL)
		 err_sys("getpwnam() error");
	 alarm(1);
 }
int main(){
	struct passwd* ptr;
	signal(SIGALRM,my_alarm);
	alarm(1);
	while(1){
		if((ptr=getpwnam("wangliang")) ==NULL)
			err_sys("getpwnam() error");
		if(strcmp(ptr->pw_name,"wangliang")!=0)
			printf("return value corrupted!,pw_name=%s\n",ptr->pw_name);
	}
}
