#include <stdio.h> //입출력에 관련한 헤더.
#include <stdlib.h> //유틸리티 함수와 메모리 관리 함수를 포함하는 헤더. 동적 메모리 할당, 문자열 변환, 난수 생성 등.
#include <unistd.h> //unix OS에 사용되는 상수를 포함하는 헤더
#include <fcntl.h> //fcntl함수에 관한 헤더
#include <errno.h>//err 상수에 관한 헤더.

#define NAMESIZE 64
#define MAXTRIES 5

struct employee{
	char name[NAMESIZE];
	int salary;
	int pid;
};

int main(int argc, char *argv[]){
	struct flock lock;
	struct employee record;
	int fd, sum=0, try =0;

	sleep(10);

	if((fd = open(argv[1],O_RDONLY))==-1){ //읽기전용으로 파일오픔
		perror(argv[1]);
		exit(1);
	}
	lock.l_type = F_RDLCK;
	lock.l_whence = 0;
	lock.l_start = 0L;
	lock.l_len = 0L;

	while(fcntl(fd, F_SETLK, &lock)==-1){
		if(errno == EACCES){
			if(try++ < MAXTRIES){
				sleep(1);
				continue;
			}
			printf("%s busy -- try later\n",argv[1]);
			exit(2);
		}
		perror(argv[1]);
		exit(3);
	}
	sum=0;
	while(read(fd, (char *)&record, sizeof(record))>0){
		printf("Employee: %s, Salary: %d\n", record.name, record.salary);
		sum+= record.salary;
	}
	printf("\nTotal salary: %d\n", sum);

	lock.l_type = F_UNLCK;
	fcntl(fd, F_SETLK, &lock);
	close(fd);
}

