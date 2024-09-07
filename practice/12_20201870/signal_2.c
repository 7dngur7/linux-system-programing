#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

static void ssu_signal_handler(int signo);

int main(void){
	if(signal(SIGINT, ssu_signal_handler)==SIG_ERR){
		fprintf(stderr, "cannot handler SIGINT\n");
		exit(EXIT_FAILURE);
	}
	if(signal(SIGTERM, ssu_signal_handler)==SIG_ERR){ 
		//kill 로 받은 종료시그널
		fprintf(stderr, "cannot handler SIGINT\n");
		exit(EXIT_FAILURE);
	}
	if(signal(SIGPROF, ssu_signal_handler)==SIG_ERR){
		//타이머 만료
		fprintf(stderr, "cannot handler SIGINT\n");
		exit(EXIT_FAILURE);
	}
	if(signal(SIGHUP, ssu_signal_handler)==SIG_ERR){
		//연결 끊어짐
		fprintf(stderr, "cannot handler SIGINT\n");
		exit(EXIT_FAILURE);
	}
	while(1)
		pause(); //시그널 수신까지 대기
	exit(0);
}

static void ssu_signal_handler (int signo){
	if(signo == SIGINT)
		printf("caught SIGINT\n");
	else if(signo == SIGTERM)
		printf("caught SIGTERM\n");
	else{
		fprintf(stderr, "unexpected signal\n");
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}
