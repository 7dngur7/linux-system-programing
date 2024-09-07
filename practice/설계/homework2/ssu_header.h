#define OPENSSL_API_COMPAT 0x10100000L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <openssl/md5.h>
#include <utime.h>
#include <sys/wait.h>

#include <sys/sysinfo.h>

#define DEBUG printf("DEBUG\n");

#define PATH_MAX 4096
#define BUF_MAX 4096
#define FILE_MAX 256

#define CMD_BACKUP    0b00001
#define CMD_REMOVE    0b00010
#define CMD_RECOVER   0b00100
#define CMD_LIST      0b01000
#define CMD_HELP      0b10000

#define OPT_D         0b000001
#define OPT_R         0b000010
#define OPT_Y         0b000100
#define OPT_A         0b001000
#define OPT_L         0b010000
#define OPT_N         0b100000
/**/
#define BACKUP_PATH       "/home/backup/"
#define BACKUP_LOG_PATH   "/home/backup/ssubak.log"
/**/
#define REPO_PATH	"./.repo"
#define COMMIT_LOG_PATH "./.repo/commit.log"
#define STAGING_LOG_PATH "./.repo/staging.log"

#define BACKUP_SEP        "\" backuped to \""
#define REMOVE_SEP        "\" removed by \""
#define RECOVER_SEP       "\" recovered to \""

#define HASH_MD5  33

typedef struct _backupNode {
  struct _dirNode *root_version_dir;
  struct _fileNode *root_file;

  char backup_time[13];
  char origin_path[PATH_MAX];
  char backup_path[PATH_MAX];

  struct _backupNode *prev_backup;
  struct _backupNode *next_backup;
} backupNode;

typedef struct _fileNode {
  struct _dirNode *root_dir;

  int backup_cnt;
  char file_name[FILE_MAX];
  char file_path[PATH_MAX];
  backupNode *backup_head;

  struct _fileNode *prev_file;
  struct _fileNode *next_file;
} fileNode;

typedef struct _dirNode {
  struct _dirNode *root_dir;

  int file_cnt;
  int subdir_cnt;
  int backup_cnt;
  char dir_name[FILE_MAX];
  char dir_path[PATH_MAX];
  fileNode *file_head;
  struct _dirNode *subdir_head;

  struct _dirNode* prev_dir;
  struct _dirNode *next_dir;
} dirNode;

typedef struct _pathNode {
  char path_name[FILE_MAX];
  int depth;

  struct _pathNode *prev_path;
  struct _pathNode *next_path;

  struct _pathNode *head_path;
  struct _pathNode *tail_path;
} pathNode;

dirNode* backup_dir_list;
dirNode *version_dir_list;

char *exe_path;
char *pwd_path;
char *home_path;

void help();
void addhelp(void);
void removehelp(void);
void statushelp(void);
void commithelp(void);
void reverthelp(void);
void loghelp(void);
void helphelp(void);
void exithelp(void);

int help_process(int argc, char* argv[]);


