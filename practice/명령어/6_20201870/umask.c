#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

//읽기 쓰기권한 설정 하는 모드 정의
#define RW_MODE (S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP | S_IROTH | S_IWOTH)
int main(int argc, char *argv[]){
	//파일 이름 선언
	char *fname1 = "ssu_file1";
	char *fname2 = "ssu_file2";

	//umask 0으로 변경
	umask(0);
	//파일1 생성
	if(creat(fname1, RW_MODE)<0){
		fprintf(stderr, "creat error for %s\n", fname1);
		exit(1);
	}

	//umask 변경
	umask(S_IWGRP | S_IRGRP | S_IROTH | S_IWOTH);
	//파일2 생성
	if(creat(fname2, RW_MODE)<0){
		fprintf(stderr, "creat error for %s\n", fname2);
		exit(1);
	}

	exit(0);
}
