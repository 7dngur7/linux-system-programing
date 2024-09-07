#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <errno.h>
#include <fcntl.h> 
int main(int argc, char *argv[]){
	struct flock lock;
	int fd;
	char command[100];

	if((fd = open(argv[1], O_RDWR))==-1){ // 파일 열기 시도
		perror(argv[1]); // 오류 메시지 출력
		exit(1);
	}

	lock.l_type = F_WRLCK; 
	lock.l_whence = 0; 
	lock.l_start = 0L; 
	lock.l_len = 0L;

	if(fcntl(fd, F_SETLK, &lock) == -1){ // Lock 설정 시도
		if(errno == EACCES){
			printf("%s busy -- try later\n",argv[1]); // 파일이 사용 중임
			exit(2);
		}
		perror(argv[1]); // 다른 오류인 경우 오류 메시지 출력
		exit(3); 
	}

	sprintf(command, "vim %s\n",argv[1]);
	system(command);

	lock.l_type = F_UNLCK; 
	fcntl(fd, F_SETLK, &lock); // Lock 해제 시도
	close(fd);

	return 0;
}

