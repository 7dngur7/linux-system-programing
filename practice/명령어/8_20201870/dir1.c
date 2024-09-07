#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#define DIRECTORY_SIZE MAXNAMLEN

int main(int argc, char *argv[]){
	//변수 및 구조체변수 선언
	struct dirent *dentry;
	struct stat statbuf;
	char filename[DIRECTORY_SIZE+1];
	DIR *dirp;
	//인자 개수 확인
	if(argc<2){
		fprintf(stderr, "usage: %s <directory>\n", argv[0]); 
		exit (1);
	}
	//디렉토리를 열거나 옮기는데 오류 확인
	if((dirp=opendir(argv[1]))<0 || chdir(argv[1])<0){
		fprintf(stderr, "opendir, chdir error for %s\n", argv[1]);
		exit(1);
	}

	//디렉토리 내부 엔트리 순회
	while ((dentry = readdir(dirp))!=NULL){
		if(dentry->d_ino == 0) continue;

		//파일 이름과 stat 구조체 가져오기
		memcpy(filename, dentry->d_name, DIRECTORY_SIZE);

		if(stat(filename, &statbuf)<0){
			fprintf(stderr, "stat error for %s\n", filename); 
			break;
		}

		//파일일 경우 이름과 크기 출력
		if((statbuf.st_mode & S_IFMT) == S_IFREG){
			printf ("%-14s %ld\n", filename, statbuf.st_size) ;
		}
		//아닐 경우 이름 출력
		else
			printf("%-14s\n", filename);

	}
	exit(0);
}

