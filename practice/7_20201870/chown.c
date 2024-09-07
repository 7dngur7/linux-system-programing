#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <fcntl.h>
#include <sys/stat.h>

int main(void){
	//구조체버퍼와 변수 선언
	struct stat statbuf;
	char *fname= "ssu_myfile";
	int fd;

	//파일 open
	if ((fd = open(fname, O_RDWR | O_CREAT, 0600)) < 0){
	       	fprintf(stderr, "open error for %s\n", fname ); 
		exit(1);
	}
	//파일 닫음
	close (fd);
	stat (fname, &statbuf);

	//첫번째 uid gid 출력
	printf("# 1st stat call # UID:%d 	GID:%d\n", statbuf.st_uid, statbuf.st_gid);
	
	//소유자 변경
	if (chown(fname,501, 300)< 0) {
		fprintf(stderr, "chown error for %s\n", fname);
	     	exit(1); 
	}

	stat (fname, &statbuf);
	//두번째 uid gid 출력
	printf("# 2nd stat call # UID:%d	GID%d\n", statbuf.st_uid,statbuf.st_gid);
	if (unlink(fname) < 0) {
		fprintf(stderr, "unlink error for %s\n" , fname); 
		exit (1);
	}
	exit(0);
}
