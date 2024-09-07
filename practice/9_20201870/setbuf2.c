#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

int main(void){
	//버퍼 및 변수 선언
	char buf[BUFFER_SIZE];
	int a,b;
	int i;

	//표준입력에 버퍼 설정
	setbuf(stdin,buf); 
	scanf("%d %d", &a, &b);
	
	//개행문자 나올때까지 버퍼 내용 출력
	for(i=0; buf[i]!='\n';i++)
		putchar(buf[i]);
	putchar('\n');
	exit(0);
}

