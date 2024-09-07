#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(void){
	//만들 파일 이름과 파일디스크립터 받을 인자
	char *fname = "ssu_test.txt";
	int fd;

	//파일 만들고 권한 설정
	if((fd = creat(fname, 0666))<0){
		fprintf(stderr, "creat error for %s\n", fname);
		exit(1);
	}
	else{
		//파일 생성 및 성공 메시지 출력
		printf("Success!\nFilename : %s\nDescriptor : %d\n", fname, fd);
		close(fd);
	}

	exit(0);
}
