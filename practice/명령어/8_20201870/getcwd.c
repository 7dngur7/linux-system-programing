#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PATHMAX 1024

int main(void){
	//변수 선언
	char *pathname;

	//작업 디렉토리 변경
	if(chdir("/home/woohyuk")<0){
		fprintf(stderr, "chdir error\n"); 
		exit (1);
	}
	
	pathname = (char *)malloc(PATHMAX);

	//현재 작업 디렉토리 얻음
	if(getcwd(pathname, PATHMAX)==NULL){
		fprintf(stderr, "getcwd error\n");
		exit (1);
	}
	
	printf("current directory = %s\n", pathname);
       	exit (0);

}
