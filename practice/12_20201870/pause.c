#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void ssu_alarm(int signo);

int main(void){
	printf("Alarm Setting\n");
	signal(SIGALRM, ssu_alarm);
	alarm(2);

	//2초마다 "done" 메시지 출력
	while(1){
		printf("done\n");
		pause();
		alarm(2);
	}
	exit(0);
}

// SIGALRM 시그널이 발생했을 때 호출되는 핸들러 함수
void ssu_alarm(int signo){
	printf("alarm..!!\n");
}
