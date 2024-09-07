#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

// SIGUSR1 시그널을 받았을 때 호출될 시그널 핸들러 함수
void ssu_signal_handler(int signo){
    printf("ssu_signal_handler control.\n");
}

int main(void){
    struct sigaction sig_act;  
    sigset_t sig_set;          

    // sig_act의 sa_mask를 빈 집합으로 초기화
    sigemptyset(&sig_act.sa_mask);
    
    sig_act.sa_flags = 0;
    
    // SIGUSR1 시그널이 발생했을 때 호출될 핸들러 함수 지정
    sig_act.sa_handler = ssu_signal_handler;
    
    // sigaction 실행
    sigaction(SIGUSR1, &sig_act, NULL);
    
    printf("before first kill()\n");
    
    // 현재 프로세스에 SIGUSR1 시그널을 보냄 -> 핸들러 함수가 실행됨
    kill(getpid(), SIGUSR1);
    
    // sig_set을 빈 집합으로 초기화
    sigemptyset(&sig_set);
    
    // sig_set에 SIGUSR1 시그널 추가
    sigaddset(&sig_set, SIGUSR1);
    
    // SIGUSR1 시그널 블록
    sigprocmask(SIG_SETMASK, &sig_set, NULL);
    
    printf("before second kill()\n");
    
    // 현재 프로세스에 SIGUSR1 시그널을 보냄 
    kill(getpid(), SIGUSR1);
    
    printf("after second kill()\n");
    
    exit(0);
}

