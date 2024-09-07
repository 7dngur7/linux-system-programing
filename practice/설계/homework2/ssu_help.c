#include <stdio.h>

void addhelp(void){
	printf(" add <PATH> : record path to staging area, path will tracking modification\n");
}

void removehelp(void){
	printf(" remove <PATH> : record path to staging area, path will not tracking modification\n");
}

void statushelp(void){
	printf(" status : show staging area status\n");


}

void commithelp(void){
 	printf(" commit <NAME> : backup staging area with commit name\n");
}
void reverthelp(void){
 	printf(" revert <NAME> : recover commit version with commit name\n");
}
void loghelp(void){
 	printf(" log : show commit log\n");
}

void helphelp(void){
	printf(" help : show commands for program\n");
}

void exithelp(void){
	printf(" exit : exit program\n");
}


void help(void){
	printf("Usage:\n >");
	addhelp();
	printf(" >");
	removehelp();
	printf(" >");
	statushelp();
	printf(" >");
	commithelp();
	printf(" >");
	reverthelp();
	printf(" >");
	loghelp();
	printf(" >");
	helphelp();
	printf(" >");
	exithelp();
}

