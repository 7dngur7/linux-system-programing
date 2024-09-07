#include <stdio.h>
#include <stdlib.h>

int main(void){
	//변수 선언
	char *fname = "ssu_test.txt";
	char *mode = "r";

	//파일 fopen
	if(fopen(fname, mode)==NULL){
		fprintf(stderr, "fopen fail\n");
		exit(1);
	}
	//성공시 성공메시지 출력
	else{
		printf("Success!\nFilename: <%s>, mode: <%s>\n", fname, mode);
	}
	exit(0);
}
