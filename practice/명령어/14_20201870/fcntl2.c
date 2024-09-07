#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>

#define MSG "message will be written to Terminal\n"

int main(void){
	int new_fd;

	if((new_fd = fcntl(STDOUT_FILENO,F_DUPFD,3))==-1){ //파일 디스크립터를 복사
		fprintf(stderr,"Error : Copying File Descriptor\n");
		exit(1);
	}

	close(STDOUT_FILENO);//표준출력을 닫고, 파일디스크립터 3으로 표준출력으로 출력
	write(3,MSG,strlen(MSG));
	exit(0);
}

