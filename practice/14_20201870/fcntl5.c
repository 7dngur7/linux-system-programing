#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h> 

#define MAX_CALL 100
#define MAX_BUF 20000

//string 을 입력받아 error문을 출력하는 함수
void serror(char *str){ 
	fprintf(stderr, "%s\n", str);
	exit(1);
}
int main(void){
	int nread, nwrite, val, i=0;
	char * ptr;

	char buf[MAX_BUF];
	int call[MAX_CALL];

	nread = read(STDIN_FILENO, buf, sizeof(buf));
	fprintf(stderr, "read %d bytes\n", nread);
	if((val = fcntl(STDOUT_FILENO, F_GETFL, 0))<0) //val에 STDOUT_FILENO fd의 접근권한과 상태플래그를 반환
		serror("fcntl F_GETFL error"); //fcntl 반환값 에러인 경우
	val |= O_NONBLOCK; //nonblock 플래그 켬
	if(fcntl(STDOUT_FILENO, F_SETFL, val) <0) //STDOUT_FILENO fd의 파일 상태 플래그를 val로 재설정
		serror("fcntl F_SETFL error"); //fcntl 반환값이 에러인 경우 

	for(ptr = buf; nread>0;i++){ //STDIN_FILENO에서 읽은 byte수가 0보다 작아질때까지 반복
		errno = 0;
		nwrite = write(STDOUT_FILENO, ptr,nread);
		if(i<MAX_CALL)
			call[i] = nwrite;
		fprintf(stderr, "nwrite = %d, errno = %d\n", nwrite, errno);
		if(nwrite >0){
			ptr += nwrite;
			nread -= nwrite;
		}
	}

	if((val = fcntl(STDOUT_FILENO, F_GETFL, 0))<0)
		serror("fcntl F_GETFL error");
	val &= ~O_NONBLOCK;
	if(fcntl(STDOUT_FILENO, F_SETFL, val)<0)
		serror("fcntl F_SETFL error");
	for(i=0;i<MAX_CALL;i++)
		fprintf(stdout, "call[%d] = %d\n", i,call[i]);
	exit(0);
}


