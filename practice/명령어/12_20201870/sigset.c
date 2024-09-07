#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int main(void){
	sigset_t set;
	sigemptyset(&set);
	// SIGINT 시그널을 시그널 집합에 추가
	sigaddset(&set, SIGINT);

    	// SIGINT 시그널이 시그널 집합에 포함되어 있는지 확인
	switch(sigismember(&set,SIGINT)){
		case 1:
			printf("SIGINT is included.\n");
			break;
		case 0:
			printf("SIGINT is not included.\n");
			break;
		default:
			printf("failed to call sigismemeber() \n");
	}
	// SIGSYS 시그널이 시그널 집합에 포함되어 있는지 확인
	switch(sigismember(&set, SIGSYS)){
		case 1:
			printf("SIGSYS is included. \n");
			break;
		case 0:
			printf("SIGSYS is not included. \n");
			break;
		default:
			printf("failed to call sigismember() \n");
	}
	exit(0);
}
