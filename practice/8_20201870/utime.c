#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <utime.h>

int main(int argc, char *argv[]){
	//변수 및 구조체변수 선언
	struct utimbuf time_buf;
	struct stat statbuf;
	int fd;
	
	//입력받은 인자수를 통해 반복
	for(int i= 1; i<argc; i++){
		//stat 가져옴
		if(stat(argv[i], &statbuf)<0){
			fprintf(stderr, "stat error for %s\n", argv[i]); 
			continue;
		}
		
		//파일 open 후 close
		if((fd = open(argv[i], O_RDWR | O_TRUNC))<0){
			fprintf(stderr, "open error for %s\n", argv[i]); 
			continue;
		}

		close(fd);

		//파일의 원래 접근 시간과 수정시간을 구조체에 넣음
		time_buf.actime = statbuf.st_atime;
		time_buf.modtime = statbuf.st_mtime;
		
		//접근시간과 수정시간 변경
		if(utime(argv[i], &time_buf)<0){
			fprintf(stderr, "utime error for %s\n", argv[i]); 
			continue;
		}
	}

	exit(0);

}
