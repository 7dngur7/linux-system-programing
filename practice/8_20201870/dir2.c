#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>

//파일 시스템의 최대길이
#ifdef PATH_MAX
static int pathmax = PATH_MAX;
#else
static int pathmax = 0;
#endif

#define MAX_PATH_GUESSED 1024

//한줄 최대 길이
#ifndef LINE_MAX
#define LINE_MAX 2048
#endif

char *pathname;
char command [LINE_MAX], grep_cmd[LINE_MAX] ;

//디렉토리에 grep 명령어 실행
int do_grep(){
	struct dirent *dentry;
	DIR *dp;
	struct stat statbuf;
	char *ptr;

	//stat 가져옴
	if(lstat(pathname, &statbuf)<0){
		fprintf(stderr, "lstat error for %s\n", pathname); 
		return 0;
	}

	//디렉토리가 아닐경우 grep 작동
	if(S_ISDIR(statbuf.st_mode)==0){
		sprintf(command,"%s %s", grep_cmd,pathname);
		printf ("%s : \n", pathname);
		system(command);
		return 0;
	}

	ptr = pathname + strlen (pathname) ;
	*ptr++ = '/'; 
	*ptr = '\0';

	//디렉토리 내부의 파일들을 검색
	if((dp = opendir(pathname))==NULL){
		fprintf(stderr, "opendir error for %s\n" , pathname);
		return 0;
	}
	while((dentry = readdir(dp))!=NULL){
		if(strcmp(dentry->d_name, ".") && strcmp(dentry->d_name, "..")){
			strcpy (ptr, dentry->d_name);
			if(do_grep()<0) break;
		}
	}
	ptr[-1] = 0;
	closedir(dp);
	return 0;
}



//grep 명령 문자열 생성
void make_grep(int argc, char *argv[]){
	strcat(grep_cmd, " grep");

	for(int i = 1; i<argc-1; i++){
		strcat(grep_cmd, " ");
		strcat(grep_cmd, argv[i]);
	}
}

int main(int argc, char *argv[]){
	//인자 개수 검사
	if(argc<2){
		fprintf(stderr, "usage: %s <-CVbchilnsvwx> <-num> <-A num> <-B num> <-f file> \n"
	"	<e> expr <directory>\n", argv[0]);
		exit(1);
	}

	//최대 경로 길이 설정
	if(pathmax == 0){
		if ((pathmax=pathconf("/", _PC_PATH_MAX)) <0)
		       	pathmax = MAX_PATH_GUESSED;
		else pathmax++;
	}
	
	//경로 메모리 할당
	if((pathname = (char *)malloc(pathmax+1))==NULL){
		fprintf(stderr, "malloc error\n"); 
		exit(1);
	}
	strcpy(pathname, argv[argc-1]);
	make_grep(argc, argv);
	
	do_grep();
	exit(0);
}
