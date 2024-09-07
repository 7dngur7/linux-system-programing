#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(void){
	//변수 선언
	int fd;
	char *fname = "ssu_test.txt";

	//파일 creat 
	if((fd = creat(fname, 0666))<0){
		fprintf(stderr, "creat error for %s\n", fname);
		exit(1);
	}

	printf("First printf is on the screen.\n");
	//표준 출력으로 fd로 복사
	dup2(fd,1);
        printf("Second printf is in this file.\n");
	exit(0);	

}
