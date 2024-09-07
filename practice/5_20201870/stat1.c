#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char *argv[]){
	//stat 저장할 구조체 선언
	struct stat statbuf;
	
	//인자 2개인지 확인
	if(argc!=2){
		fprintf(stderr, "usage : %s <file>\n", argv[0]);
		exit(1);
	}

	//stat 가져오기
	if((stat(argv[1], &statbuf))<0){
		fprintf(stderr, "stat error\n");
		exit(1);
	}

	//인자로 받은 파일의 크기 출력
	printf("%s is %ld bytes\n",argv[1], statbuf.st_size);
	exit(0);

}
