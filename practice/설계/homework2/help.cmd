#include "ssu_header.h"

int main(int argc, char*argv[]){
	if(path==0){
			help();
		}
		else{
			if(!strcmp(path, "add")){
			printf("Usage: ");
			addhelp();
			}
			else if(!strcmp(path, "remove")){
			printf("Usage: ");
			removehelp();
			}
			else if(!strcmp(path, "status")){
			printf("Usage: ");
			statushelp();
			}
			else if(!strcmp(path, "commit")){
			printf("Usage: ");
			commithelp();
			}
			else if(!strcmp(path, "revert")){
			printf("Usage: ");
			reverthelp();
			}
			else if(!strcmp(path, "log")){
			printf("Usage: ");
			loghelp();
			}
			else if(!strcmp(path, "help")){
			printf("Usage: ");
			helphelp();
			}
			else if(!strcmp(path, "exit" )){
			printf("Usage: ");
			exithelp();
			}
			else{
				fprintf(stderr, "ERROR : wrong command\n");
			}
		}
}		
