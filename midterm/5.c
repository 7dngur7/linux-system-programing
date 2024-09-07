#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <utime.h>
#include <ctype.h>
#include <time.h>


int main(int argc , char*argv[]){
	DIR *dir;
	struct dirent *dentry;
	char path[4096];
	struct stat statbuf;

	printf("Enter your file name: ");
	scanf("%s", path);

	/*if((dir = opendir(path))==NULL){

		fprintf(stderr, "opendir err\n");
		exit(1);
	}*/
		
	if(lstat(path, &statbuf)<0){
		fprintf(stderr, "lstat err\n");
		exit(1);
	}

	printf("File Type: ");
	if(S_ISREG(statbuf.st_mode)){
		printf("Regular file\n");

	}
	else if(S_ISDIR(statbuf.st_mode)){
		printf("Directory\n");

	}
	else if(S_ISLNK(statbuf.st_mode)){
		printf("Directory\n");

	}

	printf("Owner Permission:\n");
	if(statbuf.st_mode & S_IRUSR){
		printf("Read Permission bit set\n");

	}
	if(statbuf.st_mode & S_IWUSR){
		printf("Write Permission bit set\n");

	}
	if(statbuf.st_mode & S_IXUSR){
		printf("Execute Permission bit set\n");

	}
	
	printf("File Size: %ld bytes\n", statbuf.st_size);
	
	printf("Last Modification Time: %s", ctime(&statbuf.st_mtime));

	
	
}       
