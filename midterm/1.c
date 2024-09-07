#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <utime.h>
#include <ctype.h>

#define MODE_EXEC (S_IXUSR|S_IXGRP|S_IXOTH)


int main(int argc , char*argv[]){
        struct stat statbuf;
	for(int i = 1; i< argc;i++){

		if(stat(argv[i], &statbuf)<0){
			fprintf(stderr, "stat fail\n");
			exit(1);
		}

		statbuf.st_mode |= MODE_EXEC;
		statbuf.st_mode ^= (S_IXGRP | S_IXOTH);

		if(chmod(argv[i], statbuf.st_mode)<0){
			fprintf(stderr, "%s : chmod error\n", argv[i]);
			exit(1);
		}
		else{
			printf("%s : file permission was changed.\n", argv[i]);
		}


	}



	exit(0);
}       
