#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

//실행권한
#define STAT_MODE (S_IXUSR|S_IXGRP|S_IXOTH)

int main(int argc, char *argv[]){
	//구조체 버퍼 선언
	struct stat statbuf;
	
	//인자개수 확인
	if(argc<2){
		fprintf(stderr, "usage: %s <file1><file2> . . . <fileN>\n", argv[0]);
		exit(1);
	}
	
	//입력된 경로 돌면서 권한 변경
	for(int i=1; i<argc; i++){
		if(stat(argv[i], &statbuf)<0){
			fprintf(stderr, "%s: stat error\n", argv[i]);
			continue;
		}	

		//실행권한에 STAT_MODE추가
		statbuf.st_mode |=STAT_MODE;
		//그룹과 other에 대해 실행권한 토글
		statbuf.st_mode ^=(S_IXGRP|S_IXOTH);

		if(chmod(argv[i], statbuf.st_mode)<0){
			fprintf(stderr, "%s: chmod error\n", argv[i]);
		}
		else{
			printf("%s: file permission was changed.\n", argv[i]);
		}
	}
	exit(0);
}
