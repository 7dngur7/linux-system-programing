#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h> 
#include "ssu_header.h"

//dirNode를 초기화하는 함수
void dirNode_init(dirNode *dir_node) {
  dir_node->root_dir = NULL;

  dir_node->file_cnt = 0;
  dir_node->subdir_cnt = 0;
  dir_node->backup_cnt = 0;

  dir_node->file_head = (fileNode*)malloc(sizeof(fileNode));
  dir_node->file_head->prev_file = NULL;
  dir_node->file_head->next_file = NULL;
  dir_node->file_head->root_dir = dir_node;

  dir_node->subdir_head = (dirNode*)malloc(sizeof(dirNode));
  dir_node->subdir_head->prev_dir = NULL;
  dir_node->subdir_head->next_dir = NULL;
  dir_node->subdir_head->root_dir = dir_node;

  dir_node->prev_dir = NULL;
  dir_node->next_dir = NULL;
}


//dirNode를 검색하는 함수
dirNode *dirNode_get(dirNode* dir_head, char* dir_name) {
  dirNode *curr_dir = dir_head->next_dir;
  
  while(curr_dir != NULL) {
    //printf("dirnodeget test : %s, %ld, %s, dirname : %s\n", curr_dir->dir_path, strlen(curr_dir->dir_path), curr_dir->dir_path, dir_name);
    if(curr_dir != NULL && !strcmp(dir_name, curr_dir->dir_name)) {
      return curr_dir;
    }
    curr_dir = curr_dir->next_dir;
  }
  return NULL;
}

//dirNode를 넣어주는 함수
dirNode *dirNode_insert(dirNode* dir_head, char* dir_name, char* dir_path) {
  dirNode *curr_dir = dir_head;
  dirNode *next_dir, *new_dir;
  
  while(curr_dir != NULL) {
    next_dir = curr_dir->next_dir;

    if(next_dir != NULL && !strcmp(dir_name, next_dir->dir_name)) {
      return curr_dir->next_dir;
    } else if(next_dir == NULL || strcmp(dir_name, next_dir->dir_name) < 0) {
      new_dir = (dirNode*)malloc(sizeof(dirNode));
      dirNode_init(new_dir);

      new_dir->root_dir = dir_head->root_dir;
      
      sprintf(new_dir->dir_name, "%s", dir_name);
      sprintf(new_dir->dir_path, "%s%s/", dir_path, dir_name);

      new_dir->prev_dir = curr_dir;
      new_dir->next_dir = next_dir;
      
      if(next_dir != NULL) {
        next_dir->prev_dir = new_dir;
      }
      curr_dir->next_dir = new_dir;

      dir_head->root_dir->subdir_cnt++;

      return curr_dir->next_dir;
    }
    curr_dir = next_dir;
  }
  return NULL;
}

//dirNode를 끝에 추가하는 함수
dirNode *dirNode_append(dirNode* dir_head, char* dir_name, char* dir_path) {
  dirNode *curr_dir = dir_head;
  dirNode *next_dir, *new_dir;
  
  while(curr_dir != NULL) {
    next_dir = curr_dir->next_dir;
    if(next_dir == NULL) {
      new_dir = (dirNode*)malloc(sizeof(dirNode));
      dirNode_init(new_dir);

      new_dir->root_dir = dir_head->root_dir;
      
      sprintf(new_dir->dir_name, "%s", dir_name);
      sprintf(new_dir->dir_path, "%s%s/", dir_path, dir_name);

      new_dir->prev_dir = curr_dir;
      new_dir->next_dir = next_dir;
      
      if(next_dir != NULL) {
        next_dir->prev_dir = new_dir;
      }
      curr_dir->next_dir = new_dir;

      // dir_head->root_dir->subdir_cnt++;

      return curr_dir->next_dir;
    }
    curr_dir = next_dir;
  }
  return NULL;
}

//dirNode를 삭제하는 함수
void dirNode_remove(dirNode *dir_node) {
  dirNode *next_dir = dir_node->next_dir;
  dirNode *prev_dir = dir_node->prev_dir;

  if(next_dir != NULL) {
    next_dir->prev_dir = dir_node->prev_dir;
    prev_dir->next_dir = dir_node->next_dir;
  } else {
    prev_dir->next_dir = NULL;
  }

  free(dir_node->subdir_head);
  free(dir_node->file_head);
  dir_node->subdir_head = NULL;
  dir_node->file_head = NULL;

  free(dir_node);
}

//fileNode를 초기화하는 함수
void fileNode_init(fileNode *file_node) {
  file_node->root_dir = NULL;

  file_node->backup_cnt = 0;

  file_node->backup_head = (backupNode*)malloc(sizeof(backupNode));
  file_node->backup_head->prev_backup = NULL;
  file_node->backup_head->next_backup = NULL;
  file_node->backup_head->root_file = file_node;

  file_node->prev_file = NULL;
  file_node->next_file = NULL;
}

//fileNode를 찾아오는 함수
fileNode *fileNode_get(fileNode *file_head, char *file_name) {
  fileNode *curr_file = file_head->next_file;
  
  while(curr_file != NULL) {
    if(curr_file != NULL && !strcmp(file_name, curr_file->file_name)) {
      return curr_file;
    }
    curr_file = curr_file->next_file;
  }
  return NULL;
}

//fileNode를 넣어주는 함수
fileNode *fileNode_insert(fileNode *file_head, char *file_name, char *dir_path) {
  fileNode *curr_file = file_head;
  fileNode *next_file, *new_file;
  
  while(curr_file != NULL) {
    next_file = curr_file->next_file;

    if(next_file != NULL && !strcmp(file_name, next_file->file_name)) {
      return curr_file->next_file;
    } else if(next_file == NULL || strcmp(file_name, next_file->file_name) < 0) {
      new_file = (fileNode*)malloc(sizeof(fileNode));
      fileNode_init(new_file);

      new_file->root_dir = file_head->root_dir;

      strcpy(new_file->file_name, file_name);
      strcpy(new_file->file_path, dir_path);
      strcat(new_file->file_path, file_name);
      
      new_file->prev_file = curr_file;
      new_file->next_file = next_file;
      
      if(next_file != NULL) {
        next_file->prev_file = new_file;
      }
      curr_file->next_file = new_file;

      file_head->root_dir->file_cnt++;

      return curr_file->next_file;
    }
    curr_file = next_file;
  }
  return NULL;
}

//fileNode를 삭제하는 함수
void fileNode_remove(fileNode *file_node) {
  fileNode *next_file = file_node->next_file;
  fileNode *prev_file = file_node->prev_file;

  if(next_file != NULL) {
    next_file->prev_file = file_node->prev_file;
    prev_file->next_file = file_node->next_file;
  } else {
    prev_file->next_file = NULL;
  }

  free(file_node->backup_head);
  file_node->backup_head = NULL;

  free(file_node);
}


/////////////////////트리 노드 생성/////////////////////////////////////

//파일 이름을 제외한 디렉토리 경로만 반환하는 함수
char *GetDirPath(char *path){
	char *dirpath = (char *)malloc(sizeof(char) * PATH_MAX);
	int pathlen = strlen(path);
	int count = pathlen-1;
	   
	for (count; count >= 0; count--) {
	        if (path[count] == '/') {
	            break;
	        }
	}
	
    	if (count >= 0) {
        	strncpy(dirpath, path, count + 1); 
        	dirpath[count + 1] = '\0';
    	} else {
        	fprintf(stderr, "절대경로가 아닙니다\n");
        	exit(1);
    	}
    return dirpath;
}


//시작과 끝 인덱스를 받아 서브스트링 추출
char *substr(char *str, int beg, int end) {
  char *ret = (char*)malloc(sizeof(char) * (end-beg+1));

  for(int i = beg; i < end && *(str+i) != '\0'; i++) {
    ret[i-beg] = str[i];
  }
  ret[end-beg] = '\0';

  return ret;
}

