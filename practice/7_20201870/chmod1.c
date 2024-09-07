#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(void){
	//파일 이름과 구조체 선언
	struct stat statbuf;
	char *path1 = "ssu_file1";
	char *path2 = "ssu_file2";

	//path1 stat가져옴
	if(stat(path1, &statbuf)<0){
		fprintf(stderr, "stat fail\n");
	}

	//비트연산자를 이용해 모드 변경
	if(chmod(path1, (statbuf.st_mode & ~S_IXGRP) | S_ISUID)<0){
		fprintf(stderr, "chmod fail\n");
	}

	//모드 변경
	if(chmod(path2, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | S_IXOTH)<0){
		fprintf(stderr, "chmod fail\n");
	}
	
	exit(0);

}
