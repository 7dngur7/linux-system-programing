#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

int main(void){
	//만들 파일 이름과 파일디스크립터, 파일 크기 받을 인자
	char *fname = "ssu_test.txt";
	off_t fsize;
	int fd;

	//읽기 전용으로 파일 열기
	if((fd = open(fname, O_RDONLY))<0){
		fprintf(stderr, "open error for %s\n", fname);
		exit(1);
	}

	//파일 크기 구하기
	if((fsize = lseek(fd, 0, SEEK_END))<0){
		fprintf(stderr, "lseek error");
		exit(1);
	}
	
	//파일 크기 메시지 출력
	printf("The size of <%s> is %ld bytes.\n",fname, fsize);
	
	exit(0);
}