//시간 가져와주는 함수(백업 디렉토리 이름)
char *getDate_pidlog() {
	char *date = (char *)malloc(sizeof(char) * 14);
	time_t timer;
	struct tm *t;

	timer = time(NULL);	
	t = localtime(&timer);

  	sprintf(date, "%d-%02d-%02d %02d:%02d:%02d",t->tm_year+1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
  
  	return date;
}

//시간 가져와주는 함수(백업 디렉토리 이름)
char *getDate() {
	char *date = (char *)malloc(sizeof(char) * 14);
	time_t timer;
	struct tm *t;

	timer = time(NULL);	
	t = localtime(&timer);

  	sprintf(date, "%d%02d%02d%02d%02d%02d",t->tm_year+1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
  	
  
  	return date;
}


//md5 알려주는 함수
int md5(char *target_path, char *hash_result)
{
	FILE *fp;
	unsigned char hash[MD5_DIGEST_LENGTH];
	unsigned char buffer[SHRT_MAX];
	int bytes = 0;
	MD5_CTX md5;

	if ((fp = fopen(target_path, "rb")) == NULL){
		//fprintf(stderr, "ERROR: fopen error for %s\n", target_path);
		return 1;
	}

	MD5_Init(&md5);

	while ((bytes = fread(buffer, 1, SHRT_MAX, fp)) != 0)
		MD5_Update(&md5, buffer, bytes);
	
	MD5_Final(hash, &md5);

	for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
		sprintf(hash_result + (i * 2), "%02x", hash[i]);
	hash_result[HASH_MD5-1] = 0;

	fclose(fp);

	return 0;
}

//두개의 md5를 비교해주는 함수
int cmpMD5(char *path1, char *path2) {
	char *originalMD5 = (char *)malloc(sizeof(char)*HASH_MD5);
	char *backupMD5 = (char *)malloc(sizeof(char)*HASH_MD5);

        md5(path1, originalMD5);
        md5(path2, backupMD5);

	//printf("1.%s\n2.%s\n",originalMD5,backupMD5);
	return strcmp(originalMD5, backupMD5);
}


//맨 뒤에 파일 이름만 반환하는 함수 
char *GetFileName(char file_path[]) {
  	char *file_name;
  
  	while(*file_path) {
    	if(*file_path == '/' && (file_path +1) != NULL) {
    	file_name = file_path+1;
    	}
    	file_path++;
  	}
  	return file_name;
}

//디렉토리 경로와 파일이름을 합쳐 경로 반환하는 함수
char *strTodirPATH(char *dirpath, char *filepath) {
  	char *fullpath = (char *)malloc(sizeof(char) * PATH_MAX);
	strcpy(fullpath, dirpath);
	strcat(fullpath,"/");
	if(filepath != NULL)
		strcat(fullpath,filepath);
	return fullpath;
}

//디렉토리 밑에있는 파일들의 경로 리스트를 주는 함수, path : 디렉토리경로
void searchFileInDir(char *path, char *filelist[], int *filecount){
	DIR *dir;
	struct dirent *dentry;
	struct stat filestat;

	if((dir=opendir(path))==NULL){
		fprintf(stderr, "opendir 실패\n");
		exit(1);
	}
	
	//디렉토리 내부를 검사, .과 ..을 제외한 파일들을 문자열리스트에 담음
	while ((dentry = readdir(dir))!=NULL) {
		char *filepath;
		 filepath = strTodirPATH(path, dentry->d_name);

		
		if (strcmp(dentry->d_name, ".") == 0 || strcmp(dentry->d_name, "..") == 0) continue;

		if((stat(filepath, &filestat))<0){
			fprintf(stderr, "fliestat stat 실패\n");
			continue;
		}

		if(S_ISREG(filestat.st_mode)){
            		filelist[(*filecount)++] = strdup(filepath);
		}
	}
	closedir(dir);
}

//디렉토리 밑에있는 디렉토리들의 경로 리스트를 주는 함수, path : 디렉토리경로
void searchDirInDir(char *path, char *filelist[], int *filecount){
	DIR *dir;
	struct dirent *dentry;
	struct stat filestat;
	if((dir=opendir(path))==NULL){
		return;
	}
	
	//.과 ..을 제외한 디렉토리만 문자열 리스트에 담아줌
	while ((dentry = readdir(dir))!=NULL) {
		char *filepath;
		 filepath = strTodirPATH(path, dentry->d_name);

		if (strcmp(dentry->d_name, ".") == 0 || strcmp(dentry->d_name, "..") == 0) continue;

		if((stat(filepath, &filestat))<0){
			fprintf(stderr, "fliestat stat 실패\n");
			continue;
		}

		if((S_ISDIR(filestat.st_mode))){
            		filelist[(*filecount)++] = strdup(filepath);
		}
	}
	closedir(dir);
}


////////////////유틸함수//////////////////////////

// 데몬프로세스 생성하는 함수
int ssu_daemon_init(char *path) {
    pid_t pid;
    int fd, maxfd;

    // 새로운 프로세스를 생성
    if ((pid = fork()) < 0) {
        fprintf(stderr, "fork error\n");
        exit(0);
    } 
    
    // 부모 프로세스
    else if (pid != 0) {
    	printf("monitoring started (%s) : %d\n", path, pid);
        exit(0); // 부모 프로세스 종료
    }

    pid = getpid();
    setsid(); // 새로운 세션을 생성하고, 프로세스를 세션 리더로 설정
    //터미널 입출력, 정지 시그널 무시
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);

    maxfd = getdtablesize();
    // 모든 파일 디스크립터를 닫음
    for (fd = 0; fd < maxfd; fd++) 
        close(fd);

    umask(0); // 파일 모드 생성 마스크를 초기화
    chdir("/"); // 루트 디렉토리로 이동
    fd = open("/dev/null", O_RDWR); // /dev/null을 읽기/쓰기 모드로 엶
    dup(0); // 표준 입력(0)을 /dev/null로 리다이렉트
    dup(0); // 표준 출력(1)을 /dev/null로 리다이렉트

    return pid;
}


//이미 모니터링 되고있는 하위경로인지 확인하는 함수
int check_already_monitored(char *path/*, char *type*/){
    	char log_line[PATH_MAX * 2];
    	FILE* file = fopen(log_dir, "r");
    	
    	//monitor_list.log를 한 줄씩 읽어 해당 경로가 있는지 확인함
    	while (fgets(log_line, PATH_MAX * 2, file) != NULL) {
		if(strstr(log_line, path)){
			struct stat statbuf;
			char *log_path;
			strtok(log_line, " :");
			log_path = strtok(NULL, ": ");
			log_path[strlen(log_path)-1] = '\0';
				//완전히 동일하면 안됨
				if(!strcmp(log_path, path)){
					return -1;
				}
				else if(stat(log_path, &statbuf)<0){
					continue;
				}
				//디렉토리이면 상위이면 안됨
				else if(S_ISDIR(statbuf.st_mode)){
					return -1;
				}
				continue;
		}
	}
	return 0;
}

//이미 모니터링 되고있는 경로인지 확인하는 함수
int new_check_already_monitored(char *path/*, char *type*/){
    	char log_line[PATH_MAX * 2];
    	FILE* file = fopen(log_dir, "r");
    	
    	//monitor_list.log를 한 줄씩 읽어 해당 경로가 있는지 확인함
    	while (fgets(log_line, PATH_MAX * 2, file) != NULL) {
    	    	int log_line_len = strlen(log_line);
        	if (log_line[log_line_len - 1] == '\n') {
        	    log_line[log_line_len - 1] = '\0';
        	}
        	
        	char *log_path;
		strtok(log_line, " :");
		log_path = strtok(NULL, ": ");
    	    	
    		//같은 경로가 있을경우 -1 리턴
		if(!strcmp(log_path, path)){
			return -1;
		}
	}
	return 0;
}

//-d -r 옵션을 제외한 add 명령어를 처리하는 함수
int addcmd(char *filepath, char *option_t){
	struct stat filestat;
	int monitorlogFD, pidlogFD;
	int pid;
	char log_line[PATH_MAX*2];
	char pid_log[PATH_MAX];
	char pid_dir[PATH_MAX];
	int delaytime = 1;

	//파일 경로인지 확인
	stat(filepath, &filestat);
	if(S_ISDIR(filestat.st_mode)){
		fprintf(stderr, "\"%s\" is directory\n", filepath);
		return 1; 
	}

	//이미 모니터링 되고있는 경로인지 확인
	if(new_check_already_monitored(filepath)<0){
		printf("this path already monitored\n");
		return 0;
	}
	
	
	//t 옵션이 있는지 확인
	if(option_t !=""){
		if(!(delaytime = atoi(option_t))){
      			fprintf(stderr, "ERROR: -t not integer error\n");
                	printf("Usage:");
			addhelp();
      			return -1;
		}
	}
	//데몬 프로세스 생성
	if((pid=ssu_daemon_init(filepath))>0){
		if((monitorlogFD = open(log_dir, O_APPEND|O_RDWR))<0){
			fprintf(stderr, "open fail\n");
			return 0;
		}
		//monitor_list.log에 데몬 프로세스 이름 추가
		sprintf(log_line , "%d : %s\n",pid, filepath);
		if(write(monitorlogFD, log_line, strlen(log_line))<0){
			fprintf(stderr, "write fail\n");
			return 0;
		}

		//pid.log 생성
		sprintf(pid_log, "%s/%d.log",backup_dir, pid);
		if((pidlogFD = open(pid_log, O_CREAT|O_RDWR, 0666))<0){
			fprintf(stderr, "open fail\n");
			return 0;
		}
		
		//pid 이름의 디렉토리를 생성하고 백업 파일 생성
		char backup_file_path[PATH_MAX*2];
		char cpystr[PATH_MAX];
		char *backup_file_name;
		int fd1, fd2;
		char backup_dir_path[PATH_MAX];
		struct stat statbuf1, statbuf2;
		int len;
		char buf[BUF_MAX];
		
		//pid 이름의 디렉토리 생성
		sprintf(backup_dir_path, "%s/%d", backup_dir, pid);
		if(mkdir(backup_dir_path, 0777)<0){
			fprintf(stderr, "mkdir fail\n");
			return 0;
		}
	
		//백업파일 생성
		strcpy(cpystr, filepath);
		backup_file_name = GetFileName(cpystr);
		sprintf(backup_file_path, "%s/%s_%s",backup_dir_path, backup_file_name, getDate());
		if((fd1 = open(filepath, O_RDONLY)) <0 ) {
      			fprintf(stderr, "ERROR: open error\n");
      			return -1;
    		}
    		if((fd2 = open(backup_file_path, O_WRONLY|O_CREAT, 0777)) <0) {
      			fprintf(stderr, "ERROR: open error\n");
      			return -1;
    		}
    		while(len = read(fd1, buf, BUF_MAX)) {
      			write(fd2, buf, len);
    		}
    		if(stat(backup_file_path, &statbuf1)<0){
      			fprintf(stderr, "stat fail\n");
      			return -1;
    		}
    		
    		if(stat(filepath, &statbuf2)<0){
      			fprintf(stderr, "stat2 fail\n");
      			return -1;
    		}
    			
    		//mtime과 atime을 원본과 동일하게 설정
    	    	struct utimbuf utbuf;
    		utbuf.actime = statbuf2.st_atime;
    		utbuf.modtime = statbuf2.st_mtime;

    		if (utime(backup_file_path, &utbuf) == -1) {
    			fprintf(stderr, "utime fail\n");
    			close(fd1);
        		close(fd2);
        		return -1;
    		}


    		//pid.로그 작성
    		sprintf(buf, "[%s][create][%s]\n", getDate_pidlog(), filepath);
    		write(pidlogFD, buf, strlen(buf));
		
		

		while(1){
			if(stat(filepath, &statbuf2)<0){
				//삭제되었을 경우 로그 작성
    				sprintf(buf, "[%s][remove][%s]\n", getDate_pidlog(), filepath);
    				write(pidlogFD, buf, strlen(buf));
    				break;
    			}
    			
    			//해당 파일의 크기, mtime, ctime, md5 비교해서 파일이 modified 되었는지 확인
    			if(statbuf1.st_size != statbuf2.st_size && statbuf1.st_mtime != statbuf2.st_mtime && statbuf1.st_ctime < statbuf2.st_ctime &&  cmpMD5(filepath, backup_file_path)){
				//새로운 백업파일 생성
				sprintf(backup_file_path, "%s/%s_%s",backup_dir_path, backup_file_name, getDate());
				if((fd1 = open(filepath, O_RDONLY)) <0 ) {
      					fprintf(stderr, "ERROR: open error\n");
      					return -1;
    				}
    				if((fd2 = open(backup_file_path, O_WRONLY|O_CREAT, 0777)) <0) {
      					fprintf(stderr, "ERROR: open error\n");
      					return -1;
    				}
    				while(len = read(fd1, buf, BUF_MAX)) {
      					write(fd2, buf, len);
    				}
    				
    				//mtime과 atime을 원본과 동일하게 설정
    	    			struct utimbuf utbuf;
    				utbuf.actime = statbuf2.st_atime;
    				utbuf.modtime = statbuf2.st_mtime;

    				if (utime(backup_file_path, &utbuf) == -1) {
    					fprintf(stderr, "utime fail\n");
    					close(fd1);
        				close(fd2);
        				return -1;
    				}
    				
    				//pid 로그 작성
    				sprintf(buf, "[%s][modify][%s]\n", getDate_pidlog(), filepath);
    				write(pidlogFD, buf, strlen(buf));
			
				stat(filepath, &statbuf1);
			}	
			//delaytime마다 반복
			sleep(delaytime);
		}
	}
	else{
		return 0;
	}
}

//경로를 받아 상위의 디렉토리들을 재귀적으로 생성하는 함수
int mkdirRecursive(char *path) {
    char *path_copy = strdup(path);
    char *token = strtok(path_copy, "/");
    char current_path[PATH_MAX] = "";

    if (!token) {
        free(path_copy);
        return -1;
    }
    
    if (path[0] == '/') {
        // 절대 경로인 경우 맨 처음에 '/'를 추가
        strcat(current_path, "/");
    }

    while (token != NULL) {
        strcat(current_path, token); 
        strcat(current_path, "/"); 
	    
	// 디렉토리를 재귀적으로 생성
        if(mkdir(current_path, 0755)==0){
    		if(chmod(current_path, 0777)<0){
    			fprintf(stderr,"chmod fail\n");
    		}     
        }    
        token = strtok(NULL, "/");
    }

    free(path_copy);
    return 0;
}

//특정 문자열을 제외한 나머지 문자열만 리턴하는 함수
char *GetRelativePath(char *fullPath, char *basePath) {
    size_t baseLen = strlen(basePath);
    
    char *adjustedBasePath = (char *)malloc(baseLen + 2);

    strcpy(adjustedBasePath, basePath);
    if (adjustedBasePath[baseLen - 1] != '/') {
        adjustedBasePath[baseLen] = '/';
        adjustedBasePath[baseLen + 1] = '\0';
        baseLen++;
    }

    if (strncmp(fullPath, adjustedBasePath, baseLen) == 0) {
        char *relativePath = strdup(fullPath + baseLen);
        free(adjustedBasePath);
        return relativePath;
    } else {
        fprintf(stderr, "substr err\n");
        free(adjustedBasePath);
        exit(1);
    }
}

//-d, -r 옵션을 포함한 add 명령어를 처리하는 함수
int addcmd_dr(char *dirpath, char *option_dr, char *option_t){
	struct stat filestat;
	int monitorlogFD, pidlogFD;
	int pid;
	char log_line[PATH_MAX*2];
	char pid_log[PATH_MAX];
	char pid_dir[PATH_MAX];
	int delaytime = 1;

	filenode_dr *dr_head;
	int dr_node_cnt = 0;
	
	//디렉토리 경로인지 확인
	stat(dirpath, &filestat);
	if(S_ISREG(filestat.st_mode)){
		fprintf(stderr, "\"%s\" is file\n", dirpath);
		return 1; 
	}

	//이미 모니터링 되고있는 경로인지 확인
	if(new_check_already_monitored(dirpath)<0){
		printf("this path already monitored\n");
		return 0;
	}
	
	//t 옵션이 있는지 확인
	if(option_t !=""){
		if(!(delaytime = atoi(option_t))){
      			fprintf(stderr, "Error: -t not integer error\n");
                	printf("Usage:");
			addhelp();
      			return -1;
		}
	}

	//데몬 프로세스 생성
	if((pid=ssu_daemon_init(dirpath))>0){
		if((monitorlogFD = open(log_dir, O_APPEND|O_RDWR))<0){
			fprintf(stderr, "open fail\n");
			return 0;
		}
		//monitor_list.log에 데몬 프로세스 이름 추가
		sprintf(log_line , "%d : %s\n",pid, dirpath);
		if(write(monitorlogFD, log_line, strlen(log_line))<0){
			fprintf(stderr, "write fail\n");
			return 0;
		}

		//pid.log 생성
		sprintf(pid_log, "%s/%d.log",backup_dir, pid);
		if((pidlogFD = open(pid_log, O_CREAT|O_RDWR, 0666))<0){
			fprintf(stderr, "open fail\n");
			return 0;
		}
		
		//pid 이름의 디렉토리를 생성하고 백업 파일 생성
		char backup_file_path[PATH_MAX*2];
		char cpystr[PATH_MAX];
		char *backup_file_name;
		int fd1, fd2;
		char backup_dir_path[PATH_MAX];
		struct stat statbuf1, statbuf2;
		int len;
		char buf[BUF_MAX*2];
		
		//pid 이름의 디렉토리 생성
		sprintf(backup_dir_path, "%s/%d", backup_dir, pid);
		if(mkdir(backup_dir_path, 0777)<0){
			fprintf(stderr, "mkdir fail\n");
			return 0;
		}

		int testnum = 0;
		
		//옵션 d일 경우
		if(!strcmp(option_dr, "d")){
			while(1){
			    	char *newfilelist[PATH_MAX];
			    	int newfilecnt = 0;
			    	int newcheckarr[BUF_MAX] = {};
			    
			    	//현재 모니터링 되는 파일들의 링크드리스트 생성
			    	filenode_dr *current = dr_head;
			    	filenode_dr* prev = NULL;
			    	
			    	//원본 디렉토리 내부의 파일들 가져옴
			    	searchFileInDir(dirpath, newfilelist, &newfilecnt);
				
				//링크드리스트 순회
			    	while(current !=NULL){
			    		int deletecheck = 0;
			    		for(int i=0; i<newfilecnt; i++){
			    			//원본 디렉토리의 파일과 같은 이름의 링크드리스트 파일이 있으면 파일이 수정되었는지 확인	
			    			if(!strcmp(newfilelist[i], current->filepath)){
			    				//printf("modify: %s\n", newfilelist[i]);
			    				deletecheck = 1;
			    				
			    				newcheckarr[i] = 1;
			    				char filepath[PATH_MAX];
			    				
							strcpy(backup_file_path ,current->backupfilepath);
							strcpy(filepath, current->filepath);
						
	    						if(stat(backup_file_path, &statbuf1)<0){
	      							fprintf(stderr, "stat1 fail: %s\n", backup_file_path);
	      							return -1;
	    						}
	    					    												
	    						if(stat(filepath, &statbuf2)<0){
	      							fprintf(stderr, "stat2 fail\n");
	      							return -1;
	    						}
	    			
    							//해당 파일의 크기, mtime, ctime, md5 비교해서 파일이 modifiy 되었는지 확인
	    						if(statbuf1.st_size != statbuf2.st_size && statbuf1.st_mtime != statbuf2.st_mtime && statbuf1.st_ctime < statbuf2.st_ctime &&  cmpMD5(filepath, backup_file_path)){
							
								strcpy(cpystr, filepath);
								backup_file_name = GetFileName(cpystr);
								
								
								//새로운 백업파일 생성 
								sprintf(backup_file_path, "%s/%s_%s",backup_dir_path, backup_file_name, getDate());
								if((fd1 = open(filepath, O_RDONLY)) <0 ) {
	      								fprintf(stderr, "ERROR: open error\n");
	      								return -1;
	    							}
	    							if((fd2 = open(backup_file_path, O_WRONLY|O_CREAT, 0777)) <0) {
	      								fprintf(stderr, "ERROR: open error\n");
	      								return -1;
	    							}
	    							while(len = read(fd1, buf, BUF_MAX)) {
	      								write(fd2, buf, len);
	    							}
	    							
    								//mtime과 atime을 원본과 동일하게 설정
    	    							struct utimbuf utbuf;
    								utbuf.actime = statbuf2.st_atime;
    								utbuf.modtime = statbuf2.st_mtime;

    								if (utime(backup_file_path, &utbuf) == -1) {
    									fprintf(stderr, "utime fail\n");
    									close(fd1);
        								close(fd2);
        								return -1;
    								}
	    				
	    							//pid 로그 작성
	    							sprintf(buf, "[%s][modify][%s]\n", getDate_pidlog(), filepath);
	    							write(pidlogFD, buf, strlen(buf));
				
								stat(filepath, &statbuf1);
								
								//링크드리스트의 백업 파일 최신화
								strcpy(current->backupfilepath, backup_file_path);
								
							}
					
							
			    			}	
			    			    			
			    		}
			    		
			    		//링크드리스트의 노드에 해당하는 원본 파일이 없을 경우 delete 된 것으로 판단
			    		if(!deletecheck){
			    			//pid로그 작성
			    			sprintf(buf, "[%s][remove][%s]\n", getDate_pidlog(), current->filepath);
	    					write(pidlogFD, buf, strlen(buf));
	    					
			    			//링크드리스트에서 삭제
		        			if (prev == NULL) {
		            				dr_head = current->next;
		        			} 
		        			else {
		            				prev->next = current->next;
		        			}

		        			filenode_dr* temp = current;
		       				current = current->next;
		       				
	    					free(temp);
			    		}
			    		else{
		        			prev = current;
			    			current = current -> next;
			    		}
			    		
			    	}
			    	
			    	//받아온 원본 파일들 중 링크드리스트의 경로에 없는 것은 create 된 것으로 판단
			    	for(int i=0; i<newfilecnt; i++){
			    		if(!newcheckarr[i]){
			    			char *filepath = newfilelist[i];
			    			char cpystr[PATH_MAX];
			    			
				
						//백업파일 생성
						strcpy(cpystr, filepath);
						backup_file_name = GetFileName(cpystr);
						sprintf(backup_file_path, "%s/%s_%s",backup_dir_path, backup_file_name, getDate());
						if((fd1 = open(filepath, O_RDONLY)) <0 ) {
	      						fprintf(stderr, "ERROR: open error\n");
	      						return -1;
	    					}
		    					if((fd2 = open(backup_file_path, O_WRONLY|O_CREAT, 0777)) <0) {
		      						fprintf(stderr, "ERROR: open error\n");
	      						return -1;
	    					}
	    					while(len = read(fd1, buf, BUF_MAX)) {
	      						write(fd2, buf, len);
	    					}
	    					if(stat(backup_file_path, &statbuf1)<0){
	      						fprintf(stderr, "stat fail\n");
	      						return -1;
	    					}
	    					
	    					if(stat(filepath, &statbuf2)<0){
	      						fprintf(stderr, "stat2 fail\n");
	      						return -1;
	    					}
	    					
    						//mtime과 atime을 원본과 동일하게 설정
    	    					struct utimbuf utbuf;
    						utbuf.actime = statbuf2.st_atime;
    						utbuf.modtime = statbuf2.st_mtime;

    						if (utime(backup_file_path, &utbuf) == -1) {
    							fprintf(stderr, "utime fail\n");
    							close(fd1);
        						close(fd2);
        						return -1;
    						}
	    					
	    					//pid.로그 작성
	    					sprintf(buf, "[%s][create][%s]\n", getDate_pidlog(), filepath);
	    					write(pidlogFD, buf, strlen(buf));
	    					
			    			//링크드리스트에 추가
			    			filenode_dr *newnode = (filenode_dr *)malloc(sizeof(filenode_dr));
			    			strcpy(newnode->filepath, filepath);
			    			strcpy(newnode->backupfilepath, backup_file_path);
			    			newnode->next = dr_head;
			    			dr_head = newnode;	
			    		}
			    	}
				//delaytime마다 반복
				sleep(delaytime);
			 }   
		} 
		//옵션 r일 경우
		else{
			while(1){
			    	char *newfilelist[PATH_MAX];
			    	int newfilecnt = 0;
			    	int newcheckarr[BUF_MAX] = {};
			    
			    	//원본 디렉토리 내부의 파일들을 재귀적으로 가져옴
				getAllFilePath(dirpath, newfilelist, &newfilecnt);
			    	filenode_dr *current = dr_head;
			    	filenode_dr* prev = NULL;

			    	//모니터링 파일 링크드리스트를 순회함
			    	while(current !=NULL){
			    		int deletecheck = 0;
			    		for(int i=0; i<newfilecnt; i++){
			    			//원본 디렉토리의 파일과 같은 이름의 링크드리스트 파일이 있으면 파일이 수정되었는지 확인	
			    			if(!strcmp(newfilelist[i], current->filepath)){

			    				deletecheck = 1;
			
			    				newcheckarr[i] = 1;
			    				
			    				
			    				char filepath[PATH_MAX];
			    				
							strcpy(backup_file_path ,current->backupfilepath);
							strcpy(filepath, current->filepath);
					
						
	    						if(stat(backup_file_path, &statbuf1)<0){
	      							fprintf(stderr, "stat1 fail: %s\n", backup_file_path);
	      							return -1;
	    						}
	    					
	    					    												
	    						if(stat(filepath, &statbuf2)<0){
	      							fprintf(stderr, "stat2 fail\n");
	      							return -1;
	    						}
	    			
    							//해당 파일의 크기, mtime, ctime, md5 비교해서 파일이 modifiy 되었는지 확인
	    						if(statbuf1.st_size != statbuf2.st_size && statbuf1.st_mtime != statbuf2.st_mtime && statbuf1.st_ctime < statbuf2.st_ctime &&  cmpMD5(filepath, backup_file_path)){
								char cpystr2[PATH_MAX+1];
								strcpy(cpystr, filepath);
								backup_file_name = GetFileName(cpystr);
								
								char new_backup_dir_path[PATH_MAX+1]; 
								//backup_dir_path에 현재 경로에서 파일 이름빼고 dirpath 앞에꺼 떼고 붙이기
								strcpy(cpystr2, filepath);
								char *only_dir = GetDirPath(cpystr2);
								char *token = GetRelativePath(only_dir, dirpath);
						
								if(!strcmp(token, "")){
									sprintf(new_backup_dir_path, "%s%s/",backup_dir_path , token);
								}
								else{
									sprintf(new_backup_dir_path, "%s/%s",backup_dir_path , token);
								}
								
								//백업파일 생성
								sprintf(backup_file_path, "%s/%s_%s",new_backup_dir_path, backup_file_name, getDate());
								
								if((fd1 = open(filepath, O_RDONLY)) <0 ) {
	      								fprintf(stderr, "ERROR: open error\n");
	      								return -1;
	    							}
	    							if((fd2 = open(backup_file_path, O_WRONLY|O_CREAT, 0777)) <0) {
	      								fprintf(stderr, "ERROR: open error\n");
	      								return -1;
	    							}
	    							while(len = read(fd1, buf, BUF_MAX)) {
	      								write(fd2, buf, len);
	    							}
	    							
    								//mtime과 atime을 원본과 동일하게 설정
    	    							struct utimbuf utbuf;
    								utbuf.actime = statbuf2.st_atime;
    								utbuf.modtime = statbuf2.st_mtime;

    								if (utime(backup_file_path, &utbuf) == -1) {
    									fprintf(stderr, "utime fail\n");
    									close(fd1);
        								close(fd2);
        								return -1;
    								}
	    				
	    							//pid 로그 작성
	    							sprintf(buf, "[%s][modify][%s]\n", getDate_pidlog(), filepath);
	    							write(pidlogFD, buf, strlen(buf));
				
								stat(filepath, &statbuf1);
								
								strcpy(current->backupfilepath, backup_file_path);
								
							}
					
							
			    			}	
			    			    			
			    		}		    	
			    		//링크드리스트의 노드에 해당하는 원본 파일이 없을 경우 delete 된 것으로 판단
			    		if(!deletecheck){
			    			//pid 로그 작성
			    			sprintf(buf, "[%s][remove][%s]\n", getDate_pidlog(), current->filepath);
	    					write(pidlogFD, buf, strlen(buf));
	    					
			    			//링크드리스트에서 삭제
		        			if (prev == NULL) {
		            				dr_head = current->next;
		        			} 
		        			else {
		            				prev->next = current->next;
		        			}

		        			filenode_dr* temp = current;
		       				current = current->next;
		       				
	    					free(temp);
			    		}
			    		else{
		        			prev = current;
			    			current = current -> next;
			    		}
			    		
			    	}
			    	//받아온 원본 파일들 중 링크드리스트의 경로에 없는 것은 create 된 것으로 판단
			    	for(int i=0; i<newfilecnt; i++){
			    		if(!newcheckarr[i]){
			    			char *filepath = newfilelist[i];
						char cpystr[PATH_MAX], cpystr2[PATH_MAX], cpystr3[PATH_MAX];
						char new_backup_dir_path[PATH_MAX+1]; 
						//backup_dir_path에 현재 경로에서 파일 이름빼고 dirpath 앞에꺼 떼고 붙이기
						strcpy(cpystr2, filepath);
						char *only_dir = GetDirPath(cpystr2);
	
						char *token = GetRelativePath(only_dir, dirpath);
						
						if(!strcmp(token, "")){
							sprintf(new_backup_dir_path, "%s%s/",backup_dir_path , token);
						}
						else{
							sprintf(new_backup_dir_path, "%s/%s",backup_dir_path , token);
						}
						
						//재귀적으로 디렉토리 생성
						mkdirRecursive(new_backup_dir_path);
						
						//백업파일 생성
						strcpy(cpystr, filepath);
						backup_file_name = GetFileName(cpystr);
						sprintf(backup_file_path, "%s%s_%s",new_backup_dir_path, backup_file_name, getDate());
						
						if((fd1 = open(filepath, O_RDONLY)) <0 ) {
	      						fprintf(stderr, "ERROR: open error\n");
	      						return -1;
	    					}
		    				if((fd2 = open(backup_file_path, O_WRONLY|O_CREAT, 0777)) <0) {
		      						fprintf(stderr, "ERROR: open error\n");
	      						return -1;
	    					}
	    					while(len = read(fd1, buf, BUF_MAX)) {
	      						write(fd2, buf, len);
	    					}
	    					if(stat(backup_file_path, &statbuf1)<0){
	      						fprintf(stderr, "stat fail\n");
	      						return -1;
	    					}
	    					
	    					if(stat(filepath, &statbuf2)<0){
	      						fprintf(stderr, "stat2 fail\n");
	      						return -1;
	    					}
	    					
    						//mtime과 atime을 원본과 동일하게 설정
    	    					struct utimbuf utbuf;
    						utbuf.actime = statbuf2.st_atime;
    						utbuf.modtime = statbuf2.st_mtime;

    						if (utime(backup_file_path, &utbuf) == -1) {
    							fprintf(stderr, "utime fail\n");
    							close(fd1);
        						close(fd2);
        						return -1;
    						}
	    					
	    					
	    					//pid.로그 작성
	    					sprintf(buf, "[%s][create][%s]\n", getDate_pidlog(), filepath);
	    					write(pidlogFD, buf, strlen(buf));
	    					
			    			//링크드리스트 생성
			    			filenode_dr *newnode = (filenode_dr *)malloc(sizeof(filenode_dr));
			    			strcpy(newnode->filepath, filepath);
			    			strcpy(newnode->backupfilepath, backup_file_path);
			    			newnode->next = dr_head;
			    			dr_head = newnode;	
			    			
			    		}
			    	}
				//delaytime마다 반복
				sleep(delaytime);
			 }  
		
		}
			
	}
	else{
		return 0;
	}

}

//디렉토리 경로를 받아 재귀적으로 내부의 디렉토리가 비어있는지 모두 확인하고 비어있으면 삭제하는 함수
void deleteEmptyDirectories(char *path) {
    	char *filelist[PATH_MAX]; 
    	int filecount = 0;
    	int count = 0;
    	struct dirent *dentry;
	DIR *dir;

	//디렉토리들을 재귀적으로 탐색
    	searchDirInDir(path, filelist, &filecount);
    	for (int i = 0; i < filecount; i++) {
    	    	deleteEmptyDirectories(filelist[i]);
    	}
    	if((dir = opendir(path))==NULL){
		return;
	}
    	while((dentry = readdir(dir)) != NULL){
    		if(++count>2){
    			break;
    		}
    	}
    	//.과 ..외에 아무것도 없으면 디렉토리 삭제
    	closedir(dir);
    	if(count<=2){
    		remove(path);
    	}
}


//옵션 r로 remove명령어를 실행했을 때 반복적으로 파일을 remove하는 함수
void recursiveRemove(char *dirpath){
	char *filelist[PATH_MAX];
	char *dirlist[PATH_MAX];
	int filecount = 0;
	int dircount = 0;
	
	//파일은 삭제
	searchFileInDir(dirpath, filelist, &filecount);
	for(int i=0;i<filecount;i++){
		remove(filelist[i]);
	}
	
    	//디렉토리는 링크드리스트에 붙임
    	searchDirInDir(dirpath, dirlist, &dircount);
    	for(int i=0; i<dircount;i++){
    	    	char newbackuppath[PATH_MAX];
    		bfslinkedlist *newbackupnode = malloc(sizeof(bfslinkedlist));
    		strcpy(newbackupnode -> dirpath, dirlist[i]);
		
		bfslinkedlist *temp = bfshead;
        	while (temp->next != NULL) {
            		temp = temp->next;
       		}
        	temp->next = newbackupnode;
    	}
}

//pid 디렉토리를 제거하는 함수
void RemoveR(char *path){	

	bfslinkedlist *newbackupnode = malloc(sizeof(bfslinkedlist));
	strcpy(newbackupnode -> dirpath, path);
	newbackupnode -> next = bfshead;
	bfshead = newbackupnode;

	//링크드리스트 끝까지 갈 때까지 반복
   	 while (bfshead != NULL) {
        	bfslinkedlist *current = bfshead;
        	recursiveRemove(current->dirpath);

        	bfshead = bfshead->next;
        	free(current);
    	}	
    	//빈 디렉토리 삭제
    	deleteEmptyDirectories(path);

}




//remove 명령어를 처리하는 함수
int removecmd(char *pid){
    	char log_line[PATH_MAX * 2];
    	char pid_dir[PATH_MAX];
    	char pid_log[PATH_MAX+10];
    	int existchk = 0;
    	
    	char tmp_log_dir[PATH_MAX];
    	sprintf(tmp_log_dir, "%s/tmp.log", backup_dir);
    	
    	
    	FILE* file = fopen(log_dir, "r");
   	int tmp_file_FD = open(tmp_log_dir, O_CREAT|O_WRONLY|O_TRUNC, 0777);
   	char pid_filepath[PATH_MAX];
    	
    	// pid가 포함된 줄을 찾아서 제외하고 나머지 줄을 임시 파일에 복사
    	while (fgets(log_line, PATH_MAX * 2, file) != NULL) {
    	    	char cp_log_line[PATH_MAX * 2];
    	    	strcpy(cp_log_line, log_line);
    		char *linepid = strtok(log_line, " ");
    		
        	if (!strcmp(log_line, pid)) {
        		//pid가 포함된 경로 추출
        		linepid = strtok(NULL, ": ");
        		int linelen = strlen(linepid);
        		linepid[linelen-1] ='\0';
        		strcpy(pid_filepath,linepid);
        	        existchk = 1;
        	} 
        	else {
            		write(tmp_file_FD, cp_log_line, strlen(cp_log_line));
        	}
    	}
    	
    	fclose(file);
    	close(tmp_file_FD);
    	
    	// 기존 로그 파일을 삭제하고 임시 파일을 로그 파일로 이름을 변경하여 원래의 로그 파일을 대체
    	remove(log_dir);
	rename(tmp_log_dir, log_dir);

    	// PID가 존재하지 않는 경우 오류를 출력하고 -1을 반환
    	if (!existchk) {
        	fprintf(stderr, "ERROR: This pid does not exist in \"monitor_list.log\"\n");
        	return -1;
    	}
    	
    	//데몬 프로세스 종료
    	if(kill(atoi(pid), SIGKILL)<0){
    		//fprintf(stderr, "kill fail\n");
    	}
	//pid 디렉토리 제거
	sprintf(pid_dir, "%s/%s",backup_dir, pid);
	RemoveR(pid_dir);
	//pid.log 제거
	sprintf(pid_log, "%s.log", pid_dir);
	remove(pid_log);
	
	printf("monitoring ended (%s): %s\n", pid_filepath, pid);
	
    	return 0;
    	
}




void print_depth(int depth, int is_last_bit) {
  for(int i = 1; i <= depth; i++) {
    if(i == depth) {
      if((1 << depth) & is_last_bit) {
          printf("┗ ");
      } else {
          printf("┣ ");
      }
      break;
    }
    if((1 << i) & is_last_bit) {
      printf("  ");
    } else {
      printf("┃ ");
    }
  }
}

//링크드리스트에 대한 정보를 시각적으로 보여주는 함수
void print_list(dirNode* dirList, int depth, int last_bit) {
  dirNode* curr_dir = dirList->subdir_head->next_dir;
  fileNode* curr_file = dirList->file_head->next_file;
  int startcnt = 0;

  while(curr_dir != NULL && curr_file != NULL) {
    if(strcmp(curr_dir->dir_name, curr_file->file_name) < 0) {
      print_depth(depth, last_bit);
      printf("%s\n", curr_dir->dir_name);
      print_list(curr_dir, depth+1, last_bit);
      curr_dir = curr_dir->next_dir;
    } else {
      print_depth(depth, last_bit);
      printf("%s\n", curr_file->file_name);
      
      backupNode* curr_backup = curr_file->backup_head->next_backup;
      while(curr_backup != NULL) {
        print_depth(depth+1, last_bit);
        printf("%s\n", curr_backup->backup_time);
        curr_backup = curr_backup->next_backup;
      }
      curr_file = curr_file->next_file;
    }
  }
  
  while(curr_dir != NULL) {
    last_bit |= (curr_dir->next_dir == NULL)?(1 << depth):0;
    
    print_depth(depth, last_bit);
    if(!startcnt){
    	startcnt = 1;
    	if(strcmp(curr_dir->dir_name, ""))
    		printf("%s\n", curr_dir->dir_name);
    }
    else{
    	printf("%s\n", curr_dir->dir_name);
    }
    print_list(curr_dir, depth+1, last_bit);
    curr_dir = curr_dir->next_dir;
  }
  
  while(curr_file != NULL) {
    last_bit |= (curr_file->next_file == NULL)?(1 << depth):0;

    print_depth(depth, last_bit);
    printf("%s\n", curr_file->file_name);
    
    backupNode* curr_backup = curr_file->backup_head->next_backup;
    while(curr_backup != NULL) {
      print_depth(depth+1, (
        last_bit | ((curr_backup->next_backup == NULL)?(1 << depth+1):0)
        ));
      printf("%s\n", curr_backup->backup_time);
      curr_backup = curr_backup->next_backup;
    }
    curr_file = curr_file->next_file;
  }
}




// 파일 경로를 재귀적으로 가져오는 함수
void recursiveGetFile(char *dirpath, char *pathlist[], int *pathcnt, int *dircnt){
	char *filelist[PATH_MAX];
	char *dirlist[PATH_MAX];
	int filecount = 0;
	int dircount = 0;
	
	//파일은 리스트에 넣음
	searchFileInDir(dirpath, filelist, &filecount);
	for(int i=0;i<filecount;i++){
		pathlist[(*pathcnt)++] = filelist[i];
	}
    	//디렉토리는 링크드리스트에 붙임
    	searchDirInDir(dirpath, dirlist, &dircount);
    	
    	for(int i=0; i<dircount;i++){
    		//printf("%d, dirlist[i]:%s\n", dircount,dirlist[i]);
    		
    		bfslinkedlist *newbackupnode = malloc(sizeof(bfslinkedlist));
    		strcpy(newbackupnode -> dirpath, dirlist[i]);
		bfslinkedlist *temp = bfshead;
        	while (temp->next != NULL) {
            		temp = temp->next;
       		}
        	temp->next = newbackupnode;
        	(*dircnt)++;
    	}
    	
}

//입력 디렉토리 내의 모든 파일경로를 가져오는 함수
void getAllFilePath(char *path, char *pathlist[], int *pathcnt){	
	bfshead = NULL;
	
	int dircnt=0;
	bfslinkedlist *newbackupnode = malloc(sizeof(bfslinkedlist));
	strcpy(newbackupnode -> dirpath, path);
	newbackupnode -> next = bfshead;
	bfshead = newbackupnode;
	dircnt++;
	
	//링크드리스트 끝까지 갈 때까지 반복
   	 while (bfshead != NULL ) {
        	bfslinkedlist *current = bfshead;
        	recursiveGetFile(current->dirpath, pathlist, pathcnt, &dircnt);
        	bfshead = bfshead->next;
        	dircnt--;
    	        if(dircnt == 0){
    	        	break;
    	        }
        	free(current);
    	}
}



//받은 경로로 트리 링크드리스트를 만드는 함수
int backup_list_insert(dirNode* dirList, char* backup_time, char* path, char* backup_path) {
  char* ptr;
  //디렉토리 경로에 슬래시가 있는 경우
  if(ptr = strchr(path, '/')) {
    //디렉토리 이름 추출
    char* dir_name = substr(path, 0, strlen(path) - strlen(ptr));
    // 현재 디렉토리에 새 디렉토리 노드를 삽입하고 재귀적으로 호출
    dirNode* curr_dir = dirNode_insert(dirList->subdir_head, dir_name, dirList->dir_path);
    backup_list_insert(curr_dir, backup_time, ptr+1, backup_path);
    curr_dir->backup_cnt++;

  } 
  else {
    char* file_name = path;
	//파일인경우
        fileNode* curr_file = fileNode_insert(dirList->file_head, file_name, dirList->dir_path);
  }
  return 0;
}

//로그파일을 한 줄씩 읽어서 정보를 만들어 backup_list_insert 함수에 넣어주는 함수
void makecommitlistinfo(char *log_line, char *origin_dir){
    	char first[PATH_MAX];
    	char second[PATH_MAX];
    	char third[PATH_MAX];
    	char third_except_dir[PATH_MAX];
	char final_path[PATH_MAX*3+2];

    	// "-"를 기준으로 파싱
    	char *token = strtok(log_line, "[");
    	if (token != NULL) {
    	    strcpy(first, token);
    	}

    	// 마지막 부분을 파싱
    	token = strtok(NULL, "[");
    	if (token != NULL) {
        	strcpy(second, token);
    	}
    
    	token = strtok(NULL, "[");
    	if (token != NULL) {
    	    strcpy(third, token);
    	}

        int rm = strlen(third);
        third[rm-2] = '/';
        third[rm-1] = '\0';
        
    	size_t origin_dir_len = strlen(origin_dir);

    	// third 문자열에서 origin_dir 부분을 제외한 부분을 찾음
    	if (strncmp(third, origin_dir, origin_dir_len) == 0) {
        	strcpy(third_except_dir, third + origin_dir_len + 1);
    	} 
    	else {
        	strcpy(third_except_dir, third);
	}
        
        sprintf(final_path, "/%s[%s[%s", third_except_dir, second, first);
        //printf("final: %s", final_path);
        // 연결 리스트에 정보 추가
	backup_list_insert(backup_dir_list, NULL, final_path, NULL);

}

//list 명령어를 출력하는 함수
int listcmd(char *pid){
	char buf[BUF_MAX];
	int len;
	int fd;
	//pid가 비어있으면 monitor_list.log 그대로 출력
	if(pid==NULL){
	    	struct stat statbuf;
    		if(stat(log_dir, &statbuf)<0){
    			fprintf(stderr,"list stat fail\n");
    			return -1;
    		}
    		if(statbuf.st_size==0){
    			fprintf(stderr,"no monitoring file\n");
    			return -1;
    		}
	
		if((fd = open(log_dir, O_RDONLY))<0){
			fprintf(stderr, "open fail\n");
			return -1;
		}
		
        	while ((len = read(fd, buf, BUF_MAX)) > 0) {
            		if (write(STDOUT_FILENO, buf, len) != len) {
                		close(fd);
                		return -1;
            		}	
        	}
        	if (len< 0) {
			fprintf(stderr, "read err\n");
            		close(fd);
            		return -1;
        	}
        	close(fd);
	}
	//pid를 입력받은 경우
	else{
		char log_line[PATH_MAX *2];
    		struct stat statbuf;
    		char tmp_log_dir[PATH_MAX];
    		
    		//pid 로그 찾기
    		sprintf(tmp_log_dir, "%s/%s.log", backup_dir, pid);
    		
    		
    		//해당 경로가 존재하는지 확인
    		if(stat(tmp_log_dir, &statbuf)<0){
    			fprintf(stderr,"wrong pid name\n");
    			return -1;
    		}
    	
		//원본 dir 경로 찾기
		char origin_path_line[PATH_MAX*2];
        	FILE* file2;
    		if((file2 = fopen(log_dir, "r"))<0){
    			fprintf(stderr,"stat fail\n");
    			return -1;
    		}
    		
    		//로그를 한줄씩 읽어 해당 pid의 경로 찾기
    		char *origin_dir;
    		while (fgets(origin_path_line, PATH_MAX * 2, file2) != NULL) {
    			char *cur_pid = strtok(origin_path_line, ": ");
    			char *cur_path = strtok(NULL, ": ");
    			
    			if(!strcmp(cur_pid, pid)){
    				origin_dir = cur_path;
    				int origin_len = strlen(origin_dir);
    				origin_dir[origin_len-1] = '\0';
    				//printf("%s\n", origin_dir);
    				break;
    			}
    		}
		
    		
    		FILE* file;
    		if((file = fopen(tmp_log_dir, "r"))<0){
    			fprintf(stderr,"stat fail\n");
    			return -1;
    		}

    		backup_dir_list = (dirNode*)malloc(sizeof(dirNode)); 
  		dirNode_init(backup_dir_list);    	
    	
    		//경로를 makecommitlistinfo함수에 넣어줌
    		while (fgets(log_line, PATH_MAX * 2, file) != NULL) {
    			makecommitlistinfo(log_line, origin_dir);
    		}
    		

		//트리구조로 링크드리스트 만든 뒤 출력
   		print_list(backup_dir_list, 0, 0);
   		

	
	}
}

//help 명령어를 수행하는 함수
int helpcmd(char *path){
	if(path==0){
		help();
	}
	else{
		if(!strcmp(path, "add")){
			printf("Usage:");
			addhelp();
		}
		else if(!strcmp(path, "remove")){
			printf("Usage:");
			removehelp();
		}
		else if(!strcmp(path, "list")){
			printf("Usage: ");
			listhelp();
		}
		else if(!strcmp(path, "help")){
			printf("Usage:");
			helphelp();
		}
		else if(!strcmp(path, "exit" )){
			printf("Usage:");
			exithelp();
		}
		else{
			fprintf(stderr, "ERROR : wrong command\n");
		}
	}
}



int main(void){
	// 사용자의 홈 디렉토리 확인
    	char *home_dir = getenv("HOME");
    	if (home_dir == NULL) {
    		fprintf(stderr, "Error: HOME environment variable is not set\n");
    		exit(1);
    	}
    	    	
    	// 백업 디렉토리 생성
    	sprintf(backup_dir, "%s/backup", home_dir);
    	mkdir(backup_dir, 0777);
    	
    	// 로그파일 생성
    	sprintf(log_dir, "%s/monitor_list.log", backup_dir);
    	int fd = open(log_dir, O_CREAT | O_EXCL | O_RDWR, 0777);
       	close(fd);
    	
	while (1){
		struct stat statbuf;
		
		printf("20201870> ");
		char input[PATH_MAX];
        	fgets(input, PATH_MAX, stdin);
		if(!strcmp(input, "\n")){
			continue;
		}
	        input[strcspn(input, "\n")] = '\0';
        
        	char *cmd = strtok(input, " ");
        	char *path = strtok(NULL, " ");
        	
        	
        	if (strcmp(cmd, "add") == 0) {
            		if (path == NULL) {
                		fprintf(stderr, "Error: No path specified\n");
                		printf("Usage:");
				addhelp();
               			continue;
            		}
			
            		// 옵션 초기화
            		int d_flag = 0, r_flag = 0, y_flag = 0, t_flag = 0;
            		char *t_period="";
	
            		int argc = 1;
            		char *argv[10];
            		argv[0] = "add";
            		char *token = strtok(NULL, " ");
           		while (token != NULL) {
                		argv[argc++] = token;
                		token = strtok(NULL, " ");
            		}
            		
            		argv[argc] = NULL;
            		optind = 1;

            		// 옵션 파싱
            		int opt;
            		while ((opt = getopt(argc, argv, "drt")) != -1) {
                		switch (opt) {
                	    	case 'd':
                	    	    d_flag = 1;
                	    	    break;
                	    	case 'r':
                	   	    r_flag = 1;
                 	   	    break;
                   		case 'y':
                   		    y_flag = 1;
                   		    break;
                   		case 't':
                   		    t_flag = 1;
                   		    t_period = argv[argc-1];
                   		    break;
                  		default:
                        		fprintf(stderr, "Unknown option: -%c\n", opt);
                        		break;
                		}
            		}

        	
        		//경로 길이 확인
    	    		if(strlen(path)>PATH_MAX){
                		fprintf(stderr, "too long path length\n");
            			continue;
        		}
        		//경로 존재 확인
        		if(stat(path, &statbuf)<0){
                		fprintf(stderr, "Error: Wrong path. Please enter a valid path\n");
            			continue;
        		}
        	
        		//절대경로로 변환
        		char abspath[PATH_MAX];
                	if(realpath(path, abspath)==NULL){
                		fprintf(stderr, "Realpath err\n");
            			continue;
                	}
        
        	       	//경로가 home_dir 내부인지 확인
        		int homedirlen = strlen(home_dir);
    			if(strncmp(abspath, home_dir, homedirlen) != 0){
    				fprintf(stderr, "home directory path required\n");
    				continue;
    			}
    			
        		//경로가 home_dir/backup 이하인지 확인
    			int backupdirlen = strlen(backup_dir);
    			if(strncmp(abspath, backup_dir, backupdirlen) == 0){
    				fprintf(stderr, "you cannot select backup dir\n");
    				continue;
    			}
        		//printf("path: %s\n", abspath);
        		//t가 들어왔을경우 음수인지 확인
        		if(t_flag && t_period<0){
                		fprintf(stderr, "wrong period num\n");
                		continue;
        		}
        		
        		//옵션별 파싱, r옵션
        		if(r_flag){
				if(!strcmp(path, home_dir)){
					printf("you cannot contain backup dir\n");
					continue;
				}			
				//프로세스 생성
        			pid_t pid = fork();
        		       	if (pid == 0) {
        				if(t_flag){
        				        addcmd_dr(abspath,"r", t_period);
        				}
        				else{
        					addcmd_dr(abspath,"r",  "");
        				}
            		    		exit(0);
           			}
           			else if (pid > 0) {
					wait(NULL);
           			} 
           			else { 
               			 	fprintf(stderr, "fork fail\n");
               			 	exit(1); 
            			} 
        		}
        		//d 옵션
        		else if(d_flag){
				//프로세스 생성
				pid_t pid = fork();
        		       	if (pid == 0) {
        				if(t_flag){
        				        addcmd_dr(abspath,"d", t_period);
        				}
        				else{
        					addcmd_dr(abspath,"d",  "");
        				}
            		    		exit(0);
           			}
           			else if (pid > 0) {
					wait(NULL);
           			} 
           			else { 
               			 	fprintf(stderr,"fork fail\n");
               			 	exit(1); 
            			} 
        		}
        		else{
				//프로세스 생성
        		       	pid_t pid = fork();
        		       	if (pid == 0) {
        				if(t_flag){
        				        addcmd(abspath, t_period);
        				}
        				else{
        					addcmd(abspath, "");
        				}
            		    		exit(0);
           			}
           			else if (pid > 0) {
					wait(NULL);
           			} 
           			else { 
               			 	fprintf(stderr, "fork fail\n");
               			 	exit(1); 
            			} 
        		}	
        	}        	
        	else if (strcmp(cmd, "remove") == 0) {
        	       	removecmd(path);
        	}
        	else if (strcmp(cmd, "list") == 0) {
        		listcmd(path);
        	}
        	else if (strcmp(cmd, "help") == 0) {
		    	helpcmd(path);
        	}
        	else if (strcmp(cmd, "exit") == 0) {
        		printf("\n");
            		exit(0); 
        	}
        	else{
        		help();
        	}
        	printf("\n");
	}
}

