#include"apue.h"
#include<sys/wait.h>
#define DEF_PAGER "/bin/more"
int main(int argc,char** argv){
	int n;
	int fd[2];
	pid_t pid;
	char* pager;
	char* argv0;
	char line[MAXLINE];
	FILE* fp;
	if((fp=fopen(argv[1],"r"))==NULL)
		err_sys("can't open %s",argv[1]);
	if(pipe(fd)<0)
		err_sys("pipe error");
	if((pid=fork())>0){
		close(fd[0]);
		while(fgets(line,MAXLINE,fp)!=NULL){
			write(fd[1],line,strlen(line));
			}
		if(ferror(fp))
			err_sys("fgets error");
		close(fd[1]);
		if(waitpid(pid,NULL,0)<0)
			err_sys("waitpid error");
		exit(0);
		}else{
			close(fd[1]);
			dup2(fd[0],STDIN_FILENO);
			close(fd[0]);
			if((pager=getenv("PAGER"))==NULL)
				pager=DEF_PAGER;
			if((argv0=strrchr(pager,'/'))!=NULL)
				argv0++;
			else
				argv0=pager;
			if(execl(pager,argv0,(char*)0)<0)
				err_sys("execl error for %s",pager);
		}
		exit(0);
}


		
			
