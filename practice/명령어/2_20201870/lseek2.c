#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

//파일생성모드
#define CREAT_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

char buf1[] = "1234567890";
char buf2[] = "ABCDEFGHIJ";

int main(void){
	//만들 파일 이름과 파일디스크립터, 파일 크기 받을 인자
	char *fname = "ssu_hole.txt";
	int fd;

	//생성모드대로 파일 생성
	if((fd = creat(fname, CREAT_MODE))<0){
		fprintf(stderr, "creat error for %s\n", fname);
		exit(1);
	}	

	//buf1 내용작성
	if(write(fd, buf1, 12) !=12){
		fprintf(stderr, "buf1 write error\n");
		exit(1);
	}

	//15000바이트만큼 파일오프셋 이동
	if(lseek(fd, 15000, SEEK_SET)<0){
		fprintf(stderr, "lseek error\n");
		exit(1);
	}

	//buf2 내용작성
	if(write(fd, buf2, 12) !=12){
		fprintf(stderr, "buf2 write error\n");
		exit(1);
	}

	exit(0);

}
