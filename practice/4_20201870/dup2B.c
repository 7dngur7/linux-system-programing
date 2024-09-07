#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024

int main(void){
	//변수 및 버퍼 선언
	int fd;
	char *fname="ssu_test.txt";
	char buf[BUFFER_SIZE];
	int length;

	//파일 open
	if((fd = open(fname, O_RDONLY))<0){
		fprintf(stderr, "open error for %s\n", fname);
		exit(1);
	}

	//파일디스크립터 4에 1복사
	if(dup2(1, 4) != 4){
		fprintf(stderr, "dup2 call failed\n");
		exit(1);
	}

	//파일디스크립터 4로 표준출력 수행
	while(1){
		length=read(fd, buf, BUFFER_SIZE);
		if(length <= 0) break;
		
		write(4, buf, length);
	}
	exit(0);
}
