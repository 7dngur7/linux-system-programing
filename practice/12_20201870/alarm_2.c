#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define LINE_MAX 2048

static void ssu_alarm(int signo);
int main(void){
	char buf[LINE_MAX];
	int n;

    	// SIGALRM 시그널에 대해 ssu_alarm 함수를 핸들러로 설정
	if(signal(SIGALRM, ssu_alarm)==SIG_ERR){
		fprintf(stderr,"SIGALRM error\n");
		exit(1);
	}
    	// 10초 후에 SIGALRM 시그널을 발생시킴
	alarm(10);
	
	// 표준 입력(STDIN)으로부터 데이터를 읽음
	if((n=read(STDIN_FILENO,buf,LINE_MAX))<0){
		fprintf(stderr,"read() error\n");
		exit(1);
	}
	alarm(0);
	write(STDOUT_FILENO,buf,n);
	exit(0);
}

// SIGALRM 시그널을 처리하기 위한 핸들러 함수
static void ssu_alarm(int signo){
	printf("ssu_alarm() called!\n");
}
