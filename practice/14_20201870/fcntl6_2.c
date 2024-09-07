#include <stdio.h> //입출력에 관련한 헤더.
#include <stdlib.h> //유틸리티 함수와 메모리 관리 함수를 포함하는 헤더. 동적 메모리 할당, 문자열 변환, 난수 생성 등.
#include <unistd.h> //unix OS에 사용되는 상수를 포함하는 헤더
#include <fcntl.h> //fcntl함수에 관한 헤더
#include <sys/stat.h> // 파일 및 파일 시스템에 대한 정보를 얻기 위해 사용하는 시스템 상태(stat)관련 함수와 상수 정의하는 헤더.
#include <sys/types.h> //데이터 유형에 관련된 정의를 포함하는 헤더
int main(void){
	int fd;
	int val;
	
	if((fd = open("exec_copy.txt",O_CREAT))<0){
		fprintf(stderr, "open error for %s\n", "exec_copy.txt");
		exit(1);
	}
	val = fcntl(fd, F_GETFD, 0);

	if(val & FD_CLOEXEC)
		printf("close-on-exec bit on\n");
	else
		printf("close-on-exec bit off\n");

	val |= FD_CLOEXEC; //플래그 켬

	if(val & FD_CLOEXEC)
		printf("close-on-exec bit on\n");
	else
		printf("close-on-exec bit off\n");

	fcntl(fd, F_SETFD, val); //fd에 있는 상태 플래그를 val로 재설정
	execl("/home/oslab/loop", "./loop",NULL);
}

