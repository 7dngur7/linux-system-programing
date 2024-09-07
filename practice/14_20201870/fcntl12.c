#include <stdio.h> //입출력에 관련한 헤더.
#include <stdlib.h> //유틸리티 함수와 메모리 관리 함수를 포함하는 헤더. 동적 메모리 할당, 문자열 변환, 난수 생성 등.
#include <unistd.h> //unix OS에 사용되는 상수를 포함하는 헤더
#include <fcntl.h> //fcntl함수에 관한 헤더

#define NAMESIZE 64

struct employee{
	char name[NAMESIZE];
	int salary;
	int pid;
};

int main(int argc, char *argv[]){
	struct flock lock;
	struct employee record;
	int fd, recnum, pid;
	long position;
	char ans[5];

	if((fd = open(argv[1], O_RDWR))==-1){
		perror(argv[1]);
		exit(1);
	}

	pid = getpid();
	for(;;){
		printf("\nEnter record number: ");
		scanf("%d", &recnum);
		if(recnum <0)
			break;
	
		position = recnum * sizeof(record);// 해당 레코드 위치 계산
		lock.l_type = F_RDLCK;
		lock.l_whence = 0;
		lock.l_start = position;
		lock.l_len = sizeof(record);

		if(fcntl(fd, F_SETLKW, &lock) ==-1){// 잠금 설정 시도
			perror(argv[1]);
			exit(2);
		}
		lseek(fd, position, 0);
		if(read(fd, (char *)&record, sizeof(record))==0){
			printf("record %d not found\n",recnum);
			lock.l_type = F_UNLCK;
			fcntl(fd, F_SETLK, &lock);
			continue;
		}
		printf("Employee: %s, salary: %d\n", record.name, record.salary);
		printf("Do you want to update salary (y or n)? ");
		scanf("%s", ans);

		if(ans[0]!='y'){
			lock.l_type = F_UNLCK;
			fcntl(fd, F_SETLK, &lock);
			continue;
		}
		lock.l_type = F_WRLCK;
		if(fcntl(fd, F_SETLKW, &lock)==-1){// 잠금 설정 시도
			perror(argv[1]);
			exit(3);
		}
		record.pid = pid;
		printf("Enter new salary: ");
		scanf("%d", &record.salary);

		lseek(fd, position, 0);
		write(fd, (char *)&record, sizeof(record));

		lock.l_type = F_UNLCK;
		fcntl(fd,F_SETLK,&lock);// 잠금 해제 시도
	}
	close(fd);
}

