#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

void ssu_setbuf(FILE *fp, char *buf);

int main(void){
	//버퍼 및 터미널 변수 선언
	char buf[BUFFER_SIZE];
	char *fname = "/dev/pts/0"; 
	FILE *fp;

	if((fp=fopen(fname,"w"))==NULL){
		fprintf(stderr,"fopen error for %s", fname);
		exit(1);
	}

	//터미널장치파일에 버퍼 적용
	ssu_setbuf(fp,buf);
	fprintf(fp,"Hello, ");
	sleep(1);
	fprintf(fp,"UNIX!!");
	sleep(1);
	fprintf(fp,"\n");
	sleep(1);

	//버퍼 해제
	ssu_setbuf(fp,NULL);
	fprintf(fp,"HOW");
	sleep(1);
	fprintf(fp," ARE");
	sleep(1);
	fprintf(fp," YOU?");
	sleep(1);
	fprintf(fp, "\n");
	sleep(1);

	exit(0);
}

void ssu_setbuf(FILE *fp, char *buf){
	size_t size;
	int fd;
	int mode;

	fd = fileno(fp);
	
	// 파일 디스크립터가 터미널에 연결되어 있는지 확인하여 버퍼링 모드 설정
	if(isatty(fd))
		mode = _IOLBF;
	else
		mode = _IOFBF;

	if(buf==NULL){
		mode = _IONBF;
		size=0;
	}
	else
		size = BUFFER_SIZE;

	setvbuf(fp,buf,mode,size);
}

