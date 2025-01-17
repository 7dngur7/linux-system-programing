#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024

int main(void){
	char *fname = "ssu_test.txt";
	char buf[BUFFER_SIZE];
	int fd1, fd2;
	int count;

	fd1 = open(fname, O_RDONLY);
	fd2 = open(fname, O_RDONLY);
	
	if(fd1<0||fd2<0){
		fprintf(stderr, "open error for %s\n", fname);
		exit(1);
	}

	count = read(fd1, buf, 25);
	buf[count]=0;
	printf("fd1's first printf : %s\n", buf);
	lseek(fd1, 1, SEEK_CUR);
	count = read(fd1, buf, 24);
	buf[count]=0;
	printf("fd1's second printf : %s\n", buf);
	count = read(fd2, buf, 25);
	buf[count]=0;
	printf("fd2's first printf : %s\n", buf);
	lseek(fd2, 1, SEEK_CUR);
	count = read(fd2, buf, 25);
	buf[count]=0;
	printf("fd2's second printf : %s\n", buf);

	exit(0);
}


