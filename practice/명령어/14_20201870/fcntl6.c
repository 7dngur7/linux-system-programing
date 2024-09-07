#include <stdlib.h> 
#include <unistd.h> 
#include <fcntl.h> //fcntl함수에 관한 헤더
#include <sys/stat.h> 
#include <sys/types.h>
int main(void){
	int fd;
	int val;

	fd = open("exec_copy.txt", O_CREAT);
	//execl로 무한루프 실행
	execl("/home/woohyuk/Desktop/기본과제/14_20201870/loop","./a.out",NULL);
	exit(0);
}

