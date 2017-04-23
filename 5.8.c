#include<stdio.h>
int main(){
	int c;
	while((c=getchar())!=EOF)
		if(putc(c,stdout)==EOF)
			err_sys("output error");
	if(ferror(stdin))
		err_sys("input error");
	exit(0);
}
