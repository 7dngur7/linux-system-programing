#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024
int main(void){
	//변수 및 버퍼 선언
	char buf[BUFFER_SIZE];
	int fd1, fd2;
	int count;
	char *filename = "ssu_test.txt";

	//파일 open
	if((fd1 = open(filename, O_RDONLY))<0){
		fprintf(stderr, "open error for %s\n", filename);
		exit(1);
	}

	//fd2로 파일디스크립터 복사
	fd2 = dup(fd1);
	//12바이트 fd1으로 read 후 출력
	count = read(fd1, buf, 12);
	buf[count]=0;
	printf("fd1's printf: %s\n", buf);
	
	// 한 바이트 뒤로 오프셋 이동
	lseek(fd1, 1, SEEK_CUR);

	//12바이트 fd2로 read 후 출력
	count = read(fd2, buf, 12); 
	buf[count]=0;
	printf("fd2's printf: %s\n", buf);
	exit(0);
}
