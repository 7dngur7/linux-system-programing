#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	//변수 선언
	int i;
	//인자 개수 확인
	if(argc<2){
		fprintf(stderr, "usage: %s <file1> <file2> .. <fileN>\n", argv[0]);
		exit(1);
	}

	//인자로 들어온 파일 별 접근권한 확인
	for(i = 1; i<argc; i++){
		if((access(argv[i], F_OK))<0){
			fprintf(stderr, "%s doesn't exist. \n" , argv[i]);
			continue;
		}
		//읽기권한
		if((access(argv[i], R_OK))==0){
			printf("User can read %s\n" , argv[i]);		
		}
		//쓰기권한
		if((access(argv[i], W_OK))==0){
			printf("User can write %s\n" , argv[i]);
		}
		//실행권한
		if((access(argv[i], X_OK))==0){
			printf("User can execute %s\n" , argv[i]);
		}
	}
	exit(0);
}
