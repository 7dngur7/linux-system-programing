#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(void){
	//자식프로세스 1
	if(fork() ==0){
	        // "/bin/echo"를 실행하고 메세지 출력
		execl("/bin/echo", "echo", "this is", "message one", (char *)0);
		fprintf(stderr, "exec error\n");
		exit(1);
	}
	//자식프로세스 2
	if(fork()==0){
	        // "/bin/echo"를 실행하고 메세지 출력
		execl("/bin/echo","echo", "this is", "message two", (char *)0);
		fprintf(stderr, "exec error\n");
		exit(1);
	}
	//자식프로세스 3
	if(fork()==0){
	        // "/bin/echo"를 실행하고 메세지 출력
		execl("/bin/echo", "echo", "this is", "message three", (char *)0);
		fprintf(stderr,"exec error\n");
		exit(1);
	}
	printf("Parent program ending\n");
	exit(0);
}

