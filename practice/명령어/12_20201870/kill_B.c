#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void){
	//5초마다 반복
	while(1){
		printf("\n[OSLAB]");
		sleep(5);
	}
	exit(0);
}
