#define OPENSSL_API_COMPAT 0x10100000L
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>
#include <openssl/md5.h>
#include <limits.h>
#include <getopt.h>

#define PATHMAX 4096
#define STRMAX 4096
#define HASH_MD5  33

int hash;


typedef struct backuplinkedlist{
	char originalpath[PATHMAX];
	char backuppath[PATHMAX];
	struct stat statbuf;
	struct backuplinkedlist *next;
}backuplinkedlist;


typedef struct bfslinkedlist{
	char dirpath[PATHMAX];
	char backuppath[PATHMAX];
	struct bfslinkedlist *next;
}bfslinkedlist;

char *backuppath = "/home/backup";
char *ssubakpath = "/home/backup/ssubak.log";
backuplinkedlist *head = NULL;
bfslinkedlist *bfshead = NULL;

void help();
void backuphelp();
void removehelp();
void recoverhelp();
void listhelp();
void helphelp(); 
