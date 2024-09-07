#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int main(void){
	//변수 선언
	char *fname = "ssu_test2.txt";
	int fd;

	printf ("First printf : Hello, OSLAB!!\n");
	//파일 open
	if((fd = open(fname, O_RDONLY))<0){
		fprintf(stderr, "open error for %s\n", fname);
		exit(1);
	}

	//표준출력을 통해 파일에 입력받도록 freopen
	if(freopen(fname, "wd", stdout) != NULL){
		printf ("Second printf : Hello, OSLAB!!\n");	
	}
	exit(0);

}
