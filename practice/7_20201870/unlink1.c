#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(void){
	char *fname = "dump.txt";
	//파일 open
	if(open(fname, O_RDWR)<0){
		fprintf(stderr, "open error for %s\n", fname);
		exit(1);
	}
	//파일 삭제
	if(unlink(fname)<0){
		fprintf( stderr, "unlink error for %s\n", fname);
		exit(1);
	}
	printf("File Unlinked\n");
	//20초 대기
	sleep(20);
	printf("Done\n");
	exit(0);

}
