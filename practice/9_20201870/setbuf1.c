#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(void){
	//버퍼 선언
	char buf[BUFFER_SIZE];
	
	// 표준 출력을 버퍼에 설정
	setbuf(stdout, buf);
	printf("Hello, ");
	sleep(1);
	printf("OSLAB!!");
	sleep(1);
	printf("\n");
	sleep(1);

	//표준 출력에 버퍼 해제
	setbuf(stdout, NULL);
	printf("How");
	sleep(1);
	printf(" are");
	sleep(1);
	printf(" you?");
	sleep(1);
	printf("\n");
	exit(0);
}

