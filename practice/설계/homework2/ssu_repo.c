#include "ssu_header.h"

char curDirPath[PATH_MAX];

typedef struct commitlinkedlist{
	char commit[PATH_MAX];
	char path[PATH_MAX];
	char commitpath[PATH_MAX];
	struct commitlinkedlist *next;
}commitlinkedlist;
typedef struct bfslinkedlist{
	char dirpath[PATH_MAX];
	struct bfslinkedlist *next;
}bfslinkedlist;

commitlinkedlist *head = NULL;
commitlinkedlist *reverthead = NULL;
bfslinkedlist *bfshead = NULL;

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

//시작과 끝 인덱스를 받아 서브스트링 추출
char *substr(char *str, int beg, int end) {
  char *ret = (char*)malloc(sizeof(char) * (end-beg+1));

  for(int i = beg; i < end && *(str+i) != '\0'; i++) {
    ret[i-beg] = str[i];
  }
  ret[end-beg] = '\0';

  return ret;
}
/*
//디렉토리 경로와 파일이름을 합쳐 경로 반환하는 함수
char *strTodirPATH(char *dirpath, char *filepath) {
  	char *fullpath = (char *)malloc(sizeof(char) * PATH_MAX);
	strcpy(fullpath, dirpath);
	strcat(fullpath,"/");
	if(filepath != NULL)
		strcat(fullpath,filepath);
	return fullpath;
}*/

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
		char filepath[PATH_MAX];
		
		if(path[strlen(path)-1]=='/'){
			sprintf(filepath, "%s%s", path, dentry->d_name);
		}
		else{
			sprintf(filepath, "%s/%s", path, dentry->d_name);
		}
		//printf("path:%s, dname:%s\n",path, dentry->d_name);
		//sprintf(filepath, "%s%s", path, dentry->d_name);
		//char *filepath;
		//filepath = strTodirPATH(path, dentry->d_name);

		
		if (strcmp(dentry->d_name, ".") == 0 || strcmp(dentry->d_name, "..") == 0) continue;

		if((stat(filepath, &filestat))<0){
			fprintf(stderr, "fliestat stat 실패\n%s\n", filepath);
			
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
	
	//.과 .., 파일을 제외한 디렉토리만 문자열 리스트에 담아줌
	while ((dentry = readdir(dir))!=NULL) {
		char filepath[PATH_MAX];

		if(path[strlen(path)-1]=='/'){
			sprintf(filepath, "%s%s/", path, dentry->d_name);
		}
		else{
			sprintf(filepath, "%s/%s/", path, dentry->d_name);		
		}

		//.repopath일 경우에도 담지 않는다
		if (strcmp(dentry->d_name, ".") == 0 || strcmp(dentry->d_name, "..") == 0 || strcmp(dentry->d_name, ".repo") == 0 ) continue;
		

		if((stat(filepath, &filestat))<0){
			//fprintf(stderr, "1fliestat stat 실패\n");
			continue;
		}

		if((S_ISDIR(filestat.st_mode))){
            		filelist[(*filecount)++] = strdup(filepath);
		}
	}
	closedir(dir);
}
////////////////////////유틸함수/////////////////////////////////

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


//현재 스테이징 영역에서 파일들의 경로만 빼오는 함수
void search_files_dirlist(dirNode* dirList, char* file_paths[], int* cnt) {
    dirNode* curr_dir = dirList->subdir_head->next_dir;
    fileNode* curr_file = dirList->file_head->next_file;

    // 디렉토리 탐색
    while (curr_dir != NULL) {
        // 현재 디렉토리 경로를 배열에 추가하고 cnt 증가
        if (curr_dir->dir_path[strlen(curr_dir->dir_path) - 1] != '/') {
            file_paths[(*cnt)++] = curr_dir->dir_path;
        }

        // 재귀적으로 해당 디렉토리 안의 파일 경로 추가
        search_files_dirlist(curr_dir, file_paths, cnt);

        curr_dir = curr_dir->next_dir;
    }

    // 파일 탐색
    while (curr_file != NULL) {
        // 현재 파일 경로를 배열에 추가하고 cnt 증가
        if (curr_file->file_path[strlen(curr_file->file_path) - 1] != '/') {
            file_paths[(*cnt)++] = curr_file->file_path;
        }

        curr_file = curr_file->next_file;
    }
}

//모든 파일 경로를 가져오는 함수
void getAllFilePath();

//스테이징 영역에서 항목을 재귀적으로 확인해 이미 있는 파일인지 확인하는 함수
int recursive_check_staging(dirNode* dirList, char* backup_time, char* path, char* backup_path, char *checktype, int *returnnum) {
  char* ptr;
  
  //경로에 슬래시가 있는 경우
  if(ptr = strchr(path, '/')) {
    char* dir_name = substr(path, 0, strlen(path) - strlen(ptr));
     //printf("dir_rm_nametest:%s\n", dir_name);
    dirNode* curr_dir = dirNode_get(dirList->subdir_head, dir_name);
        if(curr_dir==NULL){
    		*returnnum = 0;
    		return 0;
    	}
    
	//다음 경로로 재귀 호출
    recursive_check_staging(curr_dir, backup_time, ptr+1, backup_path, checktype, returnnum);
    curr_dir->backup_cnt--;
    
    if(curr_dir->backup_cnt == 0) {
      dirNode_remove(curr_dir);
      curr_dir = NULL;
    }
   
  } else {
    char* file_name = path;
     //printf("file_rm_nametest:%s\n", file_name);

	//파일인 경우
    if(!strcmp(checktype, "file")){
        fileNode* curr_file = fileNode_get(dirList->file_head, file_name);
		//파일이 스테이징 구역에 없는 경우
		if(curr_file==NULL){
    		//printf("null!!!!\n");
    		*returnnum = 0;
    		return 0;
    	}
		//파일이 스테이징 구역에 있는 경우
    	else{
    		//printf("exist!!!\n");
    		*returnnum = -1;
    		return 0;
    	}
    }
    //디렉토리를 확인하는 경우
    	else if(!strcmp(checktype, "dir")){
     		char *stagingpaths[PATH_MAX];
     		char *originpaths[PATH_MAX];
  		int stagingcnt = 0, origincnt = 0;
  		

  		dirNode* curr_dir = dirNode_get(dirList->subdir_head, file_name);
    		if(curr_dir==NULL){
    			//printf("null!!!!\n");
    			*returnnum = 0;
    			return 0;
    		}

    		//스테이징로그에 있는 경로들
    		search_files_dirlist(backup_dir_list, stagingpaths, &stagingcnt);
			//현재 디렉토리의 모든 파일 가져오기
    		getAllFilePath(curr_dir->dir_path, originpaths, &origincnt);
    		
    		/*
    		for(int i=0;i<origincnt; i++){
    			printf("origin : %s\n", originpaths[i]);
    		}
    		for(int i=0;i<stagingcnt;i++){
    			printf("staging : %s\n", stagingpaths[i]);    			
    		}
    		*/

			// 모든 원본 파일 경로가 스테이징 영역에 있는지 확인
    		for(int i=0;i<origincnt; i++){
    			char *originpath = originpaths[i];
    			int checkexist = 0;
    			for(int j=0;j<stagingcnt;j++){
    				char *stagingpath = stagingpaths[j];
    				if(!strcmp(originpath, stagingpath)){
    					checkexist = 1;
    					break;
    				}		
    			}
    			if(!checkexist){
    				*returnnum = 0;
    				return 0;
    			}
    		}
			//모든 원본 파일이 스테이징 영역에 있는 경우
    		*returnnum = -1;
    		return 0;

    	}  	
    	return 0;
    }  
  return 0;
}

