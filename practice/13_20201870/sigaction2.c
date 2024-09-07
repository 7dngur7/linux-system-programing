#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

// 시그널 대기 상태 확인 함수
void ssu_check_pending(int signo, char *signame);

// 시그널 핸들러 함수
void ssu_signal_handler(int signo);

int main(void){
    struct sigaction sig_act;
    sigset_t sig_set;

    sigemptyset(&sig_act.sa_mask); // sa_mask 초기화
    sig_act.sa_flags = 0; // 플래그 설정 없음
    sig_act.sa_handler = ssu_signal_handler; // 핸들러 함수 설정
    
    // SIGUSR1 시그널 처리 설정
    if (sigaction(SIGUSR1, &sig_act, NULL) != 0) {
        fprintf(stderr, "sigaction() error\n");
        exit(1);
    } else {
        sigemptyset(&sig_set); // sig_set 초기화
        sigaddset(&sig_set, SIGUSR1); // SIGUSR1 시그널 추가
        
        // 시그널 마스크 설정
        if (sigprocmask(SIG_SETMASK, &sig_set, NULL) != 0) {
            fprintf(stderr, "sigprocmask() error\n");
            exit(1);
        } else {
            printf("SIGUSR1 signals are now blocked\n");
            kill(getpid(), SIGUSR1); // SIGUSR1 시그널 전송
            printf("after kill()\n");
            ssu_check_pending(SIGUSR1, "SIGUSR1"); // SIGUSR1 대기 상태 확인
            sigemptyset(&sig_set); // sig_set 초기화
            sigprocmask(SIG_SETMASK, &sig_set, NULL); // 시그널 마스크 해제
            printf("SIGUSR1 signals are no longer blocked\n");
            ssu_check_pending(SIGUSR1, "SIGUSR1"); // SIGUSR1 대기 상태 재확인
        }
    }
    exit(0);
}

// 시그널 대기 상태 확인 함수
void ssu_check_pending(int signo, char *signame){
    sigset_t sig_set;
    if (sigpending(&sig_set) != 0)
        printf("sigpending() error\n");
    else if (sigismember(&sig_set, signo))
        printf("a %s signal is pending\n", signame); // 시그널이 대기 중인 경우
    else
        printf("%s signals are not pending\n", signame); // 시그널이 대기 중이 아닌 경우
}

// 시그널 핸들러 함수
void ssu_signal_handler(int signo){
    printf("in ssu_signal_handler function\n");
}

