#include <stdio.h>

void addhelp(void){
	printf(" add <PATH> [OPTION]... : add new daemon process of <PATH> if <PATH> is file\n-d : add new daemon process of <PATH> if <PATH> is directory\n-r : add new daemon process of <PATH> recursive if <PATH> is directory\n-t <TIME> : set daemon process time to <TIME> sec (default : 1sec)\n");
}

void removehelp(void){
	printf(" remove <DAEMON_PID> : delete daemon process with <DAEMON_PID>\n");
}

void listhelp(void){
	printf(" list [DAEMON_PID] : show daemon process list or dir tree\n");
}

void helphelp(void){
	printf(" help [COMMAND] : show commands for program\n");
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
	listhelp();
	printf(" >");
	helphelp();
	printf(" >");
	exithelp();
}

