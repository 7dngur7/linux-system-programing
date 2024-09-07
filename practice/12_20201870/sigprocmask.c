#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(void){
	sigset_t sig_set;
	int count;
	
    	// 시그널 집합을 초기화하고 SIGINT 시그널을 추가
	sigemptyset(&sig_set);
	sigaddset(&sig_set, SIGINT);
	// SIGINT 시그널을 차단하도록 시그널 마스크 설정
	sigprocmask(SIG_BLOCK, &sig_set,NULL);
	for(count =3; 0<count;count--){
		printf("count %d\n",count);
		sleep(1);
	}

    	// SIGINT 시그널의 차단을 해제하여 Ctrl-C 입력에 대한 반응을 가능하게 함
	printf("Ctrl-C에 대한 블록을 해제\n"); 
	sigprocmask(SIG_UNBLOCK,&sig_set,NULL);
	printf("count중 Ctrl-C입력하면 이 문장은 출력되지 않음.\n");
	
	while(1);
	exit(0);
}
