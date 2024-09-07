#include <stdio.h> 
#include <stdlib.h>

void ssu_out(void);

int main(void){
	//ssu_out을 종료 핸들러로 등록
	if (atexit(ssu_out)) {
		fprintf(stderr, "atexit errorin");
		exit (1); 
	}
	exit(0); 
}

void ssu_out(void) {
	printf("atexit succeeded! \n"); 
}