//이미 스테이징 영역에 있는 파일인지를 리턴하는 함수
int check_already_staging(char *path){
	struct stat statbuf;
 	dirNode* current = backup_dir_list;
 	int returnnum =0;
 	if(stat(path, &statbuf)<0){
 		printf("stat fail4\n");
 	}
 	
	//경로가 디렉토리인 경우
 	if(S_ISDIR(statbuf.st_mode)){
 		recursive_check_staging(current, NULL, path,NULL, "dir", &returnnum);
 		//printf("returnnum : %d\n",returnnum);
 		return returnnum;
 	}
	//경로가 파일인 경우
 	else{
 		recursive_check_staging(current, NULL, path,NULL, "file", &returnnum);
 		return returnnum;
 	}
}

//상대경로를 만드는 함수
char *get_rel_path(char *path){
	char CPpath[PATH_MAX];
	char rel_path[PATH_MAX];
	char *return_rel_path = (char *)malloc(PATH_MAX+5);
	
	
	strcpy(CPpath, path);
	int curDirLen = strlen(curDirPath);
    	strcpy(rel_path, substr(CPpath, curDirLen, strlen(CPpath)));
    	sprintf(return_rel_path, ".%s", rel_path);
    	return return_rel_path;
}

// add 명령어 수행 함수
int addcmd(char *path){
	int baklogFD;
	char baklogcontent[PATH_MAX];
	
	//상대경로를 받아옴
    char *rel_path= get_rel_path(path);
	
	
	if(strlen(path)>PATH_MAX){
		fprintf(stderr, "ERROR: path length is too long\n");
		return 1;
	}
	
	//스테이징 영역에 이미 파일이 존재하는지 확인함
	if(check_already_staging(path)<0){
		printf("\"%s\" already exist in staging area\n", rel_path);
		return 1;
	}

	// 존재하지 않는다면 스테이징 로그에 기록함
	sprintf(baklogcontent, "add \"%s\"\n", path);
	if((baklogFD=open(STAGING_LOG_PATH, O_RDWR | O_APPEND, 0777))<0){
		fprintf(stderr, "baklog open 실패\n");
		exit(1);
	}
	if((write(baklogFD, baklogcontent, strlen(baklogcontent)))<0){
		fprintf(stderr, "baklogFD write실패\n");
		exit(1);
	}
	printf("add \"%s\"\n", rel_path);

}

//재귀적으로 스테이징 구역에 remove에 해당하는 경로가 있는지 확인하는 함수
int recursive_check_removest(dirNode* dirList, char* backup_time, char* path, char* backup_path, int *returnnum) {
  char* ptr;
 	// 경로에 슬래시가 있는 경우
  if(ptr = strchr(path, '/')) {
    char* dir_name = substr(path, 0, strlen(path) - strlen(ptr));
     //printf("dir_rm_nametest:%s\n", dir_name);
    dirNode* curr_dir = dirNode_get(dirList->subdir_head, dir_name);
      	if(curr_dir==NULL){
      		//printf("/// dir null!\n");
      		*returnnum = -1;
    		return -1;
    	}
	//다음 경로로 재귀 호출
    recursive_check_removest(curr_dir, backup_time, ptr+1, backup_path, returnnum);
    curr_dir->backup_cnt--;
    
    if(curr_dir->backup_cnt == 0) {
      dirNode_remove(curr_dir);
      curr_dir = NULL;
    }
   
  } else {
    char* file_name = path;
      	fileNode* curr_file = fileNode_get(dirList->file_head, file_name);
     	dirNode* curr_dir = dirNode_get(dirList->subdir_head, file_name);
      	/*if(curr_file==NULL){
    		printf("file null\n");
    	}
      	if(curr_dir==NULL){
    		printf("dir null\n");
    	}*/
    	
    // 파일과 디렉터리 모두 없으면 -1 반환
	if(curr_file==NULL && curr_dir==NULL){
		(*returnnum) = -1;
	}
	else{
		(*returnnum) = 0;
	}
	//printf("returnnum %d\n",returnnum);
	return 0;
  
    }
}


//링크드리스트 내에 경로에 해당하는 파일이 있는지 리턴하는 함수
int check_already_removedst(char *path){
 	dirNode* current = backup_dir_list;
	int returnnum;

	//재귀적으로 탐색
 	recursive_check_removest(current, NULL, path,NULL, &returnnum);
 	return returnnum;
 
}


// 스테이징 로그 파일에서 모든 줄을 읽어 삭제된적 있는지 확인하는 함수
int checkRemoved(char *path){
	 int len;
 	 char buf[BUF_MAX];
 	 struct stat statbuf;
	int check = 0;
	FILE *file = fopen(STAGING_LOG_PATH, "r");
	
	//파일을 줄 단위로 읽어 remove된 적 있는지 확인
	while((fgets(buf, sizeof(buf), file))!=NULL){
	    	char  *firstpath= strtok(buf, " ");
		if(strstr(firstpath, "remove")!=NULL) {
    			firstpath = strtok(NULL, "\"");
    			firstpath[strlen(firstpath)] = '\0'; 
	
			if(strstr(path, firstpath)!=NULL) {
            			check = 1;	
            		}
            	}
        }
        
        if(check){
        	return -1;
        }
        else{
  		return 0;
  	}
}

//remove 명령어 처리 함수
int removecmd(char *path){
	int baklogFD;
	char baklogcontent[PATH_MAX];
	//상대경로 가져옴	
	char *rel_path = get_rel_path(path);
	//스테이징 로그에 있는지 확인하고 없다면 remove된 적 있는지 확인
	if(check_already_removedst(path)<0 && checkRemoved(path)<0){
		printf("\"%s\" already removed from staging area\n", rel_path);
		return 1;
	}
	
	// 스테이징 로그에 쓰기
	sprintf(baklogcontent, "remove \"%s\"\n", path);
	if((baklogFD=open(STAGING_LOG_PATH, O_RDWR | O_APPEND, 0777))<0){
		fprintf(stderr, "baklog open 실패\n");
		exit(1);
	}
	if((write(baklogFD, baklogcontent, strlen(baklogcontent)))<0){
		fprintf(stderr, "baklogFD write실패\n");
		exit(1);
	}
	
	printf("remove \"%s\"\n",rel_path);

}

// commit 링크드리스트와 비교하여 변동사항을 파악하고 출력하는 함수
int print_change_status(char *path[], int cnt, char *commitdirpath){
	struct stat statbuf1, statbuf2;
	char *newbuf[BUF_MAX], *removebuf[BUF_MAX], *modibuf[BUF_MAX];
	int newcnt = 0, removecnt = 0, modicnt=0; 
	int *checklist= malloc(cnt * sizeof(int));
	
	commitlinkedlist *current = head;

    // 커밋 된적 있는 파일에 있는 파일이면 1로 초기화
   	 for(int i=0;i<cnt; i++){
   	 	checklist[i] = 0;	
   	 }
   	
	//링크드리스트 순회
	while (current != NULL) {     
		//현재 노드(커밋된 노드)가  스테이징 안에 있으면 1이 됨  
		int checknum = 0; 
		
		//현재 노드에 대한 stat 가져오기
		char *commitPath = current->commitpath;
			
		if(stat(commitPath, &statbuf1)<0){
			fprintf(stderr, "stat1 fail:%s:\n",commitPath);
		}
		//스테이징 영역에 있는 파일인지 확인함
		for(int i=0; i<cnt; i++){
			if(checknum>0) continue;
			
			if(stat(path[i], &statbuf2)<0){
				fprintf(stderr, "stat2 fail %s\n",path[i]);
			}
			
			//스테이징 배열에 대한 stat
			if(!strcmp(current ->path, path[i])){
				checklist[i] = 1;
				checknum = 1;

				//파일이 변경되었는지 확인
				//printf("size1:%ld, size2:%ld\n", statbuf1.st_size, statbuf2.st_size);
				if(statbuf1.st_size == statbuf2.st_size && cmpMD5(commitPath, path[i]) == 0){
					//printf("%s -- same\n", current -> path);
				}
				else{
					//수정된 파일인지 확인
					modibuf[modicnt++] = current -> path;
					int modisize = statbuf2.st_size-statbuf1.st_size;

				}
			}
		}
		// 스테이징 영역에 해당 파일이 없으면 삭제된 파일로 처리
		if(!checknum){
			//printf("%s -- removed\n", current -> path);
			removebuf[removecnt++] = current -> path;
		}
        	current = current->next;
   	 }

	//스테이징 영역에 없는 파일이면 새 파일로 처리
   	 for(int i=0;i<cnt; i++){
   	 	if(!checklist[i]){
   	 		newbuf[newcnt++] = path[i];
   	 		if(stat(path[i], &statbuf1)<0){
   	 			fprintf(stderr, "stat1 fail new:%s:\n",path[i]);
   	 		}
   	 	}
   	 }
	 
	 //변동사항이 있으면 출력
	if((modicnt+removecnt+newcnt) >0){
		printf("Changes to be committed:\n");
		mkdir(commitdirpath, 0777);
		
		for(int i=0; i<modicnt; i++){
			char *rel_path = get_rel_path(modibuf[i]);
			printf("    modified: \"%s\"\n",rel_path);
   	 	}
   	 	for(int i=0; i<removecnt; i++){
			char *rel_path = get_rel_path(removebuf[i]);
			printf("    removed: \"%s\"\n",rel_path);  	 	
   	 	}
   	 	for(int i=0; i<newcnt; i++){
   	 		char *rel_path = get_rel_path(newbuf[i]);
			printf("    new file: \"%s\"\n",rel_path);
   	 	}				
	}

	return 0;
}

