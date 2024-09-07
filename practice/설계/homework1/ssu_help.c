#include <stdio.h>

void backuphelp(void){
	printf(" backup <PATH> [OPTION]... : backup file if <PATH> is file\n");
	printf("   -d : backup files in directory if <PATH> is directory\n");
	printf("   -r : backup files in directory recursive if <PATH> is directory\n");
	printf("   -y : backup file although already backuped\n");

}

void removehelp(void){
	printf(" remove <PATH> [OPTION]... : remove backuped file if <PATH> is file\n");
	printf("   -d : remove backuped files in directory if <PATH> is directory\n");
	printf("   -r : remove backuped files in directory recursive if <PATH> is directory\n");
	printf("   -a : remove all backuped files\n");

}

void recoverhelp(void){
	printf(" recover <PATH> [OPTION]... : recover backuped file if <PATH> is file\n");
	printf("   -d : recover backuped files in directory if <PATH> is directory\n");
	printf("   -r : recover backuped files in directory recursive if <PATH> is directory\n");
	printf("   -l : recover latest backuped file\n");
	printf("   -n <NEW_PATH> : recover backuped file with new path\n");

}

void listhelp(void){
 	printf(" list [PATH] : show backup list by directory structure\n");
	printf("   >> rm <INDEX> [OPTION]... : remove backuped files of [INDEX] with [OPTION]\n");
	printf("   >> rc <INDEX> [OPTION]... : recover backuped files of [INDEX] with [OPTION]\n");
	printf("   >> vi(m) <INDEX> : edit original file of [INDEX]\n");
	printf("   >> exit : exit program\n");

}

void helphelp(void){
	printf(" help [COMMAND] : show commands for program\n");
}

void help(void){
	printf("\n >");
	backuphelp();
	printf(" >");
	removehelp();
	printf(" >");
	recoverhelp();
	printf(" >");
	listhelp();
	printf(" >");
	helphelp();
}
