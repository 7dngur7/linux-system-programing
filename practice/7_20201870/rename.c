#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h> 
#include <fcntl.h>

int main(int argc, char *argv[]){
	int fd;
	//인자 개수 확인
	if (argc != 3) {
		fprintf(stderr, "usage: %s <oldname> <newname> \n", argv[0]);
		exit(1);
	}

	//파일 open
	if ((fd = open (argv[1], O_RDONLY)) < 0){
	       	fprintf (stderr, "first open error for %s\n", argv[1]);
		exit (1);
		}
	else{
		close (fd);
	}
	//파일 이름 변경
	if (rename(argv[1], argv[2]) < 0) { 
		fprintf(stderr, "rename error\n");
		exit (1);
	}

	//첫 번째 이름으로 파일 open
	if ((fd = open(argv[1], O_RDONLY)) < 0){
	       	printf("second open error for %s\n", argv[1]);
	}
	else {
		fprintf(stderr, "it's very strange!\n");
		exit (1); 
	}

	//두 번째 이름으로 파일 open
	if ((fd = open(argv[2], O_RDONLY)) < 0) {
		fprintf(stderr, "third open error for %s\n" , argv[2]);
		exit(1); 
	}
	printf("Everything is good!\n"); 
	exit(0);
}

