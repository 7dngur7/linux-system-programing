#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int main(int argc, char * argv[]){
	if(argc !=2){
		fprintf(stderr, "usage : %s [Process ID]\n", argv[0]);
		exit(1);
	}
	else{
	        // 입력된 프로세스 ID를 정수로 변환하여 해당 프로세스를 SIGKILL 시그널로 종료
		kill(atoi(argv[1]), SIGKILL);
	}
	exit(0);
}
