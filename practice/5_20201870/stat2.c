#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

//stat 저장할 구조체 선언
struct stat statbuf;

void ssu_checkfile(char *fname, time_t *time);

int main(int argc, char *argv[]){
	//변경시간 저장하는 변수 선언
	time_t intertime;

	//인자 두개인지 확인
	if(argc!=2){
		fprintf(stderr, "usage: %s <file>\n", argv[0]);
		exit(1);
	}

	//stat 가져오기
	if((stat(argv[1], &statbuf))<0){
		fprintf(stderr, "stat error for %s\n", argv[1]);
		exit(1);
	}

	//파일 변경시간 가져오기
	intertime = statbuf.st_mtime;
	//10초 주기로 ssu_checkfile 확인
	while(1){
		ssu_checkfile(argv[1], &intertime);
		sleep(10);
	}

}


void ssu_checkfile(char *fname, time_t *time){
	//stat을 정상적으로 다시 가져오는지 확인
	if((stat(fname, &statbuf))<0){
		fprintf(stderr, "Warning : ssu_checkfile() error!\n");
		exit(1);
	}

	//파일 변경시간이 달라졌다면 파일명 출력후 시간 업데이트
	if(statbuf.st_mtime != *time){
		printf("Warning : %s was modified!.\n", fname);
		*time = statbuf.st_mtime;
	}

}


