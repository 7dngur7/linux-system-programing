#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	//인자 개수 확인
	if(argc != 3){
		fprintf(stderr, "usage: %s <oldname> <newname>\n", argv[0]); 
		exit (1);
	}

	//파일 링크
	if(link(argv[1], argv[2])<0){
		fprintf(stderr, "link error\n"); 
		exit (1);
	}
	else{
		printf("step1 passed.\n");
	}

	//기존 파일 삭제
	if(remove(argv[1])<0){
		fprintf(stderr, "remove error\n");
		remove (argv[2]); 
		exit (1);
	}
	else{
		printf("step2 passed.\n");
	}
	printf("Success!\n");
	exit(0);

}
