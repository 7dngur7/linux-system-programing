#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(void){
	sigset_t pendingset;
	sigset_t sig_set;
	int count =0;
	
	// 모든 시그널을 시그널 집합에 추가
	sigfillset(&sig_set);
	// 시그널 마스크를 설정하여 모든 시그널을 차단
	sigprocmask(SIG_SETMASK, &sig_set, NULL); 

	while(1){
	        // 대기 중인 시그널 확인
		printf("count : %d\n",count++);
		sleep(1);
		// SIGINT 시그널이 대기 중인 경우
		if(sigpending(&pendingset)==0){ 
			if(sigismember(&pendingset,SIGINT)){ 
				printf("SIGINT가 블록되어 대기중. 무한 루프를 종료.\n");
				break;
			}
		}
	}
	exit(0);
}
