#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void ssu_signal_handler1(int signo); 
void ssu_signal_handler2(int signo);

int main(void){
    	// SIGINT 시그널에 대해 ssu_signal_handler1 함수를 핸들러로 설정
	if (signal(SIGINT, ssu_signal_handler1) == SIG_ERR) { 
		fprintf(stderr, "cannot handle SIGINT\n");
		exit (EXIT_FAILURE);
	}
	// SIGUSR1 시그널에 대해 ssu_signal_handler2 함수를 핸들러로 설정
	if (signal(SIGUSR1, ssu_signal_handler2) == SIG_ERR) { 
		fprintf(stderr, "cannot handle SIGUSR1\n");
		exit (EXIT_FAILURE); 
	}
	raise(SIGINT);
	raise (SIGUSR1);
	printf ("main return\n"); 
	exit(0);
}

// SIGINT 시그널을 처리하기 위한 핸들러 함수
void ssu_signal_handler1(int signo){
	printf("SIGINT시그널발생\n");
}
// SIGUSR1 시그널을 처리하기 위한 핸들러 함수
void ssu_signal_handler2(int signo) { 
	printf("SIGUSR1 시그널발생\n");
}
