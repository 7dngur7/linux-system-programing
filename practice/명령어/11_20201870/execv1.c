#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/resource.h>
#include <sys/wait.h>

// 시간을 초 단위로 변환하는 함수
double ssu_maketime(struct timeval * time);

// 프로세스 종료 상태를 출력하는 함수
void term_stat(int stat);

// 자식 프로세스의 정보를 출력하는 함수
void ssu_print_child_info(int stat, struct rusage *rusage);

int main(void){
	//자원 정보를 담는 구조체
	struct rusage rusage;
	pid_t pid;
	int status;
	
	// 자식 프로세스 생성, find 실행
	if((pid=fork())==0){
		char * args[]={"find", "/", "-maxdepth", "4", "-name", "stdio.h", NULL};

		if(execv("/usr/bin/find", args)<0){
			fprintf(stderr, "execv error\n");
			exit(1);
		}
	}
	// 자식 프로세스의 종료를 기다림
	if(wait3(&status, 0, &rusage)==pid)
		ssu_print_child_info(status,&rusage);
	else{
		fprintf(stderr, "wait3 error\n");
		exit(1);
	}
	exit(0);
}
double ssu_maketime(struct timeval *time){
	return ((double)time -> tv_sec + (double)time->tv_usec/1000000.0);
}
void term_stat(int stat){
	// 정상적으로 종료된 경우
	if(WIFEXITED(stat))
		printf("normally terminated. exit status = %d\n", WEXITSTATUS(stat));
	// 시그널에 의해 종료된 경우
	else if(WIFSIGNALED(stat))
		printf("abnormal termination by signal %d. %s\n", WTERMSIG(stat),
#ifdef WCOREDUMP
				WCOREDUMP(stat)?"core dumped":"no core"
#else
				NULL
#endif
				);
	// 일시 정지된 경우
	else if(WIFSTOPPED(stat))
		printf("stopped by signal %d\n", WSTOPSIG(stat));
}

void ssu_print_child_info(int stat, struct rusage *rusage){
	printf("Termination info follows\n");
	term_stat(stat);
	printf("user CPU time : %.2f(sec)\n", ssu_maketime(&rusage->ru_utime));
	printf("system CPU time : %.2f(sec)\n", ssu_maketime(&rusage->ru_stime));
}

