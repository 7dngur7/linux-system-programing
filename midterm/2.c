#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <utime.h>
#include <ctype.h>

int main(int argc , char*argv[]){
	char operator;
	FILE *fp;
	char *path = "ssu_expr.txt";
	int character;
	if((fp = fopen(path, "rb"))==NULL){

		fprintf(stderr, "open err\n");
		exit(1);
	}
	int num = 0;
	while(!feof(fp)){
		while ((character = fgetc(fp))!=EOF && isdigit(character)){
			num = num*10 + character - 48;
		}

		printf("%d\n", num);
		num = 0;

		if(character !=EOF){
			ungetc(character, fp);
			operator = fgetc(fp);

			printf("Operator => %c\n", operator);

		}	

	}

	fclose(fp);
	exit(0);
}       
