#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

void ssu_signal_handler(int signo);

void ssu_timestamp(char *str);

int main(void){
    struct sigaction sig_act;
    sigset_t blk_set;

    sigfillset(&blk_set); // 모든 시그널을 blk_set에 추가
    sigdelset(&blk_set, SIGALRM); // SIGALRM 시그널을 blk_set에서 제외
    sigemptyset(&sig_act.sa_mask);
    sig_act.sa_flags=0;
    sig_act.sa_handler = ssu_signal_handler;
    sigaction(SIGALRM, &sig_act,NULL); // SIGALRM 시그널에 대한 핸들러 등록
    ssu_timestamp("befor sigsuspend()");
    alarm(5); // 5초 후 SIGALRM 시그널 발생
    sigsuspend(&blk_set); // blk_set에 있는 시그널이 발생할 때까지 대기
    ssu_timestamp("after sigsuspend()"); 
    exit(0);
}

// 시그널 핸들러 함수
void ssu_signal_handler(int signo){
    printf("in ssu_signal_handler() function\n");
}

// 현재 시간 출력 함수
void ssu_timestamp(char *str){
    time_t time_val;
    
    time(&time_val);
    printf("%s the time is %s\n",str,ctime(&time_val));
}

