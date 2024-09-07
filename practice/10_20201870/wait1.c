#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

// 종료 상태에 따라 메시지 출력
void ssu_echo_exit(int status);

int main(void){
	//변수 선언
	pid_t pid;
	int status;

    	//자식 프로세스 생성
	if((pid=fork())<0){
		fprintf(stderr, "fork error\n");
		exit(1);
	}
	else if(pid ==0)
		// 자식 프로세스 종료
		exit(7);

	if(wait(&status) != pid){
		fprintf(stderr, "wait error\n");
		exit(1);
	}
	ssu_echo_exit(status);

    	//자식 프로세스 생성
	if((pid = fork())<0){
		fprintf(stderr, "fork error\n");
		exit(1);
	}
	else if(pid==0)
		// 자식 프로세스는 비정상 종료
		abort();
	if(wait(&status)!=pid){
		fprintf(stderr, "wait error\n");
		exit(1);
	}
	ssu_echo_exit(status);
	
	    	//자식 프로세스 생성
	if((pid=fork())<0){
		fprintf(stderr, "fork error\n");
		exit(1);
	}
	else if(pid==0)
		// 0으로 나누어 에러 발생
		status /= 0;
	if (wait(&status)!=pid){
		fprintf(stderr, "wait error\n");
		exit(1);
	}
	ssu_echo_exit(status);
	exit(0);
}

// 종료 상태에 따라 메시지 출력
void ssu_echo_exit(int status){
	if(WIFEXITED(status))
		printf("normal temination, exit status = %d\n", WEXITSTATUS(status));
	else if(WIFSIGNALED(status))
		printf("abnormal termination, signal number = %d%s\n", WTERMSIG(status),
#ifdef WCOREDUMP
				WCOREDUMP(status) ? "(core file generated)" : "");
#else
	"");
#endif
	else if(WIFSTOPPED(status))
		printf("child stopped, signal number = %d\n", WSTOPSIG(status));
}

