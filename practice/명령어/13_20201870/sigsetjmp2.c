#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <setjmp.h>
#include <signal.h>
#include <time.h>

static void ssu_alarm(int signo); // 알람 핸들러 함수
static void ssu_func(int signo); // SIGUSR1 핸들러 함수
void ssu_mask(const char *str); // 시그널 마스크 출력 함수

static volatile sig_atomic_t can_jump; // 시그널 처리 가능 여부를 나타내는 변수
static sigjmp_buf jump_buf; // longjmp()로 이동할 위치를 저장하는 버퍼

int main(void){
    if(signal(SIGUSR1, ssu_func) == SIG_ERR){
        fprintf(stderr, "SIGUSR1 error");
        exit(1);
    }

    if(signal(SIGALRM, ssu_alarm) == SIG_ERR){
        fprintf(stderr, "SIGALRM error");
        exit(1);
    }
    ssu_mask("starting main: "); // main 함수 시작 지점에서의 시그널 마스크 출력
    if(sigsetjmp(jump_buf,1)){
        ssu_mask("ending main: "); // main 함수 종료 지점에서의 시그널 마스크 출력
        exit(0);
    }
    can_jump = 1;
    while(1)
        pause();
    exit(0);
}
void ssu_mask(const char *str){
    sigset_t sig_set;
    int err_num;

    err_num = errno;

    if(sigprocmask(0,NULL,&sig_set)<0){
        printf("sigprocmask() error");
        exit(1);
    }

    printf("%s", str);

    if(sigismember(&sig_set, SIGINT))
        printf("SIGINT ");
    if(sigismember(&sig_set, SIGQUIT))
        printf("SIGQUIT ");
    if(sigismember(&sig_set, SIGUSR1))
        printf("SIGUSR1 ");
    if(sigismember(&sig_set, SIGALRM))
        printf("SIGALRM ");

    printf("\n");
    errno = err_num;
}

static void ssu_func(int signo){
    time_t start_time;

    if(can_jump ==0)
        return;

    ssu_mask("starting ssu_func: "); // ssu_func 함수 시작 지점에서의 시그널 마스크 출력
    alarm(3); // 3초 후 SIGALRM 시그널 발생
    start_time = time(NULL);

    while(1)
        if(time(NULL) > start_time + 5)
            break;
    
    ssu_mask("ending ssu_func: "); // ssu_func 함수 종료 지점에서의 시그널 마스크 출력
    can_jump =0;
    siglongjmp(jump_buf,1); // 설정된 jump_buf로 이동
}
static void ssu_alarm(int signo){
    ssu_mask("in ssu_alarm: "); // ssu_alarm 함수에서의 시그널 마스크 출력
}

