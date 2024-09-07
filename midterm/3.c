#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <utime.h>
#include <ctype.h>

#define TABLE_SIZE 128
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]){
	static struct{
		long offset;
		int length;

	}table [TABLE_SIZE];
        char buf[BUFFER_SIZE];
	int length=0;
	int fd;

	if(argc<2){
		fprintf(stderr, "usage: %s <file>\n", argv[0]);
		exit(1);		
	}




	if((fd = open(argv[1], O_RDONLY))<0){
		fprintf(stderr, "open err \n");
		exit(1);
	}

	long offset = 0;
	int num;
	long myoffset = 0;
	while(num =read(fd, buf, BUFFER_SIZE)){
		for(int i =0; i<num; i++){
		       
			table[offset].length++;
			myoffset++;

			if(buf[i]=='\n'){
				table[++offset].offset = myoffset;

			}
		}
	}






	while(1){
		int num;
		printf("Enter line number : ");
		scanf("%d", &num);
		
		if(--num<0){
			break;
		}

		char buf2[BUFFER_SIZE];
		lseek(fd, table[num].offset, SEEK_SET);

		read(fd, buf2, table[num].length);
		
		buf2[table[num].length] = '\0';

		printf("%s", buf2);

	}


	close(fd);
	exit(0);


}       
