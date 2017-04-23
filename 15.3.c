#include"apue.h"
#include<ctype.h>
int main(){
	int c;
	while((c=fgetc(stdin))!=EOF){
		if(isupper(c))
			c=tolower(c);
		if(putchar(c)==EOF)
			err_sys("output error");
		if(c=='\n')
			fflush(stdout);
	}
	exit(0);
}

