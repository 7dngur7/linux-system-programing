#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(void){
	sigset_t old_set;
	sigset_t sig_set;

	sigemptyset(&sig_set);
	sigaddset(&sig_set, SIGINT); //SIGINT 시그널 켬
	sigprocmask(SIG_BLOCK, &sig_set, &old_set)
	sigsuspend(&old_set);//캐치할 수 있는 시그널 또는 (SIGINT)시그널이 도착할 때까지 프로세스를 잠시 블록
	exit(0);
}
