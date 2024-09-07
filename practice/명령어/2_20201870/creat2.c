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
		fprintf(stderr, "creatfale %s\n", fname);
		exit(1);
	}
	else{
		//파일 닫은 후 읽기 및 쓰기 권한으로 파일 열기
		close(fd);
		fd = open(fname, O_RDWR);
		//성공 메시지 출력
		printf("Succeeded!\n<%s> is new readable and writable\n", fname);
	}

	exit(0);
}
