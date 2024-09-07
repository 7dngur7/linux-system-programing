#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFF_SIZE 1024

int main(void){
	char buf[BUFF_SIZE];
	int length;
	
	length = read(0, buf, BUFF_SIZE);

	write(1, buf, length);

	exit(0);	


}
