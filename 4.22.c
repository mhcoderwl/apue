/*此程序用来递归降序遍历文件层次结构,命令行传入一个参数作为起点路径名.*/
#include<errno.h>     
#include"apue.h"

#include<limits.h>//定义了数据类型的范围.例如INT_MAX宏为该环境下int类型的最大值.
#include<dirent.h>
typedef int Myfunc(const char*,const struct stat*,int);
char* path_alloc(size_t*);
static Myfunc myfunc;
static int myftw(char*,Myfunc*);
static int dopath(Myfunc*);
static long nreg,ndir,nblk,nchr,nfifo,nslink,nsock,ntot;

int main( int argc,char** argv){
	int ret;
	if(argc!=2)
		err_quit("Usage: ftw <starting-pathname>");
	ret=myftw(argv[1],myfunc);
	ntot=nreg+ndir+nblk+nchr+nfifo+nslink+nsock;
	if(ntot==0)
		ntot=1;
	printf("regular files= %7ld,%5.12f %%\n",nreg,nreg*100.0/ntot);
	printf("directories = %7ld,%5.12f %%\n",ndir,ndir*100.0/ntot);
	printf("block special = %7ld,%5.12f %%\n",nblk,nblk*100.0/ntot);
	printf("char special = %7ld,%5.12f %%\n",nchr,nchr*100.0/ntot);
	printf("FIFOS    = %7ld,%5.12f %%\n",nfifo,nfifo*100.0/ntot);
	printf("symbolic links = %7ld,%5.12f %%\n",nslink,nslink*100.0/ntot);
	printf("sockets = %7ld,%5.12f %%\n",nsock,nsock*100.0/ntot);
	exit(ret);
}
#ifdef	PATH_MAX
static long	pathmax = PATH_MAX;
#else
static long	pathmax = 0;
#endif

static long	posix_version = 0;
static long	xsi_version = 0;

/* If PATH_MAX is indeterminate, no guarantee this is adequate */
#define	PATH_MAX_GUESS	1024

char *
path_alloc(size_t *sizep) /* also return allocated size, if nonnull */
{
	char	*ptr;
	size_t	size;

	if (posix_version == 0)
		posix_version = sysconf(_SC_VERSION);

	if (xsi_version == 0)
		xsi_version = sysconf(_SC_XOPEN_VERSION);

	if (pathmax == 0) {		/* first time through */
		errno = 0;
		if ((pathmax = pathconf("/", _PC_PATH_MAX)) < 0) {
			if (errno == 0)
				pathmax = PATH_MAX_GUESS;	/* it's indeterminate */
			else
				err_sys("pathconf error for _PC_PATH_MAX");
		} else {
			pathmax++;		/* add one since it's relative to root */
		}
	}

	/*
	 * Before POSIX.1-2001, we aren't guaranteed that PATH_MAX includes
	 * the terminating null byte.  Same goes for XPG3.
	 */
	if ((posix_version < 200112L) && (xsi_version < 4))
		size = pathmax + 1;
	else
		size = pathmax;

	if ((ptr = malloc(size)) == NULL)
		err_sys("malloc error for pathname");

	if (sizep != NULL)
		*sizep = size;
	return(ptr);
}
#define FTW_F 1
#define FTW_D 2
#define FTW_DNR 3
#define FTW_NS 4
static char* fullpath;
static size_t pathlen;
static int myftw(char* pathname,Myfunc* func){
	fullpath=path_alloc(&pathlen);
	if(pathlen<=strlen(pathname)){
		pathlen=strlen(pathname)*2;
		if((fullpath=realloc(fullpath,pathlen))==NULL)
			err_sys("realloc failed");
	}
	strcpy(fullpath,pathname);
	return dopath(func);
}


static int dopath(Myfunc* func){
	struct stat statbuf;
	struct dirent* dirp;
	DIR* dp;
	int  ret,n;
	if(lstat(fullpath,&statbuf)<0)
		return func(fullpath,&statbuf,FTW_NS);
	if(S_ISDIR(statbuf.st_mode)==0)
		return(func(fullpath,&statbuf,FTW_F));
	if(ret=func(fullpath,&statbuf,FTW_D)!=0)
	return ret;
	n=strlen(fullpath);
	if(n+NAME_MAX+2>pathlen){
	pathlen*=2;
	if((fullpath=realloc(fullpath,pathlen))==NULL)
		err_sys("realloc failed");
	}
	fullpath[n++]='/';
	fullpath[n]=0;
	if((dp=opendir(fullpath))==NULL)
		return func(fullpath,&statbuf,FTW_DNR);
	while((dirp=readdir(dp))!=NULL){
	if(strcmp(dirp->d_name,".")==0||strcmp(dirp->d_name,"..")==0)
		continue;
	strcpy(&fullpath[n],dirp->d_name);
	if((ret=dopath(func))!=0)
	break;
	}
	fullpath[n-1]=0;
	closedir(dp);
	return ret;
}

static int myfunc(const char* pathname,const struct stat* statptr,int type){
	switch(type){
		case FTW_F:
			if(S_ISREG(statptr->st_mode))
				nreg++;
			if(S_ISBLK(statptr->st_mode))
				ndir++;
			if(S_ISCHR(statptr->st_mode))
				nchr++;
			if(S_ISFIFO(statptr->st_mode))
				nfifo++;
			if(S_ISLNK(statptr->st_mode))
				nslink++;
			if(S_ISSOCK(statptr->st_mode))
				nsock++;
			break;
		case FTW_D:
			ndir++;
			break;
		case FTW_DNR:
			err_ret("Can't read directory %s",pathname);
			break;
		case FTW_NS:
			err_ret("stat error for %s",pathname);
			break;
	}
	return 0;
}

