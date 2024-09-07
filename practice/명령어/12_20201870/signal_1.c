#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void ssu_signal_handler (int signo);

// 시그널 처리를 위한 함수 포인터 선언
void (*ssu_func) (int);
int main(void){
	// SIGINT 시그널에 대해 ssu_signal_handler 함수를 핸들러로 설정
	ssu_func = signal(SIGINT, ssu_signal_handler);
	while(1){
		printf("process running ... \n");
		sleep(1);
	}
	exit(0);
}
// SIGINT 시그널을 처리하기 위한 핸들러 함수
void ssu_signal_handler(int signo){
	printf("SIGINT 시그널 발생.\n");
	printf("SIGINT를 SIG_DFL 로 재설정 함.\n");
	signal(SIGINT, ssu_func);
}
