#include"apue.h"
#include<sys/wait.h>
#define BUF_SIZE 4096
int main(int argc,char** argv){
	char buf[BUF_SIZE];
	pid_t pid;
	int status=0;
	printf("%% ");
	while(fgets(buf,BUF_SIZE,stdin)!=NULL){
		if(buf[strlen(buf)-1]=='\n')
			buf[strlen(buf)-1]=0;//相当于字符串的尾部的\0
		if((pid=fork())<0)
			err_sys("fork error");
		else if(pid==0){
			execlp(buf,buf,(char*)0);
			err_ret("Couldn't excute: %s\n",buf);
			exit(0);
		}if((pid=waitpid(pid,&status,0))<0)
				err_sys("waitpid error");
			printf("%% ");
			}
}
