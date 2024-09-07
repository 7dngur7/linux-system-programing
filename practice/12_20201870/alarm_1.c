#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
void ssu_signal_handler(int signo);

int count=0;
int main(void){
    	// SIGALRM 시그널에 대해 ssu_signal_handler 함수를 핸들러로 설정
	signal(SIGALRM, ssu_signal_handler);
	// 1초 후에 SIGALRM 시그널을 발생시킴
	alarm(1);
	while(1);
	exit(0);
}

// SIGALRM 시그널이 발생했을 때 호출되는 핸들러 함수
void ssu_signal_handler(int signo){
	printf("alarm %d\n", count++);
	alarm(1);
}
