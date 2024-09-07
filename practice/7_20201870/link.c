#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	//인자 개수 확인
	if(argc<3){
		fprintf(stderr, "usage: %s <filel > <file2>\n", argv[0]); 
		exit (1);
	}
	
	//link 수행
	if(link(argv[1], argv[2]) <0){
		fprintf (stderr, "link error for %s \n", argv[1]);
		exit (1);
	}
	exit(0);
}