//untracted files를 출력하는 함수
int print_untracted_status(char *curdirpaths[], int curdircnt){
    char log_line[PATH_MAX * 2];
    int commitcnt = 0, fcnt = 0;
    char checkSTR[PATH_MAX];
    int checkarr[PATH_MAX] = {0};
    struct stat statbuf;
    
    FILE* file = fopen(COMMIT_LOG_PATH, "r");
    
    if (file == NULL) {
        perror("fopen");
        return 0;
    }
	//커밋 로그를 줄별로 읽음
    while (fgets(log_line, PATH_MAX * 2, file) != NULL) {
    	char first[PATH_MAX];
    	char second[PATH_MAX];
    	char third[PATH_MAX];


    	// "commit:"를 기준으로 파싱
    	char* token = strtok(log_line, " ");

    	// "-"를 기준으로 파싱
    	token = strtok(NULL, "-");
    	if (token != NULL) {
    	    strcpy(first, token);
    	}

    	// 마지막 부분을 파싱
    	token = strtok(NULL, "\"");
    	if (token != NULL) {
        	strcpy(second, token);
    	}
    
    	token = strtok(NULL, "\"");
    	if (token != NULL) {
    	    strcpy(third, token);
    	}
	
	
        // 스테이징 영역에 해당 파일이 있으면 checkarr 배열에 표시
        for(int i=0;i<curdircnt;i++){
        	if(!strcmp(curdirpaths[i], third)){
        		checkarr[i] = 1;
        	} 
        }
    }

    int untractedcnt =0;
    for(int i=0;i<curdircnt;i++){
    	if(untractedcnt++==0)
        	printf("\nUntracked files:\n");
	
		// 스테이징 영역에 없는 파일이면서 실제 파일이 regular 파일인 경우
        if(!checkarr[i]){
        	stat(curdirpaths[i], &statbuf);
        	if(S_ISREG(statbuf.st_mode)){ 
        		//staging log에서 한번도 나온적 없는 파일
        	        if(checkRemoved(curdirpaths[i])==0){
        			char *rel_path = get_rel_path(curdirpaths[i]);
        			printf("    new file: \"%s\"\n",rel_path);
        		}
        	}
        
        } 
    }

    fclose(file);

}

//status 명령어를 수행하는 함수
int statuscmd(void){
  char path[PATH_MAX];
  char *paths[PATH_MAX];
  int cnt = 0;
  
  //현재 staging 로그에 있는 파일들을 전부 가져옴
  search_files_dirlist(backup_dir_list, paths, &cnt);

  //변경사항 가져오고 출력
  print_change_status(paths, cnt, path);
  
  char *curdirpaths[PATH_MAX];
  int curdircnt = 0;
  
    //작업 디렉토리 내부의 모든 파일 추출
    getAllFilePath(curDirPath, curdirpaths, &curdircnt);
    //untracted files를 출력하는 함수
    print_untracted_status(curdirpaths, curdircnt);

}

