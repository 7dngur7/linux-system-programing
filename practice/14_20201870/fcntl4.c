#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <errno.h> 
#include <fcntl.h> 

void set_flags(int fd, int flags);
void clr_flags(int fd, int flags);

char buf[50000];

int main(void){
	int ntowrite, nwrite;
	char * ptr;

	ntowrite = read(STDIN_FILENO, buf, sizeof(buf)); //읽은 byte 수 return
	fprintf(stderr, "reading %d bytes \n",ntowrite);

	set_flags(STDOUT_FILENO, O_NONBLOCK);

	ptr = buf;
	while(ntowrite>0){
		errno =0;
		nwrite = write(STDOUT_FILENO, ptr, ntowrite);
		fprintf(stderr, "nwrite = %d, errno = %d\n", nwrite, errno);
		if(nwrite>0){
			ptr += nwrite;
			ntowrite -= nwrite;
		}
	}
	clr_flags(STDOUT_FILENO, O_NONBLOCK); //nonblocking 해제
	exit(0);
}
void set_flags(int fd, int flags){
	int val;
	if((val = fcntl(fd,F_GETFL,0))<0){ //파일의 접근권한과 상태 플래그 return
		fprintf(stderr, "fcntl F_GETFL failed");
		exit(1);
	}
	val |= flags; //val 의 특정 비트를 켬.

	if(fcntl(fd, F_SETFL, val)<0){ //fd의 파일 상태 플래그를 arg인자에 지정한 플래그 값으로 재설정
		fprintf(stderr, "fcntl F_SETFL failed");
		exit(1);
	}
}

void clr_flags(int fd, int flags){
	int val;
	if((val = fcntl(fd, F_GETFL, 0))<0){
		fprintf(stderr, "fcntl F_GETFL failed");
		exit(1);
	}

	val &= ~flags;
	if(fcntl(fd, F_SETFL, val)<0){
		fprintf(stderr, "fcntl F_SETFL failed");
		exit(1);
	}
}

