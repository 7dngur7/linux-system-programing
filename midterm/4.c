#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <utime.h>
#include <ctype.h>
#include <string.h>

#define BUFFER_SIZE 1024
#define WORD_MAX 128

int main(int argc , char*argv[]){
	int fd;
	int length = 0, offset =0, count = 0; 	
	char buf[BUFFER_SIZE];
	if((fd = open(argv[1], O_RDONLY))<0){
		fprintf(stderr, "open err\n");
		exit(1);
	}

	length = lseek(fd, 0, SEEK_END);
	
	read(fd, buf, BUFFER_SIZE);
		lseek(fd, offset, SEEK_SET);
		for( int i=0; i<length; i++){
			if(buf[i] == '\n'){
				char buf2[WORD_MAX];
				lseek(fd, offset, SEEK_SET);
				read(fd, buf2, i-offset);
				
				printf("%s", buf2);
				count++;
				offset = i;
			}
			
	//		if(buf[offset]==EOF) break;
		
		}
	

	

	printf("\nline number : %d\n", count);


	exit(0);
}       