//커밋 이름과 원본파일 경로를 주면 커밋 경로를 알려주는 함수
char *getCommitPath(char *commitPath, char *commitName){
	int curDirLen = strlen(curDirPath);
    	char *path= substr(commitPath, curDirLen, strlen(commitPath));
    	char *repoPath = (char *)malloc(PATH_MAX + 10);
    	sprintf(repoPath, "%s/.repo/%s%s", curDirPath, commitName, path);
    	
    	//printf("repo %s\n", repoPath);
    	return repoPath;
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
	    
		// 디렉터리를 재귀적으로 생성
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

//파일 경로와 커밋 이름을 받아 커밋 디렉토리를 만드는 함수
int commit_to_repo(char *path,char *commitname){   
    int fd1, fd2;
    int len;
    char buf[BUF_MAX];
    char cpCommitPath[PATH_MAX];
    struct stat statbuf, cp_statbuf;

    // 커밋된 파일의 저장 경로 가져오기
    char *repoPath = getCommitPath(path, commitname);    
    strcpy(cpCommitPath, repoPath);
    char *commitDirPath = GetDirPath(cpCommitPath);
    
    // 커밋 디렉터리 및 하위 디렉터리 생성
    mkdirRecursive(commitDirPath);
    
	// 원본, 커밋 파일 열고 복사
    if((fd1 = open(path, O_RDONLY)) <0 ) {
      fprintf(stderr, "ERROR: open error for '%s'\n", path);
      return -1;
    }
    if((fd2 = open(repoPath, O_WRONLY|O_CREAT|O_TRUNC, 0777)) <0) {
      fprintf(stderr, "ERROR: open error for '%s'\n", repoPath);
      return -1;
    }
    while(len = read(fd1, buf, BUF_MAX)) {
      write(fd2, buf, len);
    }
  
  // 원본 파일의 stat 정보 가져오기
    if(stat(path, &statbuf)<0){
    	fprintf(stderr, "stat fail\n");
    	close(fd1);
        close(fd2);
        return -1;
    }
    // 원본 파일과 커밋파일 속성을 같게 설정
    if (fchmod(fd2, statbuf.st_mode) <0) {
    	fprintf(stderr, "fchmod fail\n");
    	close(fd1);
        close(fd2);
        return -1;
    }

    struct utimbuf utbuf;
    utbuf.actime = statbuf.st_atime;
    utbuf.modtime = statbuf.st_mtime;

    // 원본 파일과 커밋파일 시간을 같게 설정
    if (utime(repoPath, &utbuf) == -1) {
    	fprintf(stderr, "utime fail\n");
    	close(fd1);
        close(fd2);
        return -1;
    }
    if (fchown(fd2, statbuf.st_uid, statbuf.st_gid) == -1) {
    	fprintf(stderr, "fchown fail\n");
    	close(fd1);
        close(fd2);
        return -1;
    }
  
    close(fd1);
    close(fd2);
    return 0;
    
}

//커밋 로그를 작성하는 함수
int write_commit_log(char *path, char *commitname, char *command){
	int baklogFD;
	char baklogcontent[PATH_MAX*2];
	char cppath[PATH_MAX];
	
	strcpy(cppath, path);

    // 커밋 로그가 없으면 생성
	if((baklogFD=open(COMMIT_LOG_PATH, O_RDWR | O_APPEND, 0777))<0){
		fprintf(stderr, "baklog open 실패\n");
		exit(1);
	}

    // 커맨드에 따라 백업 로그 내용을 설정
	if(!strcmp(command, "new")){
		sprintf(baklogcontent, "commit: %s - new file: \"%s\"\n",commitname, cppath);	
	}
	else if(!strcmp(command, "modify")){
		sprintf(baklogcontent, "commit: %s - modified: \"%s\"\n",commitname, cppath);	
	}
	else if(!strcmp(command, "remove")){
		sprintf(baklogcontent, "commit: %s - removed: \"%s\"\n",commitname, cppath);	
	}		
	else{
		printf("command error!\n");
		exit(1);
	}	
	
	if((write(baklogFD, baklogcontent, strlen(baklogcontent)))<0){
		fprintf(stderr, "baklogFD write실패\n");
		exit(1);
	}
	close(baklogFD);
	
	
}


//commit 링크드리스트와 비교해가면서 변동사항 파악하고 로그를 작성하는 함수
int commit_and_wirte_log(char *path[], int cnt, char *commitdirpath, char *commitname){
	struct stat statbuf1, statbuf2;
	char *newbuf[BUF_MAX], *removebuf[BUF_MAX], *modibuf[BUF_MAX];
	int newcnt = 0, removecnt = 0, modicnt=0, pluscnt=0, minuscnt=0; 
	int *checklist= malloc(cnt * sizeof(int));
	
	commitlinkedlist *current = head;
	//checklist 초기화
   	 for(int i=0;i<cnt; i++){
   	 	checklist[i] = 0;	
   	 }
   	
    // 링크드 리스트 순회하여 스테이징 영역과 커밋된 파일 비교
	while (current != NULL) {     
		//현재 노드(커밋된 노드)가  스테이징 영역안에 있으면 1이 됨  
		int checknum = 0; 
		
		//현재 노드에 대한 stat
		char *commitPath = current->commitpath;
			
		if(stat(commitPath, &statbuf1)<0){
			fprintf(stderr, "stat1 fail:%s:\n",commitPath);
		}
		//스테이징 영역에 있는 파일들과 비고
		for(int i=0; i<cnt; i++){
			if(checknum>0) continue;
			
			if(stat(path[i], &statbuf2)<0){
				fprintf(stderr, "stat2 fail %s\n",path[i]);
			}
			
            // 현재 파일이 스테이징 영역에 있을 경우
			if(!strcmp(current ->path, path[i])){
				checklist[i] = 1;
				checknum = 1;

                // 파일 크기와 MD5 해시값을 비교하여 수정 여부 확인
				if(statbuf1.st_size == statbuf2.st_size && cmpMD5(commitPath, path[i]) == 0){
					//printf("%s -- same\n", current -> path);
				}
				else{
					// 크기 또는 MD5 해시값이 다르면 수정된 파일로 처리
					modibuf[modicnt++] = current -> path;
					int modisize = statbuf2.st_size-statbuf1.st_size;
					if(modisize>=0){
						pluscnt += modisize;
					}
					else{
						minuscnt += modisize;
					}
				}
			}
		}
		// 스테이징 영역에 해당 파일이 없으면 삭제된 파일로 처리
		if(!checknum){
			removebuf[removecnt++] = current -> path;
			minuscnt-=statbuf1.st_size;
		}
        	current = current->next;
   	 }

    // 스테이징 영역에 없는 파일들을 새 파일로 처리
   	 for(int i=0;i<cnt; i++){
   	 	if(!checklist[i]){
   	 		//printf("%s -- newfile\n", path[i]);
   	 		newbuf[newcnt++] = path[i];
   	 		if(stat(path[i], &statbuf1)<0){
   	 			fprintf(stderr, "stat1 fail new:%s:\n",path[i]);
   	 		}
   	 		pluscnt += statbuf1.st_size;
   	 	}
   	 }
   	 
   	
    // 변경된 파일이 있으면 커밋을 수행하고 로그를 작성
	if((modicnt+removecnt+newcnt) >0){
		printf("commit to %s\n", commitname);
		printf("%d files changed, %d insertions(+), %d deletions(-)\n", modicnt+removecnt+newcnt,pluscnt, minuscnt);
		mkdir(commitdirpath, 0777);
		
		// 수정된 파일 처리, .repo에 커밋 진행
		for(int i=0; i<modicnt; i++){
			write_commit_log(modibuf[i], commitname, "modify");					
			commit_to_repo(modibuf[i], commitname);
			char *rel_path = get_rel_path(modibuf[i]);
			printf("modified: \"%s\"\n",rel_path);
   	 	}
        // 삭제된 파일 처리, .repo에 커밋 진행
   	 	for(int i=0; i<removecnt; i++){
			write_commit_log(removebuf[i], commitname, "remove"); 
			char *rel_path = get_rel_path(removebuf[i]);
			printf("removed: \"%s\"\n",rel_path);  	 	
   	 	}
		// 새로운 파일 처리
   	 	for(int i=0; i<newcnt; i++){
   	 		write_commit_log(newbuf[i], commitname, "new");
   	 		commit_to_repo(newbuf[i], commitname);
   	 		char *rel_path = get_rel_path(newbuf[i]);
			printf("new file: \"%s\"\n",rel_path);
   	 	}				
	}
	// 변경된 파일이 없을경우
	else{
		printf("Nothing to commit\n");
	}   	 
	return 0;
}

//commit 명령 수행하는 함수
int commitcmd(char *commitname){
  if (strchr(commitname, '/') != NULL) {
  	fprintf(stderr,"잘못된 문자가 포함되었습니다.\n");
  	return 1;
  }
  if (strlen(commitname)>=FILE_MAX){
  	fprintf(stderr, "파일 최대길이보다 깁니다.\n");
  	return 1;
  }
  
  //커밋 저장할 경로
  char path[PATH_MAX];
  sprintf(path, "./.repo/%s", commitname);
  if(!access(path, F_OK)) {
     fprintf(stderr, "%s is already exist in repo\n", commitname);
     return 1;
  }
  
  char *paths[PATH_MAX];
  int cnt = 0;
  
  //스테이징 영역에 있는 파일들을 모두 가져옴
  search_files_dirlist(backup_dir_list, paths, &cnt);
    /*for(int i=0; i<cnt; i++){
  	printf("**%s\n",paths[i]);
  }  
  */
  //커밋을 진행하고 로그를 쓰는 함수
  commit_and_wirte_log(paths, cnt, path, commitname);

}

/*
//커밋 이름과 원본파일 경로를 주면 커밋 경로를 알려주는 함수
char *getOriginPath(char *commitPath){
	int curDirLen = strlen(curDirPath)+strlen(REPO_PATH);
    	char *path= substr(commitPath, curDirLen, strlen(commitPath));
    	char *repoPath = (char *)malloc(PATH_MAX + 10);
    	sprintf(repoPath, "%s/%s", curDirPath, path);

    	return repoPath;
}
*/


//해당 commit의 내부에 있는 파일들의 원본파일을 가져와 비교 후 교체
int revert_commit(char *path[], int cnt, char *commitname){
	int fd1, fd2;
    	int len;
    	int revertcheck = 0;
    	char buf[BUF_MAX];
    	char cpCommitPath[PATH_MAX];
    	char *commitlist[BUF_MAX];
    	struct stat statbuf1, statbuf2;

    //revert 링크드리스트에서 커밋한 파일이 나올때 까지 앞 노드는 건너뜀
	while (reverthead != NULL) {
  		if(!strcmp(reverthead->commit, commitname)){
  			printf("revertbreak:%s\n", commitname);			
    			break;
    		}
    		reverthead = reverthead ->next;
    	}
    // 작업 디렉토리 내의 모든 파일에 대해 반복합
	for(int i=0;i<cnt; i++){
		int checknum = 0;
		commitlinkedlist *current = reverthead;
	
		while (current != NULL) {
			//노드의 커밋명과 커밋명이 같다면 checknum ++
			if(!strcmp(current->commit, commitname)){
				checknum++;
				//printf("checknum %d\n", checknum);
			}
			//checknum이 1이상이면 진행
			if(checknum>0){
				//둘이 원본경로 비교해서 같으면 복구하고 다음으로 넘어감
				if(!strcmp(current->path, path[i])){
				    char * commitPath = current->commitpath;	
				
				if(stat(commitPath, &statbuf1)<0){
					fprintf(stderr, "stat1 fail:%s:\n",commitPath);
				}
		
				if(stat(path[i], &statbuf2)<0){
					fprintf(stderr, "stat2 fail %s\n",path[i]);
				}
				
				    //파일의 크기 비교 후 md5 비교해 내용이 다른지 확인
				    if(statbuf1.st_size != statbuf2.st_size && cmpMD5(commitPath, path[i]) != 0){
					char bufbuf[PATH_MAX*2 + 50];
					// 복구된 파일 목록에 추가합니다.
					sprintf(bufbuf, "recover \"%s\" from %s", current -> path, current ->commit);
					commitlist[revertcheck++] = strdup(bufbuf);
						
					//파일 복구
					if((fd1 = open(commitPath, O_RDONLY)) == -1) {
      						fprintf(stderr, "ERROR: open error for '%s'\n", path[i]);	
      						return -1;
    					}
		
    					if((fd2 = open(path[i], O_WRONLY|O_CREAT|O_TRUNC, 0777)) == -1) {
      						fprintf(stderr, "ERROR: open error for '%s'\n", path[i]);
      						return -1;
    					}
    		
    					while(len = read(fd1, buf, BUF_MAX)) {
      						write(fd2, buf, len);
    					}
      					close(fd1);
    					close(fd2);
				    }
    				    break;	
				}	
			}
			current = current->next;
   	 	}	
	}

	//복구된 파일이 있는 경우 목록 출력
	if(revertcheck){
		printf("revert to %s\n", commitname);
		for(int i=0; i<revertcheck; i++){
			printf("%s\n", commitlist[i]);
		}
	}
	//없을 경우 메시지 출력
	else{
		printf("nothing changed with %s", commitname);
	}    		
}

//원본경로와 커밋 이름을 받아 커밋 경로를 반환하는 함수
char *make_commit_path(char *origin_path, char *commitname){
	char CPpath[PATH_MAX];
	char rel_path[PATH_MAX];
	char *return_commit_path = (char *)malloc(PATH_MAX+5);
	
	strcpy(CPpath, origin_path);
	int curDirLen = strlen(curDirPath);
    	strcpy(rel_path, substr(CPpath, curDirLen, strlen(CPpath)));
    	sprintf(return_commit_path, "./.repo/%s%s", commitname, rel_path);
    	return return_commit_path;

}


//new file과 modified를 전부 링크드리스트에 붙이는 함수
void makerevertlist(char *first, char *second, char *third){

	//new file또는 modified에 대해서만 링크드리스트 노드 생성
	if(!strcmp(second, " new file: ") || strcmp(second, " modified: ")==0){
		commitlinkedlist *newbackupnode = malloc(sizeof(commitlinkedlist));
		strcpy(newbackupnode -> commit, first);
		
		
		strcpy(newbackupnode -> path, third);
		
		char *commitpath = make_commit_path(third, first);
		strcpy(newbackupnode -> commitpath, commitpath);
		
		
		newbackupnode -> next = reverthead;
		reverthead = newbackupnode;
	}
	//remove일 경우에는 무시
	else if(strcmp(second, " removed: ")==0){
	}
	else{
	fprintf(stderr, "command 인식실패");
	exit(1);
	}
	
}

//로그파일을 한 줄씩 읽어서 정보를 만들어 makelinkedlist()함수에 넣어주는 함수
void makeRevertListInfo(){
    char log_line[PATH_MAX * 2];
    FILE* file = fopen(COMMIT_LOG_PATH, "r");

    if (file == NULL) {
        perror("fopen");
        return;
    }

	//커밋 로그를 한줄씩 읽음
    while (fgets(log_line, PATH_MAX * 2, file) != NULL) {
    	char first[PATH_MAX];
    	char second[PATH_MAX];
    	char third[PATH_MAX];

    	// "commit:"를 기준으로 파싱
    	char* token = strtok(log_line, " ");

    	// "-"를 기준으로 파싱
    	token = strtok(NULL, "-");
    	if (token != NULL) {
    	    strcpy(first, token);
    	}

    	// 마지막 부분을 파싱
    	token = strtok(NULL, "\"");
    	if (token != NULL) {
        	strcpy(second, token);
    	}
    
    	token = strtok(NULL, "\"");
    	if (token != NULL) {
    	    strcpy(third, token);
    	}

        int rm = strlen(first);
        first[rm-1] = '\0';
        
        // 커밋 되었던 파일들에 대해 링크드리스트 생성
        makerevertlist(first, second, third);
    }

    fclose(file);
}

//revert 명령어 수행하는 함수
int revertcmd(char *commitname){
 if (strchr(commitname, '/') != NULL) {
  	fprintf(stderr,"잘못된 문자가 포함되었습니다.\n");
  	return 1;
  }
  if (strlen(commitname)>=FILE_MAX){
  	fprintf(stderr, "파일 최대길이보다 깁니다.\n");
  	return 1;
  }
  
  char path[PATH_MAX];
  sprintf(path, "./.repo/%s", commitname);
  // 커밋이 존재하는지 확인.
  if(!access(path, F_OK)) {
     //fprintf(stderr, "same commit is already exist\n");
     //return 1;
  }
  else{
  	fprintf(stderr,"ERROR: same commit is not exist\n");
  	return 1;
  }
  
    reverthead=NULL;
    //커밋 로그를 읽어와 new file과 modify만 가져옴
    makeRevertListInfo();

  	commitlinkedlist *current = reverthead;
	int ct = 1;
	/*
	printf("revert 링크드리스트 \n");
    	while (current != NULL) {
        	printf("%d. commit:%s, Path:%s, commitpath:%s\n", ct++, current->commit, current->path, current -> commitpath);
        	current = current->next;
   	 }
   	 printf("\n");
 */
 
  char *curdirpaths[PATH_MAX];
  int curdircnt = 0;
    //작업 디렉토리 내부의 모든 파일 추출
    getAllFilePath(curDirPath, curdirpaths, &curdircnt);
    
  //지금까지 커밋되었던 파일들과 현재 작업디렉토리 내부에 존재하는 파일들을 가지고 revert진행
  revert_commit(curdirpaths, curdircnt, commitname);
}

//커밋명을 받았을 경우 log 명령어를 수행하는 함수
int logcmd(char *commitname){
//로그파일을 한 줄씩 읽어서 정보를 만들어 makelinkedlist()함수에 넣어주는 함수
    char log_line[PATH_MAX * 2];
    FILE* file = fopen(COMMIT_LOG_PATH, "r");
    int commitcnt = 0, checkexist = 0;

    if (file == NULL) {
        perror("fopen");
        return 0;
    }
	//커밋 로그를 한줄씩 읽음
    while (fgets(log_line, PATH_MAX * 2, file) != NULL) {
    	char first[PATH_MAX];
    	char second[PATH_MAX];
    	char third[PATH_MAX];

    	// "commit:"를 기준으로 파싱
    	char* token = strtok(log_line, " ");

    	// "-"를 기준으로 파싱
    	token = strtok(NULL, "-");
    	if (token != NULL) {
    	    strcpy(first, token);
    	}

    	// 마지막 부분을 파싱
    	token = strtok(NULL, "\"");
    	if (token != NULL) {
        	strcpy(second, token);
    	}
    
    	token = strtok(NULL, "\"");
    	if (token != NULL) {
    	    strcpy(third, token);
    	}
        // 커밋명이 지정된 커밋명과 일치하는 경우 해당 로그 정보를 출력
        int rm = strlen(first);
        first[rm-1] = '\0';
        
        if(!strcmp(first, commitname)){
        	checkexist = 1;
        	if(commitcnt == 0){
        		printf("commit: %s\n", first);
        		commitcnt++;
        	}
        	printf(" - %s\"%s\"\n", second, third);
        
        }
    }

	// 지정된 커밋명이 존재하지 않는 경우 오류 출력
    if(!checkexist){
    	fprintf(stderr, "ERROR: version directory isn't exist\n");    
    }
    fclose(file);
}

//커밋명 없이 log 명령어를 수행하는 함수
int all_logcmd(void){
    char log_line[PATH_MAX * 2];
    FILE* file = fopen(COMMIT_LOG_PATH, "r");
    int commitcnt = 0, fcnt = 0;
    char checkSTR[PATH_MAX];
    
    //checkSTR = "/";
    if (file == NULL) {
        perror("fopen");
        return 0;
    }

	//커밋 로그 한줄씩 읽음
    while (fgets(log_line, PATH_MAX * 2, file) != NULL) {
    	char first[PATH_MAX];
    	char second[PATH_MAX];
    	char third[PATH_MAX];


    	// "commit:"를 기준으로 파싱
    	char* token = strtok(log_line, " ");

    	// "-"를 기준으로 파싱
    	token = strtok(NULL, "-");
    	if (token != NULL) {
    	    strcpy(first, token);
    	}

    	// 마지막 부분을 파싱
    	token = strtok(NULL, "\"");
    	if (token != NULL) {
        	strcpy(second, token);
    	}
    
    	token = strtok(NULL, "\"");
    	if (token != NULL) {
    	    strcpy(third, token);
    	}

        int rm = strlen(first);
        first[rm-1] = '\0';
        
        // 이전 커밋명과 현재 커밋명이 다를 경우 개행을 추가
        if(strcmp(first, checkSTR)){
        	commitcnt = 0;
        	if(fcnt){
        		printf("\n");
        	}
      		fcnt++;
        }
        
    	// 첫 번째 커밋 메시지면 커밋명 출력
        if(commitcnt == 0){
        	printf("commit: %s\n", first);
        	commitcnt++;
        }
		//커밋 메시지 출력
        printf(" - %s\"%s\"\n", second, third);
        strcpy(checkSTR, first);
        
    }

    fclose(file);
  
}

int helpcmd(char *path){
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
/*
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


void print_list(dirNode* dirList, int depth, int last_bit) {
  dirNode* curr_dir = dirList->subdir_head->next_dir;
  fileNode* curr_file = dirList->file_head->next_file;

  while(curr_dir != NULL && curr_file != NULL) {
    if(strcmp(curr_dir->dir_name, curr_file->file_name) < 0) {
      print_depth(depth, last_bit);
      printf("%s/\n", curr_dir->dir_name);
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
    printf("%s/\n", curr_dir->dir_name);
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
*/

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

//스테이징로그를 읽은 정보로 파일, 디렉토리를 구분해 링크드리스트에 노드를 삽입하는 함수
int backup_list_insert();

//내부의 파일, 디렉토리들을 backup_list_insert에 넣어주는 함수
int new_recursive_insert(char *dirPath){
    	int pathcnt = 0;
	char *pathlist[PATH_MAX];

    //해당 디렉토리 내부를 재귀적으로 탐색, 파일 경로 가져옴 -> 전부 backup_list_insert에 파일로 돌림
	getAllFilePath(dirPath, pathlist, &pathcnt);
	
	for(int i=0;i<pathcnt; i++){
		//printf("실제파일 : %s\n", pathlist[i]);
		backup_list_insert(backup_dir_list, NULL, pathlist[i], NULL, "file");
	}

}

//스테이징로그를 읽은 정보로 파일, 디렉토리를 구분해 링크드리스트에 노드를 삽입하는 함수
int backup_list_insert(dirNode* dirList, char* backup_time, char* path, char* backup_path, char *checktype) {
  char* ptr;

  //디렉토리 경로에 슬래시가 있는 경ㅡ
  if(ptr = strchr(path, '/')) {
    //디렉토리 이름 추출
    char* dir_name = substr(path, 0, strlen(path) - strlen(ptr));
    // 현재 디렉토리에 새 디렉토리 노드를 삽입하고 재귀적으로 호출
    dirNode* curr_dir = dirNode_insert(dirList->subdir_head, dir_name, dirList->dir_path);
    backup_list_insert(curr_dir, backup_time, ptr+1, backup_path, checktype);
    curr_dir->backup_cnt++;

  } else {
    char* file_name = path;
	//파일인경우
    if(!strcmp(checktype, "file")){
        fileNode* curr_file = fileNode_insert(dirList->file_head, file_name, dirList->dir_path);
    }
	//디렉토리인 경어
    else if(!strcmp(checktype, "dir")){
        // 현재 디렉토리에 새 디렉토리 노드를 삽입하고 백업 카운트 증가
    	dirNode* curr_dir = dirNode_insert(dirList->subdir_head, file_name, dirList->dir_path);
    	curr_dir->backup_cnt++;

        // 재귀적으로 디렉토리 내의 파일을 백업 리스트에 추가
    	new_recursive_insert(curr_dir->dir_path);
    }
  }

  return 0;
}

//스테이징로그를 읽은 정보로 링크드리스트에서 노드를 제거하는 함수
int backup_list_remove(dirNode* dirList, char* backup_time, char* path, char* backup_path, char *checktype) {
  char* ptr;
    // 디렉토리 경로에 슬래시가 있는 경우
  if(ptr = strchr(path, '/')) {
	// 디렉토리 이름 추출
    char* dir_name = substr(path, 0, strlen(path) - strlen(ptr));
    dirNode* curr_dir = dirNode_get(dirList->subdir_head, dir_name);
        if(curr_dir==NULL){
    		//printf("null!!!!\n");
    		return 1;
    	}
    backup_list_remove(curr_dir, backup_time, ptr+1, backup_path, checktype);
    curr_dir->backup_cnt--;
    
    if(curr_dir->backup_cnt == 0) {
      dirNode_remove(curr_dir);
      curr_dir = NULL;
    }
   
  } else {
    char* file_name = path;
	//파일인 경우 파일 노드 제거
    if(!strcmp(checktype, "file")){
        fileNode* curr_file = fileNode_get(dirList->file_head, file_name);
    	if(curr_file==NULL){
    		//printf("null!!!!\n");
    		return 1;
    	}
    	else{
        	fileNode_remove(curr_file);
     		curr_file = NULL;
    	}
    }
	//디렉토리인 경우 디렉토리 노드 제거
    else if(!strcmp(checktype, "dir")){

    	char dir_name[PATH_MAX];
    	sprintf(dir_name, "%s/", file_name);
    	dirNode* curr_dir = dirNode_get(dirList->subdir_head, file_name);
    	if(curr_dir==NULL){
    		//printf("dif null!!!!\n");
    		return 1;
    	}
    	else{
    		dirNode_remove(curr_dir);
     		curr_dir = NULL;
    	}	
    }  
  }

  return 0;
}

//스테이징 로그를 읽어 정보를 분리 후 노드를 추가, 제거하는 함수
int init_backup_list(void) {
 	 int len;
 	 char buf[BUF_MAX];
 	 struct stat statbuf;

	FILE *file = fopen(STAGING_LOG_PATH, "r");
	// 스테이징 로그 파일을 열고 파일 내용을 한 줄씩 읽어들임

	while((fgets(buf, sizeof(buf), file))!=NULL){
		//add 처리
		if((strstr(buf, "add")) != NULL) {
            		char *firstpath = strtok(buf, " "); 
            		firstpath = strtok(NULL, "\"");  
            		firstpath[strlen(firstpath)] = '\0'; 
	            // 파일의 상태 정보 확인 후 조회 실패시 넘어감 			 
     			if(stat(firstpath, &statbuf)<0){
     				//fprintf(stderr, "stat fail2\n");
     				continue;
     			}
     			            
				// 디렉토리인지 파일인지 확인하여 링크드 리스트에 추가
     			if(S_ISDIR(statbuf.st_mode)){
				backup_list_insert(backup_dir_list, NULL, firstpath, NULL, "dir");
     			}
     			else{
     				backup_list_insert(backup_dir_list, NULL, firstpath, NULL, "file");
     			}

   		 }
		 //remove 처리
    		if((strstr(buf, "remove")) != NULL) {
    			char  *firstpath= strtok(buf, " ");
    			firstpath = strtok(NULL, "\"");
    			firstpath[strlen(firstpath)] = '\0'; 
	            // 파일의 상태 정보 확인 후 조회 실패시 넘어감 			 
     			if(stat(firstpath, &statbuf)<0){
     				//fprintf(stderr, "stat fail3\n");
     				continue;
     			}
            	// 디렉토리인지 파일인지 확인하여 링크드 리스트에서 제거
     			if(S_ISDIR(statbuf.st_mode)){
      				backup_list_remove(backup_dir_list, NULL, firstpath, NULL, "dir");
     			}
     			else{
      				backup_list_remove(backup_dir_list, NULL, firstpath, NULL,"file");
     			}

     			 

    		}
  	}

   	//print_list(backup_dir_list, 0, 0);

  return 0;
}


//인자로 받은 정보로 링크드리스트를 생성하는 함수
void makelinkedlist(char *first, char *second, char *third){

	//링크드리스트 노드를 새로 생성
	if(!strcmp(second, " new file: ")){
		commitlinkedlist *newbackupnode = malloc(sizeof(commitlinkedlist));
		strcpy(newbackupnode -> commit, first);
		
		
		strcpy(newbackupnode -> path, third);
		
		char *commitpath = make_commit_path(third, first);
		strcpy(newbackupnode -> commitpath, commitpath);
		
		
		newbackupnode -> next = head;
		head = newbackupnode;
	}
	//commit 값만 받은 정보로 변경
	else if(strcmp(second, " modified: ")==0){
		commitlinkedlist *current = head;
	    	while (current != NULL) {
        		if(strcmp(third, current->path)==0){
				strcpy(current -> commit, first);
				char *commitpath = make_commit_path(third, first);
				strcpy(current -> commitpath, commitpath);
			}
        		current = current->next;
		}
	}
	//링크드리스트 노드 삭제
	else if(strcmp(second, " removed: ")==0){
		commitlinkedlist *prev = NULL;
		commitlinkedlist *current = head;
	    	while (current != NULL) {
        		if(strcmp(third, current->path)==0){
				if(prev!=NULL){
					prev->next = current->next;
				}
				else{
					head = current->next;
				}
				free(current);
				break;	
			}
			prev = current;
        		current = current->next;
		}
	}
	
	else{
	fprintf(stderr, "command 인식실패");
	exit(1);
	}
	
}

//로그파일을 한 줄씩 읽어서 정보를 만들어 makelinkedlist()함수에 넣어주는 함수
void makecommitlistinfo(){
    char log_line[PATH_MAX * 2];
    FILE* file = fopen(COMMIT_LOG_PATH, "r");

    if (file == NULL) {
        perror("fopen");
        return;
    }

    while (fgets(log_line, PATH_MAX * 2, file) != NULL) {
    	char first[PATH_MAX];
    	char second[PATH_MAX];
    	char third[PATH_MAX];

    	// "commit:"를 기준으로 파싱
    	char* token = strtok(log_line, " ");

    	// "-"를 기준으로 파싱
    	token = strtok(NULL, "-");
    	if (token != NULL) {
    	    strcpy(first, token);
    	}

    	// 마지막 부분을 파싱
    	token = strtok(NULL, "\"");
    	if (token != NULL) {
        	strcpy(second, token);
    	}
    
    	token = strtok(NULL, "\"");
    	if (token != NULL) {
    	    strcpy(third, token);
    	}

        int rm = strlen(first);
        first[rm-1] = '\0';
        
        
        //printf("first:%s, command:%s, second:%s\n", first, second, third);
        // 연결 리스트에 정보 추가
        makelinkedlist(first, second, third);
    }

    fclose(file);
}
//ssu_repo 초기 세팅 함수
int init(char* path) {
  int commit_log_fd, staging_log_fd;

  exe_path = path;
  pwd_path = (char *)getenv("PWD");
   char *home_dir = getenv("HOME");
    //printf("user home dir: %s\n", home_dir);

  // 레포지토리 경로가 존재하지 않는 경우 생성
  if(access(REPO_PATH, F_OK)) {
    if(mkdir(REPO_PATH, 0755) <0) {
      return -1;
    };
  }

  //커밋 로그 파일 없으면 생성
  if((commit_log_fd = open(COMMIT_LOG_PATH, O_RDWR|O_CREAT, 0777)) <0) {
    fprintf(stderr, "ERROR: open error for '%s'\n", BACKUP_LOG_PATH);
    return -1;
  }
  //스테이징 로그 파일 없으면 생성
  if((staging_log_fd = open(STAGING_LOG_PATH, O_RDWR|O_CREAT, 0777)) <0) {
    fprintf(stderr, "ERROR: open error for '%s'\n", BACKUP_LOG_PATH);
    return -1;
  }
  
  //staging.log로 링크드리스트 만들기
  init_backup_list();
 
 //commit.log로 링크드리스트 만들기
  makecommitlistinfo();


  close(commit_log_fd);
   close(staging_log_fd);

 	commitlinkedlist *current = head;
	int ct = 1;
	
	/*
	printf("전체 링크드리스트 \n");
    	while (current != NULL) {
        	printf("%d. commit:%s, Path:%s, commitpath:%s\n", ct++, current->commit, current->path, current -> commitpath);
        	current = current->next;
   	 }
   	 printf("\n");
   	 */

  return 0;
  
}
/*
int to_abs_path(char *path, char*newpath){
	char *abspath;
	if(path[0]=='.'){
			printf("--%s", path);
		strcpy(abspath, path+1);
		sprintf(newpath, "%s%s", curDirPath, abspath);
		printf("--%s", newpath);
	}
	else if(path[0]!='/'){
		sprintf(newpath, "%s/%s", curDirPath, path);
	}
	else{
		strcpy(newpath, path);
	}
	return 0;
}
*/

//main 함수
int main(int argc, char* argv[]) {

    if(argc>1){
        if(init(argv[0]) == -1) {
    		fprintf(stderr, "ERROR: init error.\n");
    		exit(0);
  	}
    	if (strcmp(argv[1], "add") == 0) {
    		addcmd(argv[2]);
    	}
    	else if (strcmp(argv[1], "remove") == 0) {
    		
    		 printf("remove");
    		//removecmd(argv[2]);
    	}
    	else if (strcmp(argv[1], "status") == 0) {
    		statuscmd();
    	}
    	else if (strcmp(argv[1], "commit") == 0) {
    		commitcmd(argv[2]);
    	}
    	else if (strcmp(argv[1], "revert") == 0) {
    		revertcmd(argv[2]);
    	}
    	else if (strcmp(argv[1], "log") == 0) {
    		if(argv[2]==0){
            		all_logcmd();
            	}        	
            	else{
            	        if(strlen(argv[2])>=FILE_MAX){
        			fprintf(stderr, "ERROR: file size is too big\n");
        			exit(0);
        		}
            	
            		logcmd(argv[2]); 
            	}
    	}
    	else if (strcmp(argv[1], "help") == 0) {
    		helpcmd(argv[2]);
    	}    	
	exit(0);
    }
   
	//현재 작업 디렉토리 가져오기
	if(getcwd(curDirPath, sizeof(curDirPath)) ==NULL){
		printf("getcwdfail\n");
	}
	//printf("%s\n", curDirPath);
	struct stat statbuf;
    	backup_dir_list = (dirNode*)malloc(sizeof(dirNode)); 
  	dirNode_init(backup_dir_list);
    while (1) {
    	head = NULL;
   	
   	
    	if(init(argv[0]) == -1) {
    		fprintf(stderr, "ERROR: init error.\n");
    		continue;
  	}
    
	printf("20201870> ");

        char input[PATH_MAX];
        fgets(input, PATH_MAX, stdin);
	if(!strcmp(input, "\n")){
		continue;
	}
	
        input[strcspn(input, "\n")] = '\0';
        
        char *cmd = strtok(input, " ");
        char *path = strtok(NULL, "");

        if (strcmp(cmd, "add") == 0) {
            pid_t pid = fork();
            if (pid == 0) {
            	if(path==0){
            		printf("ERROR: <PATH> is not include\nUsage: ");
            		addhelp();
            		exit(0);
            	}
            	if(stat(path, &statbuf)<0){
            		fprintf(stderr,"ERROR: ‘%s’ is wrong path\n", path);
            		exit(1);
            	}
                char abspath[PATH_MAX];
                if(realpath(path, abspath)==NULL){
                	fprintf(stderr, "Realpath err\n");
            		exit(1);
                }
                if(strlen(abspath)>PATH_MAX){
			fprintf(stderr, "ERROR: path length is too long\n");
            		exit(1);
		}
		
		if(!S_ISDIR(statbuf.st_mode) && !S_ISREG(statbuf.st_mode)){
			fprintf(stderr, "ERROR: path is not dir or reg\n");
            		exit(1);
		}
		if(access(path, F_OK)) {
     			fprintf(stderr, "ERROR: cannot access to the file\n");
            		exit(1);
  		}
  		
  		if(strstr(abspath, curDirPath)==NULL){
  			fprintf(stderr, "ERROR: this file is not in the current dir\n");
            		exit(1);
  		}
  		
  		char repoPath[PATH_MAX+10];
  		sprintf(repoPath, "%s/.repo", curDirPath);
  		if(strstr(abspath, repoPath) !=NULL){
  			fprintf(stderr, "ERROR: path include repo dir\n");
            		exit(1);
  		}
 
            	    // 자식 프로세스
            		addcmd(abspath);
    			//execl("./ssu_repo","./ssu_repo", "add", abspath, (char *)0);            		
    			//execl("./addcmd","./addcmd", abspath, (char *)0);

            	    exit(0);
           }
           else if (pid > 0) {
             	   // 부모 프로세스가 자식 프로세스가 종료될 때까지 sleep
		wait(NULL);
		sleep(1);
           } 
           else {
                printf("fork fail\n");
                exit(1);
           }
        } 
        else if (strcmp(cmd, "remove") == 0) {
                        
            pid_t pid = fork();
            if (pid == 0) {
            	if(path==0){
            		printf("ERROR: <PATH> is not include\nUsage: ");
            		removehelp();
            		exit(0);
            	}
            	if(stat(path, &statbuf)<0){
            		fprintf(stderr,"ERROR: ‘%s’ is wrong path\n", path);
            		exit(1);
            	}
            	
                char abspath[PATH_MAX];
                //to_abs_path(path, abspath);
                if(realpath(path, abspath)==NULL){
                	fprintf(stderr, "Realpath err\n");
            		exit(1);
                }
                if(strlen(abspath)>PATH_MAX){
			fprintf(stderr, "ERROR: path length is too long\n");
			exit(1);
		}
                if(strstr(abspath, curDirPath)==NULL){
  			fprintf(stderr, "ERROR: this file is not in the current dir\n");
     			exit(1);
  		}
            		//printf("abspath:%s\n", abspath);
            	    // 자식 프로세스
            		removecmd(abspath);
    			//execl("./ssu_repo","./ssu_repo", "remove", abspath, (char *)0);
            	    exit(0);
           }
           else if (pid > 0) {
             	   // 부모 프로세스가 자식 프로세스가 종료될 때까지 sleep
		wait(NULL);
		sleep(1);
           } 
           else {
                printf("fork fail\n");
                exit(1);
            }
        } 
        else if (strcmp(input, "status") == 0) {
        
                pid_t pid = fork();
            	if (pid == 0) {
            	    // 자식 프로세스
            		statuscmd();
    			//execl("./ssu_repo","./ssu_repo", "status", (char *)0);
            	    exit(0);
           	 }
             	else if (pid > 0) {
             	   // 부모 프로세스가 자식 프로세스가 종료될 때까지 sleep
		//wait(NULL);
		sleep(1);
            	} else {

                printf("fork fail\n");
                exit(1);
            }
        } 
        else if (strcmp(input, "commit") == 0) {
            pid_t pid = fork();
            if (pid == 0) {
            	if(path==0){
            		printf("ERROR: <NAME> is not include\nUsage: ");
            		commithelp();
            		exit(0);
            	}
            	    // 자식 프로세스
            	    commitcmd(path);
    		    //execl("./ssu_repo2","./ssu_repo2", "commit", path, (char *)0);
            	    exit(0);
           }
           else if (pid > 0) {
             	   // 부모 프로세스가 자식 프로세스가 종료될 때까지 sleep
		wait(NULL);
		sleep(1);
           } 
           else {
                printf("fork fail\n");
                exit(1);
            }

        } 
        else if (strcmp(input, "revert") == 0) {
            pid_t pid = fork();
            if (pid == 0) {
            	if(path==0){
            		printf("ERROR: <NAME> is not include\nUsage: ");
            		reverthelp();
            		continue;
            	}
            	
            	    // 자식 프로세스
            		revertcmd(path);
    			//execl("./ssu_repo","./ssu_repo", "revert", path, (char *)0);
            	    exit(0);
           }
           else if (pid > 0) {
             	   // 부모 프로세스가 자식 프로세스가 종료될 때까지 sleep
		wait(NULL);
		sleep(1);
           } 
           else {
                printf("fork fail\n");
                exit(1);
            } 	
        } 
        else if (strcmp(input, "log") == 0) {
           pid_t pid = fork();
           if (pid == 0) {
            	    // 자식 프로세스
		struct stat statbuf;

        	if(stat(COMMIT_LOG_PATH, &statbuf)<0){
        		printf("commit log stat fail\n");
      			continue;
        	}
        	
        	if(!statbuf.st_size){
        		fprintf(stderr, "ERROR: commit file empty\n");
        		continue;
        	}
        	
		if(path==0){
            		all_logcmd();
            	}        	
            	else{
            	        if(strlen(path)>=FILE_MAX){
        			fprintf(stderr, "ERROR: file size is too big\n");
        			continue;
        		}
            	
            		logcmd(path);
            	}
    		//execl("./ssu_repo","./ssu_repo", "log", path, (char *)0);
            	exit(0);
           }
           else if (pid > 0) { 
             	   // 부모 프로세스가 자식 프로세스가 종료될 때까지 sleep
		wait(NULL);
		sleep(1);
           } else {

                printf("fork fail\n");
                exit(1);
            }	   	
        } else if (strcmp(input, "help") == 0) {
            pid_t pid = fork();
            if (pid == 0) {
		    helpcmd(path);
            	    // 자식 프로세스
    		    //execl("./ssu_repo","./ssu_repo", "help", path, (char *)0);
            	    exit(0);
           }
           else if (pid > 0) {
             	   // 부모 프로세스가 자식 프로세스가 종료될 때까지 sleep 
		wait(NULL);
		sleep(1);
           } 
           else { 
                printf("fork fail\n");
                exit(1); 
            } 	
		
		
        } else if (strcmp(input, "exit") == 0) {
            printf("\n");
            exit(0); 
        } else {
       		help();
            //printf("Unknown command: %s\n", input);        
        } 
        printf("\n");
    }
   
}

