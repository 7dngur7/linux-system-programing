#include "ssu_header.h"

//시간 가져와주는 함수(백업 디렉토리 이름)
char *getDate() {
	char *date = (char *)malloc(sizeof(char) * 14);
	time_t timer;
	struct tm *t;

	timer = time(NULL);	
	t = localtime(&timer);

  	sprintf(date, "%02d%02d%02d%02d%02d%02d",t->tm_year %100, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
  
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

	printf("1.%s\n2.%s\n",originalMD5,backupMD5);
	return strcmp(originalMD5, backupMD5);
}

//천단위로 쉼표 찍어주는 함수
char *cvtNumComma(int a) {
  	char *str = (char *)malloc(sizeof(char) * STRMAX);
  	char *ret = (char *)malloc(sizeof(char) * STRMAX);
  	int i;
  	for(i = 0; a > 0; i++) {
    		str[i] = a%10 + '0';
    		a /= 10;
    		if(i%4 == 2) {
     	 		i++;
     	 		str[i] = ',';
    		}
 	}
  	str[i] = '\0';

  	for(i = 0; i < strlen(str); i++) {
    		ret[i] = str[strlen(str)-i-1];
  	}
  	ret[i] = '\0';
  
  	return ret;
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
  	char *fullpath = (char *)malloc(sizeof(char) * PATHMAX);
	strcpy(fullpath, dirpath);
	strcat(fullpath,"/");
	if(filepath != NULL)
		strcat(fullpath,filepath);
	return fullpath;
}

//파일 이름을 제외한 디렉토리 경로만 반환하는 함수
char *GetDirPath(char *path){
	char *dirpath = (char *)malloc(sizeof(char) * PATHMAX);
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


// 해당 경로의 파일이 이미 백업되었는지 확인하는 함수, 이미 백업되었으면 -1리턴
int checkAlreadyBackuped(char *path){
	backuplinkedlist *current = head;
	struct stat originst;
	
	if(stat(path, &originst)<0){
        	fprintf(stderr, "origin stat 실패\n");
        	exit(1);
       }

	//링크드리스트 순회
	while (current != NULL) {        
		//printf("경로 확인 1: %s, 2: %s, cmp : %d\n", current ->originalpath, path, strcmp(current->originalpath, path));
		if(strcmp(current->originalpath, path) == 0 &&originst.st_size == current->statbuf.st_size && cmpMD5(current->backuppath, path) == 0){
		
        		//printf("경로 동일 확인되었습니다. 1: %s, 2: %s, cmp : %d, cmpMD5 : %d\n", originalMD5, backupMD5, strcmp(originalMD5, backupMD5), cmpMD5(current->backuppath, path));
        		printf("\"%s\" already backuped to \"%s\"\n", path, current->backuppath);
			return -1;
        	}
        	else{
        		//printf("동일한 백업 파일이 아닙니다.\n");
        	}
        	current = current->next;
   	 }
	
	return 0;
}

//옵션 없는 backup 명령어를 실행하는 함수
int BackupBasic(char *path){
	int pathnum, filebytes;
	char *filepath = (char *)malloc(sizeof(char *)*STRMAX);
	char *filename = (char *)malloc(sizeof(char *)*STRMAX);
	char *backuppath = (char *)malloc(sizeof(char *)*STRMAX);
	char *newpath = (char *)malloc(sizeof(char *)*STRMAX);
	char *filecontent = (char *)malloc(sizeof(char *)*STRMAX);
	char *baklogcontent = (char *)malloc(sizeof(char *)*STRMAX);
	
	int originalFD, backupFD, baklogFD;
	char *date = getDate();
	
	struct stat originalstat, backupstat;

	//백업파일이 있나 확인, 있다면 최근에 백업된 파일이 내용이 같은지 확인
	if(checkAlreadyBackuped(path)<0){
		return 0;
	}
	
	//디렉토리경로 구하기
	strcpy(filepath, path);
	for(pathnum=strlen(filepath)-1; filepath[pathnum]!='/';pathnum--);

	strcpy(filename, filepath+pathnum+1);
	filepath[pathnum]='\0';

	sprintf(backuppath,"/home/backup/%s",date);	
	if((originalFD=open(path,O_RDONLY))<0){
		fprintf(stderr, "original open 실패\n");
		exit(1);
	}

	//경로에 파일이 있는지 검사
	if(stat(path, &originalstat)<0){
		fprintf(stderr, "stat 실패\n");
		exit(1);
	}
	
	//버전디렉토리 검사 후 생성
	if(stat(backuppath, &backupstat)<0){
		mkdir(backuppath, 0777);
	}
	
	sprintf(newpath, "%s/%s", backuppath, filename);
	
	//백업파일 O_CREAT 으로 open
	if((backupFD=open(newpath ,O_WRONLY | O_CREAT | O_TRUNC, 777))<0){
		fprintf(stderr, "backup open 실패\n");
		exit(1);
	}

	//원본 파일 read	
	while((filebytes=read(originalFD, filecontent,STRMAX))>0){
		if((write(backupFD, filecontent, filebytes))<0){
			fprintf(stderr,"backupFD write실패\n");
			exit(1);
		};
	}

	printf("\"%s\" backuped to \"%s\"\n", path, newpath);
	
	// log write하기
	sprintf(baklogcontent, "%s : \"%s\" backuped to \"%s\"\n", date, path, newpath);
	if((baklogFD=open(ssubakpath, O_RDWR | O_APPEND, 0777))<0){
		fprintf(stderr, "baklog open 실패\n");
		exit(1);
	}
	if((write(baklogFD, baklogcontent, strlen(baklogcontent)))<0){
		fprintf(stderr, "baklogFD write실패\n");
		exit(1);
	}	
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

//옵션 d로 backup명령어를 실행했을 때 실행되는 함수
int RealBackupD(char *dirpath){
	char *filelist[STRMAX];
	int filecount = 0;
	searchFileInDir(dirpath, filelist, &filecount);
	
	for(int i=0;i<filecount;i++){
		BackupBasic(filelist[i]);
	}

}


//옵션 d, y로 backup명령어를 실행했을 때 실행되는 함수
void backupD(char *dirpath){
	char *filelist[STRMAX];
	int filecount = 0;
	
	int pathnum, filebytes;
	char *filepath = (char *)malloc(sizeof(char *)*STRMAX);
	char *filename = (char *)malloc(sizeof(char *)*STRMAX);
	char *backuppath = (char *)malloc(sizeof(char *)*STRMAX);
	char *newpath = (char *)malloc(sizeof(char *)*STRMAX);
	char *filecontent = (char *)malloc(sizeof(char *)*STRMAX);
	char *baklogcontent = (char *)malloc(sizeof(char *)*STRMAX);
	
	int originalFD, backupFD, baklogFD;
	char *date = getDate();
	
	struct stat originalstat, backupstat;
	
	//디렉토리에서 파일들을 찾음
	searchFileInDir(dirpath, filelist, &filecount);
	
	//파일 돌면서 백업 수행
	for(int i=0;i<filecount;i++){
		strcpy(filepath, filelist[i]);
		for(pathnum=strlen(filepath)-1; filepath[pathnum]!='/';pathnum--);
	
		strcpy(filename, filepath+pathnum+1);
		filepath[pathnum]='\0';

		sprintf(backuppath,"/home/backup/%s",date);	
		if((originalFD=open(filelist[i],O_RDONLY))<0){
			fprintf(stderr, "original open 실패\n");
			exit(1);
		}

		
		if(stat(filelist[i], &originalstat)<0){
			fprintf(stderr, "stat 실패\n");
			exit(1);
	
		}
		
		//버전 디렉토리 생성
		if(stat(backuppath, &backupstat)<0){
			mkdir(backuppath, 0777);
		}
	
		sprintf(newpath, "%s/%s", backuppath, filename);
		
		//백업 파일 생성
		if((backupFD=open(newpath ,O_WRONLY | O_CREAT | O_TRUNC, 777))<0){
			fprintf(stderr, "backup open 실패\n");
			exit(1);
		}

		//원본파일 read 및 write
		while((filebytes=read(originalFD, filecontent,STRMAX))>0){
			if((write(backupFD, filecontent, filebytes))<0){
				fprintf(stderr,"backupFD write실패\n");
				exit(1);
			};
		}
	

		printf("\"%s\" backuped to \"%s\"\n", filelist[i], newpath);
	
		// log file write하기
		sprintf(baklogcontent, "%s : \"%s\" backuped to \"%s\"\n", date, filelist[i], newpath);
		if((baklogFD=open(ssubakpath, O_RDWR | O_APPEND, 0777))<0){
			fprintf(stderr, "baklog open 실패\n");
			exit(1);
		}
		
		if((write(baklogFD, baklogcontent, strlen(baklogcontent)))<0){
			fprintf(stderr, "baklogFD write실패\n");
			exit(1);
		}	
    	}
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

//옵션 r로 backup명령어를 실행했을 때 반복적으로 파일을 백업하는 함수
int RealRecursiveBackup(char *dirpath, char *date, char *backuppath){
	char *filelist[STRMAX];
	char *dirlist[STRMAX];
	int filecount = 0;
	int dircount = 0;
	
	int pathnum, filebytes;
	char *filepath = (char *)malloc(sizeof(char *)*STRMAX);
	char *filename = (char *)malloc(sizeof(char *)*STRMAX);
	char *newpath = (char *)malloc(sizeof(char *)*STRMAX);
	char *filecontent = (char *)malloc(sizeof(char *)*STRMAX);
	char *baklogcontent = (char *)malloc(sizeof(char *)*STRMAX);
	
	int originalFD, backupFD, baklogFD;
	
	struct stat originalstat, backupstat;
	
	//디렉토리 내부의 파일 가져옴
	searchFileInDir(dirpath, filelist, &filecount);
	
	//각 파일들에 대해 백업 진행
	for(int i=0;i<filecount;i++){
		//printf("%s\n", filelist[i]);
	
		if(checkAlreadyBackuped(filelist[i])<0){
			continue;
		}
		strcpy(filepath, filelist[i]);
		
		for(pathnum=strlen(filepath)-1; filepath[pathnum]!='/';pathnum--);
	
		strcpy(filename, filepath+pathnum+1);
		filepath[pathnum]='\0';

		if((originalFD=open(filelist[i],O_RDONLY))<0){
			fprintf(stderr, "original open 실패\n");
			exit(1);
		}

		
		if(stat(filelist[i], &originalstat)<0){
			fprintf(stderr, "stat 실패\n");
			exit(1);
	
		}
		
		//버전 디렉토리가 없다면 생성
		if(stat(backuppath, &backupstat)<0){
			mkdir(backuppath, 0777);
		}
	
		sprintf(newpath, "%s/%s", backuppath, filename);
		
		//백업파일 생성
		if((backupFD=open(newpath ,O_WRONLY | O_CREAT | O_TRUNC, 777))<0){
			fprintf(stderr, "backup open 실패\n");
			exit(1);
		}

		//원본파일 read 후 write
		while((filebytes=read(originalFD, filecontent,STRMAX))>0){
			if((write(backupFD, filecontent, filebytes))<0){
				fprintf(stderr,"backupFD write실패\n");
				exit(1);
			};
		}

		printf("\"%s\" backuped to \"%s\"\n", filelist[i], newpath);
	
		// log file write하기
		sprintf(baklogcontent, "%s : \"%s\" backuped to \"%s\"\n", date, filelist[i], newpath);
		if((baklogFD=open(ssubakpath, O_RDWR | O_APPEND, 0777))<0){
			fprintf(stderr, "baklog open 실패\n");
			exit(1);
		}
		if((write(baklogFD, baklogcontent, strlen(baklogcontent)))<0){
			fprintf(stderr, "baklogFD write실패\n");
			exit(1);
		}	
    	}
    	
 	free(filepath);
	free(filename);
	free(newpath);
	free(filecontent);
	free(baklogcontent);
    	
    	//디렉토리를 검사해 bfslinkedlist에 붙임
    	searchDirInDir(dirpath, dirlist, &dircount);
    	for(int i=0; i<dircount;i++){
    	//백업경로 만들어서 넣어줘야됨
    	    	char newbackuppath[PATHMAX];
    		bfslinkedlist *newbackupnode = malloc(sizeof(bfslinkedlist));
    		strcpy(newbackupnode -> dirpath, dirlist[i]);

 		sprintf(newbackuppath, "%s/%s", backuppath, GetFileName(dirlist[i]));
 		//printf("newbackuppath: %s\n", newbackuppath);

		strcpy(newbackupnode -> backuppath, newbackuppath);
		
		bfslinkedlist *temp = bfshead;
        	while (temp->next != NULL) {
            		temp = temp->next;
       		}
        	temp->next = newbackupnode;
    	}
}
//옵션 r로 backup명령어를 실행했을 때 실행되는 함수
void RealBackupR(char *path){	
	char *backuppath = (char *)malloc(sizeof(char *)*PATHMAX);
	char *date = getDate();

	sprintf(backuppath,"/home/backup/%s",date);	

	bfslinkedlist *newbackupnode = malloc(sizeof(bfslinkedlist));
	strcpy(newbackupnode -> dirpath, path);
	strcpy(newbackupnode -> backuppath, backuppath);
	newbackupnode -> next = bfshead;
	bfshead = newbackupnode;

	//링크드리스트의 끝까지 갈때까지 RealRecursiveBackup 반복
   	 while (bfshead != NULL) {
        	bfslinkedlist *current = bfshead;

        	//printf("bfs test : %s\n", current->dirpath);
        	RealRecursiveBackup(current->dirpath, date, current ->backuppath);

        	bfshead = bfshead->next;
        	free(current);
    	}	
}


//옵션 r, y로 backup명령어를 실행했을 때 반복적으로 파일을 백업하는 함수
void recursiveBackup(char *dirpath, char *date, char *backuppath){
	char *filelist[STRMAX];
	char *dirlist[STRMAX];
	int filecount = 0;
	int dircount = 0;
	
	int pathnum, filebytes;
	char *filepath = (char *)malloc(sizeof(char *)*STRMAX);
	char *filename = (char *)malloc(sizeof(char *)*STRMAX);
	char *newpath = (char *)malloc(sizeof(char *)*STRMAX);
	char *filecontent = (char *)malloc(sizeof(char *)*STRMAX);
	char *baklogcontent = (char *)malloc(sizeof(char *)*STRMAX);
	
	int originalFD, backupFD, baklogFD;
	
	struct stat originalstat, backupstat;
	
	
	searchFileInDir(dirpath, filelist, &filecount);
	
	for(int i=0;i<filecount;i++){
		strcpy(filepath, filelist[i]);
		for(pathnum=strlen(filepath)-1; filepath[pathnum]!='/';pathnum--);
	
		strcpy(filename, filepath+pathnum+1);
		filepath[pathnum]='\0';

		if((originalFD=open(filelist[i],O_RDONLY))<0){
			fprintf(stderr, "original open 실패\n");
			exit(1);
		}
		
		if(stat(filelist[i], &originalstat)<0){
			fprintf(stderr, "stat 실패\n");
			exit(1);
		}
			
		if(stat(backuppath, &backupstat)<0){
			mkdir(backuppath, 0777);
		}
		sprintf(newpath, "%s/%s", backuppath, filename);
	
		if((backupFD=open(newpath ,O_WRONLY | O_CREAT | O_TRUNC, 777))<0){
			fprintf(stderr, "backup open 실패\n");
			exit(1);
		}

		//원본파일 내용을 백업파일에 작성	
		while((filebytes=read(originalFD, filecontent,STRMAX))>0){
			if((write(backupFD, filecontent, filebytes))<0){
				fprintf(stderr,"backupFD write실패\n");
				exit(1);
			};
		}

		printf("\"%s\" backuped to \"%s\"\n", filelist[i], newpath);
	
		// log file write하기
		sprintf(baklogcontent, "%s : \"%s\" backuped to \"%s\"\n", date, filelist[i], newpath);
		if((baklogFD=open(ssubakpath, O_RDWR | O_APPEND, 0777))<0){
			fprintf(stderr, "baklog open 실패\n");
			exit(1);
		}
		if((write(baklogFD, baklogcontent, strlen(baklogcontent)))<0){
			fprintf(stderr, "baklogFD write실패\n");
			exit(1);
		}	
    	}
    	
 	free(filepath);
	free(filename);
	free(newpath);
	free(filecontent);
	free(baklogcontent);
    	
    	//디렉토리 내부에 디렉토리를 가져와 링크드리스트에 붙임
    	searchDirInDir(dirpath, dirlist, &dircount);
    	for(int i=0; i<dircount;i++){
    	    	char newbackuppath[PATHMAX];
    		bfslinkedlist *newbackupnode = malloc(sizeof(bfslinkedlist));
    		strcpy(newbackupnode -> dirpath, dirlist[i]);

 		sprintf(newbackuppath, "%s/%s", backuppath, GetFileName(dirlist[i]));
 		//printf("newbackuppath: %s\n", newbackuppath);

		strcpy(newbackupnode -> backuppath, newbackuppath);
		
		bfslinkedlist *temp = bfshead;
        	while (temp->next != NULL) {
            		temp = temp->next;
       		}
        	temp->next = newbackupnode;
    	}
}



//옵션 r, y로 backup명령어를 실행했을 때 실행되는 함수
void backupR(char *path){	
	char *backuppath = (char *)malloc(sizeof(char *)*PATHMAX);
	char *date = getDate();

	sprintf(backuppath,"/home/backup/%s",date);	

	bfslinkedlist *newbackupnode = malloc(sizeof(bfslinkedlist));
	strcpy(newbackupnode -> dirpath, path);
	strcpy(newbackupnode -> backuppath, backuppath);
	newbackupnode -> next = bfshead;
	bfshead = newbackupnode;

	//bfslinkedlist의 끝에 닿을때까지 recursiveBackup 수행
   	 while (bfshead != NULL) {
        	bfslinkedlist *current = bfshead;

        	recursiveBackup(current->dirpath, date, current ->backuppath);

        	bfshead = bfshead->next;
        	free(current);
    	}	
}

//옵션 y로 backup명령어를 실행했을 때 실행되는 함수
void backupY(char *path){
	int pathnum, filebytes;
	char *filepath = (char *)malloc(sizeof(char *)*STRMAX);
	char *filename = (char *)malloc(sizeof(char *)*STRMAX);
	char *backuppath = (char *)malloc(sizeof(char *)*STRMAX);
	char *newpath = (char *)malloc(sizeof(char *)*STRMAX);
	char *filecontent = (char *)malloc(sizeof(char *)*STRMAX);
	char *baklogcontent = (char *)malloc(sizeof(char *)*STRMAX);
	
	int originalFD, backupFD, baklogFD;
	char *date = getDate();
	
	struct stat originalstat, backupstat;
	
	strcpy(filepath, path);
	for(pathnum=strlen(filepath)-1; filepath[pathnum]!='/';pathnum--);

	strcpy(filename, filepath+pathnum+1);
	filepath[pathnum]='\0';

	sprintf(backuppath,"/home/backup/%s",date);	
	if((originalFD=open(path,O_RDONLY))<0){
		fprintf(stderr, "original open 실패\n");
		exit(1);
	}


	if(stat(path, &originalstat)<0){
		fprintf(stderr, "stat 실패\n");
		exit(1);
	}
	
	
	if(stat(backuppath, &backupstat)<0){
		mkdir(backuppath, 0777);
	}
	else{
		fprintf(stderr,"backuppath디렉토리 존재\n");
		exit(1);
	}
	
	sprintf(newpath, "%s/%s", backuppath, filename);
	
	if((backupFD=open(newpath ,O_WRONLY | O_CREAT | O_TRUNC, 777))<0){
		fprintf(stderr, "backup open fail\n");
		exit(1);
	}

	//원본파일 내용을 읽어 백업파일에 write	
	while((filebytes=read(originalFD, filecontent,STRMAX))>0){
		if((write(backupFD, filecontent, filebytes))<0){
			fprintf(stderr,"backupFD write실패\n");
			exit(1);
		};
	}

	printf("\"%s\" backuped to \"%s\"\n", path, newpath);
	
	// log file 작성
	sprintf(baklogcontent, "%s : \"%s\" backuped to \"%s\"\n", date, path, newpath);
	if((baklogFD=open(ssubakpath, O_RDWR | O_APPEND, 0777))<0){
		fprintf(stderr, "baklog open 실패\n");
		exit(1);
	}
	if((write(baklogFD, baklogcontent, strlen(baklogcontent)))<0){
		fprintf(stderr,  "baklogFD write실패\n");
		exit(1);
	}	

}


//원본 파일의 경로에 대한 백업 파일들의 리스트를 구해주는 함수
void checkSameOriginalPath(char *path, char **backuplist, int *listcount){
	backuplinkedlist *current = head;
	struct stat backupexistST;
	
	//원본 경로와 stat을 비교해 현재 버전디렉토리에 있는 파일 중 원본파일에 해당하는 파일 담음
	while (current != NULL) {       
		//printf("pathcheck : %d\n", stat(current->backuppath, &backupexistST));
        	if(strcmp(current->originalpath, path) == 0&& stat(current->backuppath, &backupexistST)>=0){
        		backuplist[(*listcount)++] = current -> backuppath;
        		//printf("리스트 추가\n");
        	}
        	current = current->next;
   	 }
	
}

//절대경로를 받아 버전 디렉토리명만 리턴하는 함수
char *versionDirName(char *path){
    	char *dirname;
  	int count = 0;

	//3번째 슬래시에 있는 getDate로 가져온 디렉토리명 추출
  	  dirname = strtok(path, "/");
  	  while (dirname != NULL) {
   	  	count++;
   		if (count == 3) {
     	       	return dirname;
        	}
       		dirname = strtok(NULL, "/");
   	}
   	
   	return NULL;
}


//디렉토리 경로를 받아 디렉토리가 비어있는지 확인하고 비어있으면 삭제하는 함수
void checkDirEmpty(char *path){
	int count = 0;
	struct dirent *dentry;
	DIR *dir;
	//디렉토리경로 추출
	char *dirpath = GetDirPath(path);
	if((dir = opendir(dirpath))==NULL){
		fprintf(stderr, "opendir 실패\n");
		exit(1);
	}
    	while((dentry = readdir(dir)) != NULL){
    		if(++count>2){
    			break;
    		}
    	}
    	closedir(dir);
    	
	//.과 ..외에 아무것도 없으면 디렉토리 삭제
    	if(count<=2){
    		remove(dirpath);
    	}  	
}

//디렉토리 경로를 받아 재귀적으로 내부의 디렉토리가 비어있는지 모두 확인하고 비어있으면 삭제하는 함수
void deleteEmptyDirectories(char *path) {
    	char *filelist[STRMAX]; 
    	int filecount = 0;
    	int count = 0;
    	struct dirent *dentry;
	DIR *dir;

	//디렉토리들을 재귀적으로 탐색(DFS)
    	searchDirInDir(path, filelist, &filecount);
    	for (int i = 0; i < filecount; i++) {
    	    	deleteEmptyDirectories(filelist[i]);
    	}
    	if((dir = opendir(path))==NULL){
		//fprintf(stderr, "opendir 실패\n");
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

//오름차순 정렬을 위한 문자열 비교 함수
int comparepath(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

//옵션 없는 remove 명령어를 실행하는 함수
int RemoveBasic(char *path){
	char *backuplist[PATHMAX];
	char *backuppath = (char *)malloc(sizeof(char)*PATHMAX);
	char *removenum = (char *)malloc(sizeof(char)*STRMAX);
	int listcount = 0;

	//같은 원본경로를 가진 백업파일 리스트에 담음
	checkSameOriginalPath(path, backuplist, &listcount);

	if(listcount == 0){
		fprintf(stderr, "경로 \"%s\"에 대한 백업 파일이 없습니다\n", path);
		return 0;
	}
	//백업파일이 하나일 경우
	else if(listcount == 1){
		int removeindexnum = 0;
		char *removepath = backuplist[removeindexnum];
		//파일삭제
		if(remove(removepath)<0){
			fprintf(stderr, "remove error index : %d\n", removeindexnum);
			exit(1);
		}
		else{
			//로그 작성
			printf("\"%s\" removed by \"%s\"\n", removepath, path);
			char *newremovepath = strdup(removepath);
			char *date = versionDirName(newremovepath);

			int baklogFD;
			char *baklogcontent = (char *)malloc(sizeof(char *)*STRMAX);
		
			sprintf(baklogcontent, "%s : \"%s\" removed by \"%s\"\n", date, removepath, path);
			if((baklogFD=open(ssubakpath, O_RDWR | O_APPEND, 0777))<0){
				fprintf(stderr, "baklog open 실패\n");
				exit(1);
			}
			if((write(baklogFD, baklogcontent, strlen(baklogcontent)))<0){
				fprintf(stderr, "baklogFD write실패");
				exit(1);
			}
		}
		//빈 디렉토리일 경우 삭제
		checkDirEmpty(removepath);
		return 0;
	}
	printf("backup files of %s\n", path);
	//오름차순으로 배열하기
    	qsort(backuplist, listcount, sizeof(char *), comparepath);

	//백업파일들 오름차순으로 출력
	printf("0. exit\n");
	for(int i=0; i<listcount; i++){
		int fd = open(backuplist[i], O_RDONLY);
		strcpy(backuppath, backuplist[i]);
		//printf("%d. %s    %sbytes\n%s\n", i, versionDirName(backuppath), lseek(fd, 0, SEEK_END), backuplist[i]);
	 	int filenum = lseek(fd, 0, SEEK_END);
		
		printf("%d. %s      %sbytes\n", i+1, versionDirName(backuppath), cvtNumComma(filenum));
	}

	//숫자 입력받음
	printf(">> ");
	fgets(removenum, sizeof(removenum), stdin);
	removenum[strlen(removenum) - 1] = '\0';

	if(strcmp(removenum, "0")==0){
		exit(0);
	}
  	//printf("%s", removenum);
	int removeindexnum = atoi(removenum) -1;
	char *removepath = backuplist[removeindexnum];
	if (removeindexnum < 0 || removeindexnum >= listcount) {
 		fprintf(stderr, "index: %d\n", removeindexnum);
		exit(1);
	}

	//선택된 파일 삭제
	if(remove(removepath)<0){
		fprintf(stderr, "remove error index : %d\n", removeindexnum);
		exit(1);
	}
	else{
		printf("\"%s\" removed by \"%s\"\n", removepath, path);
		char *newremovepath = strdup(removepath);
		char *date = versionDirName(newremovepath);

		int baklogFD;
		char *baklogcontent = (char *)malloc(sizeof(char *)*STRMAX);
		
		//로그 작성
		sprintf(baklogcontent, "%s : \"%s\" removed by \"%s\"\n", date, removepath, path);
		if((baklogFD=open(ssubakpath, O_RDWR | O_APPEND, 0777))<0){
			fprintf(stderr, "baklog open 실패\n");
			exit(1);
		}
		if((write(baklogFD, baklogcontent, strlen(baklogcontent)))<0){
			fprintf(stderr, "baklogFD write실패\n");
			exit(1);
		}
	}

	//디렉토리가 비어있는지 확인 후 디렉토리 삭제
	checkDirEmpty(removepath);
	return 0;
}

//옵션 d로 remove 명령어를 실행했을 때 실행되는 함수
int RemoveD(char *dirpath){
	char *filelist[STRMAX];
	int filecount = 0;
	//디렉토리 내부의 파일들 삭제
	searchFileInDir(dirpath, filelist, &filecount);
	
	for(int i=0;i<filecount;i++){
		RemoveBasic(filelist[i]);
	}
	//빈 디렉토리 있는지 검사
	deleteEmptyDirectories(backuppath);

}


//옵션 r로 remove명령어를 실행했을 때 반복적으로 파일을 remove하는 함수
void recursiveRemove(char *dirpath){
	char *filelist[STRMAX];
	char *dirlist[STRMAX];
	int filecount = 0;
	int dircount = 0;
	
	//파일은 삭제
	searchFileInDir(dirpath, filelist, &filecount);
	for(int i=0;i<filecount;i++){
		RemoveBasic(filelist[i]);
	}
	
    	//디렉토리는 링크드리스트에 붙임
    	searchDirInDir(dirpath, dirlist, &dircount);
    	for(int i=0; i<dircount;i++){
    	    	char newbackuppath[PATHMAX];
    		bfslinkedlist *newbackupnode = malloc(sizeof(bfslinkedlist));
    		strcpy(newbackupnode -> dirpath, dirlist[i]);
		
		bfslinkedlist *temp = bfshead;
        	while (temp->next != NULL) {
            		temp = temp->next;
       		}
        	temp->next = newbackupnode;
    	}
}

//옵션 r로 remove 명령어를 실행했을 때 실행되는 함수
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
    	deleteEmptyDirectories(backuppath);

}

//옵션 a로 remove 명령어를 실행했을 때 실행되는 함수
int RemoveA(char *path){
	char *backuplist[PATHMAX];
	char *backuppath = (char *)malloc(sizeof(char)*PATHMAX);
	int listcount = 0;

	checkSameOriginalPath(path, backuplist, &listcount);

	if(listcount == 0){
		fprintf(stderr, "경로 \"%s\"에 대한 백업 파일이 없습니다\n", path);
		return 1;
	}
	
	for(int i=0; i<listcount; i++){
		char *removepath=backuplist[i];
	
		if(remove(removepath)<0){
			fprintf(stderr, "remove error index : %d\n", i);
			exit(1);
		}
		else{
			printf("\"%s\" removed by \"%s\"\n", removepath, path);
			char *newremovepath = strdup(removepath);
			char *date = versionDirName(newremovepath);
	
			int baklogFD;
			char *baklogcontent = (char *)malloc(sizeof(char *)*STRMAX);
		
			sprintf(baklogcontent, "%s : \"%s\" removed by \"%s\"\n", date, removepath, path);
			if((baklogFD=open(ssubakpath, O_RDWR | O_APPEND, 0777))<0){
				fprintf(stderr, "baklog open 실패\n");
				exit(1);
			}
			if((write(baklogFD, baklogcontent, strlen(baklogcontent)))<0){
				fprintf(stderr, "baklogFD write실패\n");
				exit(1);
			}
		}
		
		//디렉토리가 비어있는지 확인 후 디렉토리 삭제
		checkDirEmpty(removepath);
	}
	deleteEmptyDirectories(backuppath);
	return 0;
}



//옵션 d, a로 remove 명령어를 실행했을 때 실행되는 함수
int RemoveDA(char *dirpath){
	char *filelist[STRMAX];
	int filecount = 0;
	
	//디렉토리의 파일을 돌며 RemoveA 실행
	searchFileInDir(dirpath, filelist, &filecount);
	
	for(int i=0;i<filecount;i++){
		RemoveA(filelist[i]);
	}
	//빈 디렉토리 삭제
	deleteEmptyDirectories(backuppath);

}

//옵션 r, a로 remove명령어를 실행했을 때 반복적으로 파일을 remove하는 함수
void recursiveRemoveRA(char *dirpath){
	char *filelist[STRMAX];
	char *dirlist[STRMAX];
	int filecount = 0;
	int dircount = 0;
	
	//디렉토리 내부 파일을 돌며 RemoveA 실행
	searchFileInDir(dirpath, filelist, &filecount);
	for(int i=0;i<filecount;i++){
		RemoveA(filelist[i]);
	}
	
    	//디렉토리 내부의 디렉토리를 링크드리스트에 붙임
    	searchDirInDir(dirpath, dirlist, &dircount);
    	for(int i=0; i<dircount;i++){
    	    	char newbackuppath[PATHMAX];
    		bfslinkedlist *newbackupnode = malloc(sizeof(bfslinkedlist));
    		strcpy(newbackupnode -> dirpath, dirlist[i]);
		
		bfslinkedlist *temp = bfshead;
        	while (temp->next != NULL) {
            		temp = temp->next;
       		}
        	temp->next = newbackupnode;
    	}
}

//옵션 r, a로 remove 명령어를 실행했을 때 실행되는 함수
void RemoveRA(char *path){	

	bfslinkedlist *newbackupnode = malloc(sizeof(bfslinkedlist));
	strcpy(newbackupnode -> dirpath, path);
	newbackupnode -> next = bfshead;
	bfshead = newbackupnode;

	//링크드리스트를 돌며 recursiveRemoveRA실행
   	 while (bfshead != NULL) {
        	bfslinkedlist *current = bfshead;
        	recursiveRemoveRA(current->dirpath);

        	bfshead = bfshead->next;
        	free(current);
    	}
    	//빈 디렉토리 삭제
    	deleteEmptyDirectories(backuppath);	
}



//옵션 없는 recover 명령어를 실행하는 함수
int RecoverBasic(char *path){
	char *backuplist[PATHMAX];
	char *backuppath = (char *)malloc(sizeof(char)*PATHMAX);
	char *removenum = (char *)malloc(sizeof(char)*STRMAX);
	int listcount = 0;

	//파일 복구 후 백업파일 삭제용 변수
	int originalFD, backupFD, filebytes;
	struct stat originalstat, backupstat;
	char filecontent[STRMAX];
	
	//원본파일경로에서 백업한 파일 리스트 가져오기
	checkSameOriginalPath(path, backuplist, &listcount);

	if(listcount == 0){
		fprintf(stderr, "경로 \"%s\"에 대한 백업 파일이 없습니다\n", path);
		return 0;
	}
	else if(listcount == 1){
		int removeindexnum = 0;
		char *removepath = backuplist[removeindexnum];
		if (removeindexnum < 0 || removeindexnum >= listcount) {
 			fprintf(stderr, "index: %d\n", removeindexnum);
			exit(1);
		}
		
		if(stat(path, &originalstat)>=0){
			//md5로 동일한 내용인지 확인
			if(cmpMD5(removepath, path)==0){
				printf("\"%s\" not changed with \"%s\"\n", removepath, path);
				return 0;
			}
		}

		//복구할 파일 덮어씌워 만들기
		if((originalFD=open(path,O_WRONLY | O_CREAT | O_TRUNC, 0666))<0){
			fprintf(stderr, "original open 실패\n");
			exit(1);
		}
		if((fchmod(originalFD,  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH))<0){
			fprintf(stderr, "original chmod 실패\n");
			exit(1);
		}
		if(stat(removepath, &backupstat)<0){
			fprintf(stderr, "backup stat 실패\n");
			exit(1);
		}
		
		//백업파일 내용 읽고 원본파일 경로에 복구
		if((backupFD=open(removepath ,O_RDONLY))<0){
			fprintf(stderr, "backup open실패\n");
			exit(1);
		}
		while((filebytes=read(backupFD, filecontent, STRMAX))>0){
			if((write(originalFD, filecontent, filebytes))<0){
				fprintf(stderr, "backupFD write실패\n");
				exit(1);
			};
		}

		//백업파일 삭제
		if(remove(removepath)<0){
			fprintf(stderr, "remove error index : %d\n", removeindexnum);
			exit(1);
		}
		
		printf("\"%s\" recovered to \"%s\"\n", removepath, path);
	
		char *newremovepath = strdup(removepath);
		char *date = versionDirName(newremovepath);
		int baklogFD;
		char *baklogcontent = (char *)malloc(sizeof(char *)*STRMAX);
	
		// 로그파일 작성
		sprintf(baklogcontent, "%s : \"%s\" recovered to \"%s\"\n", date, removepath, path);
		if((baklogFD=open(ssubakpath, O_RDWR | O_APPEND, 0777))<0){
			fprintf(stderr, "baklog open 실패\n");
			exit(1);
		}
		if((write(baklogFD, baklogcontent, strlen(baklogcontent)))<0){
			fprintf(stderr, "baklogFD write실패\n");
			exit(1);
		}

		//디렉토리가 비어있는지 확인 후 디렉토리 삭제
		checkDirEmpty(removepath);
		close(originalFD);
		
		return 0;
	}
	
	printf("backup files of %s\n", path);
	//오름차순으로 배열하기
	qsort(backuplist, listcount, sizeof(char*), comparepath);

	printf("0. exit\n");
	for(int i=0; i<listcount; i++){
		int fd = open(backuplist[i], O_RDONLY);
		strcpy(backuppath, backuplist[i]);
		//printf("%d. %s    %sbytes\n%s\n", i, versionDirName(backuppath), lseek(fd, 0, SEEK_END), backuplist[i]);
	 	int filenum = lseek(fd, 0, SEEK_END);
		
		printf("%d. %s      %sbytes\n", i+1, versionDirName(backuppath), cvtNumComma(filenum));
	}

	//숫자 입력받음
	printf(">> ");
	fgets(removenum, sizeof(removenum), stdin);
	removenum[strlen(removenum) - 1] = '\0';

	if(strcmp(removenum, "0")==0){
		exit(0);
	}
	int removeindexnum = atoi(removenum) -1;
	char *removepath = backuplist[removeindexnum];
	if (removeindexnum < 0 || removeindexnum >= listcount) {
 		fprintf(stderr, "index: %d\n", removeindexnum);
		exit(1);
	}
	
	if(stat(path, &originalstat)>=0){
		//md5로 동일한 내용인지 확인
		if(cmpMD5(removepath, path)==0){
			printf("\"%s\" not changed with \"%s\"\n", removepath, path);
			return 0;
		}
	}

	//파일 복구 후 백업파일 삭제
	if((originalFD=open(path, O_WRONLY | O_CREAT | O_TRUNC, 0666))<0){
		fprintf(stderr, "original open 실패\n");
		exit(1);
	}

	if((fchmod(originalFD,  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH))<0){
		fprintf(stderr, "original chmod 실패\n");
		exit(1);
	}
	
	if(stat(removepath, &backupstat)<0){
		fprintf(stderr, "backup stat 실패\n");
		exit(1);
	}
	
	//원본파일 복구
	if((backupFD=open(removepath ,O_RDONLY))<0){
		fprintf(stderr, "backup open 실패\n");
		exit(1);
	}
	while((filebytes=read(backupFD, filecontent, STRMAX))>0){
		if((write(originalFD, filecontent, filebytes))<0){
			fprintf(stderr, "backupFD write실패\n");
			exit(1);
		};
	}
	
	//백업파일 삭제
	if(remove(removepath)<0){
		fprintf(stderr, "remove error index : %d\n", removeindexnum);
		exit(1);
	}
	printf("\"%s\" recovered to \"%s\"\n", removepath, path);
	
	char *newremovepath = strdup(removepath);
	char *date = versionDirName(newremovepath);
	int baklogFD;
	char *baklogcontent = (char *)malloc(sizeof(char *)*STRMAX);
	
	// 로그파일 작성
	sprintf(baklogcontent, "%s : \"%s\" recovered to \"%s\"\n", date, removepath, path);
	if((baklogFD=open(ssubakpath, O_RDWR | O_APPEND, 0777))<0){
		fprintf(stderr, "baklog open 실패\n");
		exit(1);
	}
	if((write(baklogFD, baklogcontent, strlen(baklogcontent)))<0){
		fprintf(stderr, "baklogFD write실패\n");
		exit(1);
	}

//디렉토리가 비어있는지 확인 후 디렉토리 삭제
	checkDirEmpty(removepath);
	close(originalFD);
	
	return 0;
}

//옵션 d로 recover 명령어를 실행했을 때 실행되는 함수
int RecoverD(char *dirpath){
	char *filelist[STRMAX];
	int filecount = 0;
	searchFileInDir(dirpath, filelist, &filecount);
	
	for(int i=0;i<filecount;i++){
		RecoverBasic(filelist[i]);
	}
	deleteEmptyDirectories(backuppath);

}

//옵션 r로 recover명령어를 실행했을 때 반복적으로 파일을 recover하는 함수
void recursiveRecover(char *dirpath){
	char *filelist[STRMAX];
	char *dirlist[STRMAX];
	int filecount = 0;
	int dircount = 0;
	
	//디렉토리 내부의 파일들 복구
	searchFileInDir(dirpath, filelist, &filecount);
	for(int i=0;i<filecount;i++){
		RecoverBasic(filelist[i]);
	}
	
    	//디렉토리 내의 디렉토리 링크드리스트에 붙이기
    	searchDirInDir(dirpath, dirlist, &dircount);
    	for(int i=0; i<dircount;i++){
    	    	char newbackuppath[PATHMAX];
    		bfslinkedlist *newbackupnode = malloc(sizeof(bfslinkedlist));
    		strcpy(newbackupnode -> dirpath, dirlist[i]);
		
		bfslinkedlist *temp = bfshead;
        	while (temp->next != NULL) {
            		temp = temp->next;
       		}
        	temp->next = newbackupnode;
    	}
}

//옵션 r로 recover 명령어를 실행했을 때 실행되는 함수
void RecoverR(char *path){	
	bfslinkedlist *newbackupnode = malloc(sizeof(bfslinkedlist));
	strcpy(newbackupnode -> dirpath, path);
	newbackupnode -> next = bfshead;
	bfshead = newbackupnode;

	//링크드리스트 끝에 도달할때까지 recursiveRecover반복
   	 while (bfshead != NULL) {
        	bfslinkedlist *current = bfshead;
        	recursiveRecover(current->dirpath);

        	bfshead = bfshead->next;
        	free(current);
    	}
    	deleteEmptyDirectories(backuppath);	
}

//옵션 l로 recover 명령어를 실행했을 때 실행되는 함수
int RecoverL(char *path){
	char *backuplist[PATHMAX];
	char *backuppath = (char *)malloc(sizeof(char)*PATHMAX);
	char *removenum = (char *)malloc(sizeof(char)*STRMAX);
	int listcount = 0;

	//파일 복구 후 백업파일 삭제용 변수
	int originalFD, backupFD, filebytes;
	struct stat originalstat, backupstat;
	char filecontent[STRMAX];

	checkSameOriginalPath(path, backuplist, &listcount);

	if(listcount == 0){
		fprintf(stderr, "경로 \"%s\"에 대한 백업 파일이 없습니다\n", path);
		exit(1);
	}
	char *removepath = backuplist[0];
	
	//md5로 동일한 내용인지 확인
	if(cmpMD5(removepath, path)==0){
		printf("\"%s\" not changed with \"%s\"\n", removepath, path);
		return 0;
	}

	//파일 복구 후 백업파일 삭제
	if((originalFD=open(path,O_WRONLY | O_CREAT | O_TRUNC, 0777))<0){
		fprintf(stderr, "original open 실패\n");
		exit(1);
	}

	
	if(stat(removepath, &backupstat)<0){
		fprintf(stderr, "backup stat 실패\n");
		exit(1);
	
	}
	
	if(stat(path, &originalstat)<0){
		fprintf(stderr, "original stat 실패\n");
		exit(1);
	}
	
	//원본파일 복구
	if((backupFD=open(removepath ,O_RDONLY))<0){
		fprintf(stderr, "backup open 실패\n");
		exit(1);
	}
	while((filebytes=read(backupFD, filecontent, STRMAX))>0){
		if((write(originalFD, filecontent, filebytes))<0){
			fprintf(stderr, "backupFD write실패\n");
			exit(1);
		};
	}

	
	if(remove(removepath)<0){
		fprintf(stderr, "remove error\n");
		exit(1);
	}
	
	printf("\"%s\" recovered to \"%s\"\n", removepath, path);
	
	char *newremovepath = strdup(removepath);
	char *date = versionDirName(newremovepath);
	int baklogFD;
	char *baklogcontent = (char *)malloc(sizeof(char *)*STRMAX);
	
	// 로그파일 작성
	sprintf(baklogcontent, "%s : \"%s\" recovered to \"%s\"\n", date, removepath, path);
	if((baklogFD=open(ssubakpath, O_RDWR | O_APPEND, 0777))<0){
		fprintf(stderr, "baklog open 실패\n");
		exit(1);
	}
	if((write(baklogFD, baklogcontent, strlen(baklogcontent)))<0){
		fprintf(stderr, "baklogFD write실패\n");
		exit(1);
	}
	
	//디렉토리 비었는지 확인 후 삭제
	checkDirEmpty(removepath);
	close(originalFD);

	return 0;
}

//옵션 n으로 recover 명령어를 실행했을 때 실행되는 함수
int RecoverN(char *path, char *newpath){
	char *backuplist[PATHMAX];
	char *backuppath = (char *)malloc(sizeof(char)*PATHMAX);
	char *removenum = (char *)malloc(sizeof(char)*STRMAX);
	int listcount = 0;

	//파일 복구 후 백업파일 삭제용 변수
	int originalFD, backupFD, filebytes;
	struct stat originalstat, backupstat;
	char filecontent[STRMAX];
	
	//원본경로에서 백업한 백업파일들 리스트에 담아 가져오기
	checkSameOriginalPath(path, backuplist, &listcount);

	if(listcount == 0){
		fprintf(stderr, "경로 \"%s\"에 대한 백업 파일이 없습니다\n", path);
		return 0;
	}
	else if(listcount == 1){
		int removeindexnum = 0;
		char *removepath = backuplist[removeindexnum];
		if (removeindexnum < 0 || removeindexnum >= listcount) {
 			fprintf(stderr, "index: %d\n", removeindexnum);
			exit(1);
		}
		
		if((access(GetDirPath(newpath), F_OK))!=0){
			mkdir(newpath, 0777);
		}
	
		if((originalFD=open(newpath,O_WRONLY | O_CREAT | O_TRUNC, 0666))<0){
			fprintf(stderr, "original open 실패\n");
			exit(1);
		}
		
		if((fchmod(originalFD,  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH))<0){
			fprintf(stderr, "original chmod 실패\n");
			exit(1);
		}
	
		if(stat(removepath, &backupstat)<0){
			fprintf(stderr, "backup stat 실패\n");
			exit(1);
		}
		
		//원본파일 복구
		if((backupFD=open(removepath ,O_RDONLY))<0){
			fprintf(stderr, "backup open실패\n");
			exit(1);
		}
		
		while((filebytes=read(backupFD, filecontent, STRMAX))>0){
			if((write(originalFD, filecontent, filebytes))<0){
				fprintf(stderr, "backupFD write실패\n");
				exit(1);
			};
		}

		if(remove(removepath)<0){
			fprintf(stderr, "remove error index : %d\n", removeindexnum);
			exit(1);
		}
		
		printf("\"%s\" recovered to \"%s\"\n", removepath, newpath);
	
		char *newremovepath = strdup(removepath);
		char *date = versionDirName(newremovepath);
		int baklogFD;
		char *baklogcontent = (char *)malloc(sizeof(char *)*STRMAX);
	
		// 로그파일 작성, date 가져와야됨
		sprintf(baklogcontent, "%s : \"%s\" recovered to \"%s\"\n", date, removepath, newpath);
		if((baklogFD=open(ssubakpath, O_RDWR | O_APPEND, 0777))<0){
			fprintf(stderr, "baklog open 실패\n");
			exit(1);
		}
		if((write(baklogFD, baklogcontent, strlen(baklogcontent)))<0){
			fprintf(stderr, "baklogFD write실패\n");
			exit(1);
		}

		//디렉토리가 비어있는지 확인 후 디렉토리 삭제
		checkDirEmpty(removepath);
		close(originalFD);
		
		return 0;
	}
	
	printf("backup files of %s\n", path);
	//오름차순으로 배열하기
	qsort(backuplist, listcount, sizeof(char*), comparepath);

	printf("0. exit\n");
	for(int i=0; i<listcount; i++){
		int fd = open(backuplist[i], O_RDONLY);
		strcpy(backuppath, backuplist[i]);
	 	int filenum = lseek(fd, 0, SEEK_END);
		
		printf("%d. %s      %sbytes\n", i+1, versionDirName(backuppath), cvtNumComma(filenum));
	}

//숫자 입력받음
	printf(">> ");
	fgets(removenum, sizeof(removenum), stdin);
	removenum[strlen(removenum) - 1] = '\0';

	if(strcmp(removenum, "0")==0){
		exit(0);
	}
	int removeindexnum = atoi(removenum) -1;
	char *removepath = backuplist[removeindexnum];
	if (removeindexnum < 0 || removeindexnum >= listcount) {
 		fprintf(stderr, "index: %d\n", removeindexnum);
		exit(1);
	}
	
	if((access(GetDirPath(newpath), F_OK))!=0){
		mkdir(GetDirPath(newpath), 0777);
	}

	//파일 복구 후 백업파일 삭제
	if((originalFD=open(newpath,O_WRONLY | O_CREAT | O_TRUNC, 0666))<0){
		fprintf(stderr, "original open 실패\n");
		exit(1);
	}
	
		if((fchmod(originalFD,  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH))<0){
		fprintf(stderr, "original chmod 실패\n");
		exit(1);
	}

	
	if(stat(removepath, &backupstat)<0){
		fprintf(stderr, "backup stat 실패\n");
		exit(1);
	
	}
	
	if((backupFD=open(removepath ,O_RDONLY))<0){
		fprintf(stderr, "backup open 실패\n");
		exit(1);
	}
	
	while((filebytes=read(backupFD, filecontent, STRMAX))>0){
		if((write(originalFD, filecontent, filebytes))<0){
			fprintf(stderr, "backupFD write실패\n");
			exit(1);
		};
	}

	
	if(remove(removepath)<0){
		fprintf(stderr, "remove error index : %d\n", removeindexnum);
		exit(1);
	}
	
	printf("\"%s\" recovered to \"%s\"\n", removepath, newpath);
	
	char *newremovepath = strdup(removepath);
	char *date = versionDirName(newremovepath);
	int baklogFD;
	char *baklogcontent = (char *)malloc(sizeof(char *)*STRMAX);
	
	// 로그파일 작성, date 가져와야됨
	sprintf(baklogcontent, "%s : \"%s\" recovered to \"%s\"\n", date, removepath, newpath);
	if((baklogFD=open(ssubakpath, O_RDWR | O_APPEND, 0777))<0){
		fprintf(stderr, "baklog open 실패\n");
		exit(1);
	}
	if((write(baklogFD, baklogcontent, strlen(baklogcontent)))<0){
		fprintf(stderr, "baklogFD write실패\n");
		exit(1);
	}

//디렉토리가 비어있는지 확인 후 디렉토리 삭제
	checkDirEmpty(removepath);
	close(originalFD);
	
	return 0;
}

//옵션 n으로 옵션 d, r과 같이 디렉토리 경로를 받았을 때 recover명령어를 수행하는 함수
int RecoverDirN(char *path, char *newpath){
	char *backuplist[PATHMAX];
	char *backuppath = (char *)malloc(sizeof(char)*PATHMAX);
	char *removenum = (char *)malloc(sizeof(char)*STRMAX);
	int listcount = 0;

	//파일 복구 후 백업파일 삭제용 변수
	int originalFD, backupFD, filebytes;
	struct stat originalstat, backupstat;
	char filecontent[STRMAX];
	checkSameOriginalPath(path, backuplist, &listcount);

	if(listcount == 0){
		fprintf(stderr, "경로 \"%s\"에 대한 백업 파일이 없습니다\n", path);
		return 0;
	}
	else if(listcount == 1){
		int removeindexnum = 0;
		char *removepath = backuplist[removeindexnum];
		if (removeindexnum < 0 || removeindexnum >= listcount) {
 			fprintf(stderr, "index: %d\n", removeindexnum);
			exit(1);
		}

		if((stat(newpath, &originalstat))<0){
			mkdir(newpath, 0777);
		}
		chmod(newpath, 0777);
	
		char newpathfile[PATHMAX];
		sprintf(newpathfile, "%s/%s", newpath, GetFileName(path));
	
		//파일 복구 후 백업파일 삭제
		if((originalFD=open(newpathfile,O_WRONLY | O_CREAT | O_TRUNC, 0666))<0){
			fprintf(stderr, "original open 실패\n");
			exit(1);
		}
		
		if((fchmod(originalFD,  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH))<0){
			fprintf(stderr, "original chmod 실패\n");
			exit(1);
		}
	
		if(stat(removepath, &backupstat)<0){
			fprintf(stderr, "backup stat 실패\n");
			exit(1);
		}
		

		
		if((backupFD=open(removepath ,O_RDONLY))<0){
			fprintf(stderr, "backup open실패\n");
			exit(1);
		}
		
		while((filebytes=read(backupFD, filecontent, STRMAX))>0){
			if((write(originalFD, filecontent, filebytes))<0){
				fprintf(stderr, "backupFD write실패\n");
				exit(1);
			};
		}

		//백업파일 삭제
		if(remove(removepath)<0){
			fprintf(stderr, "remove error index : %d\n", removeindexnum);
			exit(1);
		}
		
		printf("\"%s\" recovered to \"%s\"\n", removepath, newpath);
	
		char *newremovepath = strdup(removepath);
		char *date = versionDirName(newremovepath);
		int baklogFD;
		char *baklogcontent = (char *)malloc(sizeof(char *)*STRMAX);
	
		// 로그파일 작성
		sprintf(baklogcontent, "%s : \"%s\" recovered to \"%s\"\n", date, removepath, newpath);
		if((baklogFD=open(ssubakpath, O_RDWR | O_APPEND, 0777))<0){
			fprintf(stderr, "baklog open 실패\n");
			exit(1);
		}
		if((write(baklogFD, baklogcontent, strlen(baklogcontent)))<0){
			fprintf(stderr, "baklogFD write실패\n");
			exit(1);
		}

		//디렉토리가 비어있는지 확인 후 디렉토리 삭제
		checkDirEmpty(removepath);
		close(originalFD);
		
		return 0;
	}
	
	printf("backup files of %s\n", path);
	//오름차순으로 배열하기
	qsort(backuplist, listcount, sizeof(char*), comparepath);

	printf("0. exit\n");
	for(int i=0; i<listcount; i++){
		int fd = open(backuplist[i], O_RDONLY);
		strcpy(backuppath, backuplist[i]);
	 	int filenum = lseek(fd, 0, SEEK_END);
		
		printf("%d. %s      %sbytes\n", i+1, versionDirName(backuppath), cvtNumComma(filenum));
	}

	//숫자 입력받음
	printf(">> ");
	fgets(removenum, sizeof(removenum), stdin);
	removenum[strlen(removenum) - 1] = '\0';

	if(strcmp(removenum, "0")==0){
		exit(0);
	}
	int removeindexnum = atoi(removenum) -1;
	char *removepath = backuplist[removeindexnum];
	if (removeindexnum < 0 || removeindexnum >= listcount) {
 		fprintf(stderr, "index: %d\n", removeindexnum);
		exit(1);
	}
	
	if((stat(newpath, &originalstat))<0){
		mkdir(newpath, 0777);
	}
	chmod(newpath, 0777);
	
	char newpathfile[PATHMAX];
	sprintf(newpathfile, "%s/%s", newpath, GetFileName(path));
	
	//파일 복구 후 백업파일 삭제
	if((originalFD=open(newpathfile,O_WRONLY | O_CREAT | O_TRUNC, 0666))<0){
		fprintf(stderr, "original open 실패\n");
		exit(1);
	}
	
		if((fchmod(originalFD,  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH))<0){
		fprintf(stderr, "original chmod 실패\n");
		exit(1);
	}

	
	if(stat(removepath, &backupstat)<0){
		fprintf(stderr, "backup stat 실패\n");
		exit(1);
	
	}
	
	//원본파일 새로 open 후 백업파일 내용 write
	if((backupFD=open(removepath ,O_RDONLY))<0){
		fprintf(stderr, "backup open 실패\n");
		exit(1);
	}
	
	while((filebytes=read(backupFD, filecontent, STRMAX))>0){
		if((write(originalFD, filecontent, filebytes))<0){
			fprintf(stderr, "backupFD write실패\n");
			exit(1);
		};
	}

	//백업파일 삭제
	if(remove(removepath)<0){
		fprintf(stderr, "remove error index : %d\n", removeindexnum);
		exit(1);
	}
	
	printf("\"%s\" recovered to \"%s\"\n", removepath, newpathfile);
	
	char *newremovepath = strdup(removepath);
	char *date = versionDirName(newremovepath);
	int baklogFD;
	char *baklogcontent = (char *)malloc(sizeof(char *)*STRMAX);
	
	// 로그파일 작성
	sprintf(baklogcontent, "%s : \"%s\" recovered to \"%s\"\n", date, removepath, newpathfile);
	if((baklogFD=open(ssubakpath, O_RDWR | O_APPEND, 0777))<0){
		fprintf(stderr, "baklog open 실패\n");
		exit(1);
	}
	if((write(baklogFD, baklogcontent, strlen(baklogcontent)))<0){
		fprintf(stderr, "baklogFD write실패\n");
		exit(1);
	}

	//디렉토리가 비어있는지 확인 후 디렉토리 삭제
	checkDirEmpty(removepath);
	close(originalFD);
	
	return 0;
}

//옵션 d, l로 recover 명령어를 실행했을 때 실행되는 함수
int RecoverDL(char *dirpath){
	char *filelist[STRMAX];
	int filecount = 0;
	//디렉토리 내부의 파일에 대해 RecoverL 실행
	searchFileInDir(dirpath, filelist, &filecount);
	
	for(int i=0;i<filecount;i++){
		RecoverL(filelist[i]);
	}
	//빈 디렉토리 삭제
	deleteEmptyDirectories(backuppath);

}

//옵션 d, n으로 recover 명령어를 실행했을 때 실행되는 함수
int RecoverDN(char *dirpath, char *newpath){
	char *filelist[STRMAX];
	int filecount = 0;
	//디렉토리 내부의 파일에 대해 RecoverN 실행
	searchFileInDir(dirpath, filelist, &filecount);
	
	for(int i=0;i<filecount;i++){
		RecoverDirN(filelist[i], newpath);
	}
	//빈 디렉토리 삭제
	deleteEmptyDirectories(backuppath);

}

//옵션 r, n으로 recover명령어를 실행했을 때 반복적으로 파일을 recover하는 함수
void recursiveRecoverRN(char *dirpath, char *newpath){
	char *filelist[STRMAX];
	char *dirlist[STRMAX];
	int filecount = 0;
	int dircount = 0;
	
	//디렉토리 내부의 파일에 대해 RecoverN 실행
	searchFileInDir(dirpath, filelist, &filecount);
	for(int i=0;i<filecount;i++){
		RecoverN(filelist[i], newpath);
	}
	
    	//디렉토리 내부의 디렉토리를 노드로 링크드리스트에 붙임
    	searchDirInDir(dirpath, dirlist, &dircount);
    	for(int i=0; i<dircount;i++){
    	    	char newbackuppath[PATHMAX];
    		bfslinkedlist *newbackupnode = malloc(sizeof(bfslinkedlist));
    		strcpy(newbackupnode -> dirpath, dirlist[i]);
		
		bfslinkedlist *temp = bfshead;
        	while (temp->next != NULL) {
            		temp = temp->next;
       		}
        	temp->next = newbackupnode;
    	}
}

//옵션 r, n으로 recover 명령어를 실행했을 때 실행되는 함수
void RecoverRN(char *path, char *newpath){	

	bfslinkedlist *newbackupnode = malloc(sizeof(bfslinkedlist));
	strcpy(newbackupnode -> dirpath, path);
	newbackupnode -> next = bfshead;
	bfshead = newbackupnode;

	//링크드리스트 끝에 도달할 때 까지 recursiveRecoverRN반복
   	 while (bfshead != NULL) {
        	bfslinkedlist *current = bfshead;
        	recursiveRecoverRN(current->dirpath, newpath);

        	bfshead = bfshead->next;
        	free(current);
    	}	
    	//빈 디렉토리 삭제
    	deleteEmptyDirectories(backuppath);
}

//옵션 r, l로 recover명령어를 실행했을 때 반복적으로 파일을 recover하는 함수
void recursiveRecoverRL(char *dirpath){
	char *filelist[STRMAX];
	char *dirlist[STRMAX];
	int filecount = 0;
	int dircount = 0;
	
	//디렉토리 내부의 파일에 대해 RecoverL 실행
	searchFileInDir(dirpath, filelist, &filecount);
	for(int i=0;i<filecount;i++){
		RecoverL(filelist[i]);
	}
	
    	//디렉토리 내부의 디렉토리를 노드로 링크드리스트에 붙임
    	searchDirInDir(dirpath, dirlist, &dircount);
    	for(int i=0; i<dircount;i++){
    	    	char newbackuppath[PATHMAX];
    		bfslinkedlist *newbackupnode = malloc(sizeof(bfslinkedlist));
    		strcpy(newbackupnode -> dirpath, dirlist[i]);
		
		bfslinkedlist *temp = bfshead;
        	while (temp->next != NULL) {
            		temp = temp->next;
       		}
        	temp->next = newbackupnode;
    	}
    	
}


//옵션 r, l로 recover 명령어를 실행했을 때 실행되는 함수
void RecoverRL(char *path){	

	bfslinkedlist *newbackupnode = malloc(sizeof(bfslinkedlist));
	strcpy(newbackupnode -> dirpath, path);
	newbackupnode -> next = bfshead;
	bfshead = newbackupnode;

	//링크드리스트 끝에 도달할 때 까지 recursiveRecoverRL반복
   	 while (bfshead != NULL) {
        	bfslinkedlist *current = bfshead;

        	recursiveRecoverRL(current->dirpath);

        	bfshead = bfshead->next;
        	free(current);
    	}
    	deleteEmptyDirectories(backuppath);	
}





//인자로 받은 정보로 링크드리스트를 생성하는 함수
void makelinkedlist(char *firstpath, char *secondpath, char *command){
	struct stat statbuf;
	//링크드리스트 노드 생성
	if(strcmp(command, " backuped to ")==0){
		backuplinkedlist *newbackupnode = malloc(sizeof(backuplinkedlist));
		strcpy(newbackupnode -> originalpath, firstpath);
		strcpy(newbackupnode -> backuppath, secondpath);
        	if (stat(firstpath, &statbuf) <0) {
        	}
        	newbackupnode->statbuf = statbuf;
		newbackupnode -> next = head;
		head = newbackupnode;
	}
	else if(strcmp(command, " removed by ")==0){
		//링크드리스트 노드 삭제
		backuplinkedlist *prev = NULL;
		backuplinkedlist *current = head;
	    	while (current != NULL) {
        		if(strcmp(firstpath, current->backuppath)==0 && strcmp(secondpath, current->originalpath)==0 &&access(firstpath, F_OK) <0){
				//printf("찾음 : Original Path: %s, Backup Path: %s, inode: %ld\n", current->originalpath, current->backuppath, current->statbuf.st_ino);
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
	else if(strcmp(command, " recovered to ")==0){
		//링크드리스트 노드 삭제
		backuplinkedlist *prev = NULL;
		backuplinkedlist *current = head;
	    	while (current != NULL) {
        		if(strcmp(firstpath, current->backuppath)==0 && strcmp(secondpath, current->originalpath)==0 &&access(firstpath, F_OK) <0){
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
void makelinkedlistinfo(){
	char line[PATHMAX*2];
	FILE *file = fopen(ssubakpath, "r");
	
	while((fgets(line, PATHMAX*2, file))!=NULL){
		strtok(line, "\""); 
        	char *firstpath = strtok(NULL, "\"");
        	char *command = strtok(NULL, "\"");
        	char *secondpath = strtok(NULL, "\"");

		makelinkedlist(firstpath, secondpath, command);
	
	}
}

//인자로 받은 명령어와 옵션 별로 함수를 호출하고 경로 문자열을 넣어주는 함수
int main(int argc, char* argv[]){
	char *path = (char *)malloc(sizeof(char *)*STRMAX);
	struct stat backupst;
	struct stat pathcheckst;
	
	if(argc<2){
		fprintf(stderr, "ERROR: wrong input.\n%s help : show commands for program\n", argv[0]);
		exit(1);
	}
	if(strcmp(argv[1], "backup")!=0 && strcmp(argv[1], "remove")!=0 && strcmp(argv[1], "recover")!=0 && strcmp(argv[1], "list")!=0 && strcmp(argv[1], "help")!=0){
   		fprintf(stderr,"ERROR: invalid command -- \'%s\'\n%s help : show commands for program\n", argv[1], argv[0]);
   		exit(1);
   	}
   	
   	//help 명령어에 관한 분기 처리
   	if(!strcmp(argv[1],"help")){
		if (argv[2] == NULL) {
			printf("Usage:");
			help();
			exit(0);
		}
       		else {
           		// 옵션이 있는경우
            		if (!strcmp(argv[2], "backup")) {
            			printf("Usage:");
				backuphelp();
				exit(0);
            		}
            		else if (!strcmp(argv[2], "remove")) {
            			printf("Usage:");
				removehelp();
				exit(0);
           		}	
            		else if (!strcmp(argv[2], "recover")) {
            			printf("Usage:");
				recoverhelp();
				exit(0);
           		}
            		else if (!strcmp(argv[2], "list")) {
            			printf("Usage:");
           		   	listhelp();
           		   	exit(0);
           		}
            		else if (!strcmp(argv[2], "help")) {
            			printf("Usage:");
				helphelp();
				exit(0);
           		}
           		else{
           			fprintf(stderr, "잘못된 옵션입니다.\n");
           			exit(1);
           		}
        	}
		
	}

	//인자개수 검사
	if(argc<3){
		fprintf(stderr, "ERROR: missing operand <PATH>\n");
		exit(1);
	}
   	
   	//길이제한을 넘거나 경로가 올바르지 않은경우 
	 if(strlen(argv[2])>PATHMAX){
		fprintf(stderr, "입력한 경로가 길이제한을 넘었습니다.\n");
		exit(1);
	}
	
	//입력받은 경로를 절대경로로 변환
	realpath(argv[2], path);
	if(strcmp(argv[1],"remove")!=0 && strcmp(argv[1],"recover")!=0){
		//일반 파일이나 디렉토리가 아니거나 접근 권한이 없는경우
		if(stat(path, &pathcheckst)<0){
			fprintf(stderr, "ERROR: \'%s\' is not exist.\n", path);
			exit(1);
		}
	}
	
	//인자로 입력받은 경로가 홈 디렉토리를 벗어날 경우
	char *homedir = "/home";
	if(strncmp(path, homedir, strlen(homedir))!=0){
		fprintf(stderr, "ERROR: path must be in user directory.\n - \'%s\' is not in user directory\n", path);
		exit(1);
	}
	
	//경로가 백업 디렉토리일 경우
	if(strncmp(path, backuppath, strlen(backuppath))==0){
		fprintf(stderr, "ERROR: path must be in user directory.\n - \'%s\' is not in user directory\n", path);
		exit(1);
	}
	
   	//백업 디렉토리 없으면 생성
	if((access(backuppath, F_OK))!=0){
		mkdir("/home/backup", 0777);
	}

	//로그파일 없으면 생성
	int fd = open(ssubakpath, O_CREAT | O_EXCL | O_RDWR, 0777);
	if (fd<0) {
        	//printf("이미 log 파일이 있습니다.\n");
    	}
	else{
        	close(fd);
    	}
	
	//로그를 읽어 링크드리스트 생성
	makelinkedlistinfo();
	
	backuplinkedlist *current = head;
	int ct = 1;
	/*printf("전체 링크드리스트 \n");
    	while (current != NULL) {
        	printf("%d. Original Path: %s, Backup Path: %s, inode: %ld\n", ct++, current->originalpath, current->backuppath, current->statbuf.st_ino);
        	current = current->next;
   	 }
   	 printf("\n");
   	 */

	
        // argv[2]의 개행 문자 없애기
	if (argv[2] != NULL) {
        argv[2][strcspn(argv[2], "\n")] = '\0';
        }
    	else{
		if(!strcmp(argv[1],"backup")){
			printf("Usage:");
			backuphelp();
			exit(1);
		}
		else if (!strcmp(argv[1], "remove")) {
            		printf("Usage:");
			removehelp();
			exit(1);
           	}	
            	else if (!strcmp(argv[1], "recover")) {
            		printf("Usage:");
			recoverhelp();
			exit(1);
           	}
   	}
   	
   
	//명령어 및 옵션 분기문
	int optcount;
	if(!strcmp(argv[1],"backup")){
		int doption=0, roption=0, yoption=0;
		if(!(S_ISDIR(pathcheckst.st_mode)) && !(S_ISREG(pathcheckst.st_mode))){
			fprintf(stderr, "일반 파일이나 디렉토리가 아닙니다.\n");
			exit(1);
		}

		while ((optcount = getopt(argc, argv, "dry")) != -1) {
            		switch (optcount) {
                		case 'd':
                    			doption = 1;
                    			break;
                		case 'r':
                    			roption = 1;
                    			break;
               			case 'y':
                   			yoption = 1;
                    			break;
                		default:
                		        printf("Usage:");
					backuphelp();
                   			exit(0);
            		}
       		}
		
		// 옵션이 없는경우
		if (!doption && !roption && !yoption) {
			//-d나 r을 사용하지 않았는데 디렉토리 경로를 받은경우
			if(S_ISDIR(pathcheckst.st_mode)){
				fprintf(stderr, "ERROR: \'%s\' is directory.\n- use \'-d\'option or input file path.\n", path);
				exit(1);
			}
            		BackupBasic(path);
       		} 
       		else {
       		        // 옵션이 있는경우
       		    	if (doption && roption && yoption) {
        			        if(S_ISREG(pathcheckst.st_mode)){
						fprintf(stderr, "파일 경로 대신 디렉토리 경로를 입력해야 합니다.\n");
						exit(1);
					}
             		 	backupR(path);
        			
        			exit(0);

    			} 
   		 	if (doption && roption) {
       			 	        if(S_ISREG(pathcheckst.st_mode)){
						fprintf(stderr, "파일 경로 대신 디렉토리 경로를 입력해야 합니다.\n");
						exit(1);
					}
             		 	RealBackupR(path);
       			 	exit(0);

    			} 
    			if (doption && yoption) {
        			        if(S_ISREG(pathcheckst.st_mode)){
						fprintf(stderr, "파일 경로 대신 디렉토리 경로를 입력해야 합니다.\n");
						exit(1);
					}
       				backupD(path);
        			exit(0);

    			} 
    			if (roption && yoption) {
        			        if(S_ISREG(pathcheckst.st_mode)){
						fprintf(stderr, "파일 경로 대신 디렉토리 경로를 입력해야 합니다.\n");
					exit(1);
				}
             		 	backupR(path);
       			 	exit(0);

    			}
       		
       		
            		if (doption) {
            		        //-d나 r을 사용하였는데 파일 경로를 받은경우
            		       	if(S_ISREG(pathcheckst.st_mode)){
					fprintf(stderr, "파일 경로 대신 디렉토리 경로를 입력해야 합니다.\n");
					exit(1);
				}
       				RealBackupD(path);
			}
            		if (roption) {
            		       //-d나 r을 사용하였는데 파일 경로를 받은경우
            		       	if(S_ISREG(pathcheckst.st_mode)){
					fprintf(stderr, "파일 경로 대신 디렉토리 경로를 입력해야 합니다.\n");
					exit(1);
				}
             		 	//backupR(path);
             		 	RealBackupR(path);
           		}	
           		if (yoption) {
    				 //-d나 r을 사용하지 않았는데 디렉토리 경로를 받은경우
    				if(S_ISDIR(pathcheckst.st_mode)){
					fprintf(stderr, "디렉토리 경로 대신 파일 경로를 입력해야 합니다.\n");
					exit(1);
				}
       				backupY(path);
			}
        	}
	}
	else if(!strcmp(argv[1],"remove")){
		int doption=0, roption=0, aoption=0;
		
		while ((optcount = getopt(argc, argv, "dra")) != -1) {
            		switch (optcount) {
                		case 'd':
                    			doption = 1;
                    			break;
                		case 'r':
                    			roption = 1;
                    			break;
               			case 'a':
                   			aoption = 1;
                    			break;
                		default:
                		        printf("Usage:");
                    			removehelp();
                   			exit(1);
            		}
       		}
		// 옵션이 없는경우
		if (!doption && !roption && !aoption) {
			//-d나 r을 사용하지 않았는데 디렉토리 경로를 받은경우
			if(S_ISDIR(pathcheckst.st_mode)){
				fprintf(stderr, "디렉토리 경로 대신 파일 경로를 입력해야 합니다.\n");
				exit(1);
			}
            		RemoveBasic(path);
       		} 
       		else {
           		// 옵션이 있는경우
           		if (doption && roption && aoption) {
           		        //-d나 r을 사용하였는데 파일 경로를 받은경우
            		        if(S_ISREG(pathcheckst.st_mode)){
					fprintf(stderr, "파일 경로 대신 디렉토리 경로를 입력해야 합니다.\n");
					exit(1);
				}
				RemoveRA(path);
       			 	exit(0);
    			} 
   		 	if (doption && roption) {
       			 	printf("Backup with -d, -r options: %s\n", path);
       			 	        if(S_ISREG(pathcheckst.st_mode)){
						fprintf(stderr, "파일 경로 대신 디렉토리 경로를 입력해야 합니다.\n");
						exit(1);
					}
             		 	RemoveR(path);
       			 	exit(0);
    			} 
    			if (doption && aoption) {
            			//-d나 r을 사용하였는데 파일 경로를 받은경우
            			 if(S_ISREG(pathcheckst.st_mode)){
					fprintf(stderr, "파일 경로 대신 디렉토리 경로를 입력해야 합니다.\n");
					exit(1);
				}
				RemoveDA(path);
       			 	exit(0);
    			} 
    			if (roption && aoption) {
           		        //-d나 r을 사용하였는데 파일 경로를 받은경우
            		        if(S_ISREG(pathcheckst.st_mode)){
					fprintf(stderr, "파일 경로 대신 디렉토리 경로를 입력해야 합니다.\n");
					exit(1);
				}
				RemoveRA(path);
       			 	exit(0);
    			}
           		
           		
            		if (doption) {
            			//-d나 r을 사용하였는데 파일 경로를 받은경우
            			 if(S_ISREG(pathcheckst.st_mode)){
					fprintf(stderr, "파일 경로 대신 디렉토리 경로를 입력해야 합니다.\n");
					exit(1);
				}
				RemoveD(path);
            		}
            		if (roption) {
            		        //-d나 r을 사용하였는데 파일 경로를 받은경우
            		        if(S_ISREG(pathcheckst.st_mode)){
					fprintf(stderr, "파일 경로 대신 디렉토리 경로를 입력해야 합니다.\n");
					exit(1);
				}
             		 	RemoveR(path);
           		}	
           		if (aoption) {
           			//-d나 r을 사용하지 않았는데 디렉토리 경로를 받은경우
           			if(S_ISDIR(pathcheckst.st_mode)){
					fprintf(stderr, "디렉토리 경로 대신 파일 경로를 입력해야 합니다.\n");
					exit(1);
				}
           			RemoveA(path);
			}
        	}
		
	}
	else if(!strcmp(argv[1],"recover")){
		int doption=0, roption=0, loption=0, noption=0;
		
		while ((optcount = getopt(argc, argv, "drln")) != -1) {
            		switch (optcount) {
                		case 'd':
                    			doption = 1;
                    			break;
                		case 'r':
                    			roption = 1;
                    			break;
               			case 'l':
                   			loption = 1;
                    			break;
                    		case 'n':
                   			noption = 1;
                    			break;
                		default:
                		        printf("Usage:");
                    			recoverhelp();
                   			exit(0);
            		}
       		}
		
		// 옵션이 없는경우
		if (!doption && !roption && !loption&& !noption) {
		//-d나 r을 사용하지 않았는데 디렉토리 경로를 받은경우
			if(S_ISDIR(pathcheckst.st_mode)){
				fprintf(stderr, "디렉토리 경로 대신 파일 경로를 입력해야 합니다.\n");
				exit(1);
			}
			RecoverBasic(path);
       		} 
       		else {
       		
       		 	if (doption && roption && loption && noption ) {
            		        //-d나 r을 사용하였는데 파일 경로를 받은경우
            		        if(S_ISREG(pathcheckst.st_mode)){
					fprintf(stderr, "파일 경로 대신 디렉토리 경로를 입력해야 합니다.\n");
					exit(1);
				}
				if(argc<6){
					fprintf(stderr,"recover할 새로운 경로를 지정해야합니다.\n");
					exit(1);
				}
				char *newpath = (char *)malloc(sizeof(char *)*STRMAX);
				realpath(argv[5], newpath);
				struct stat newpathcheckst;
				
				if(stat(path, &newpathcheckst)<0){
					fprintf(stderr, "ERROR: \'%s\' is not exist.\n", path);
					exit(1);
				}
           			if(S_ISREG(newpathcheckst.st_mode)){
					fprintf(stderr, "디렉토리 경로 대신 파일 경로를 입력해야 합니다.\n");	
					exit(1);
				}
				RecoverDN(path, newpath);
       			 	exit(0);
    			}
    			else if (roption && loption && noption) {		
            		        //-d나 r을 사용하였는데 파일 경로를 받은경우
            		        if(S_ISREG(pathcheckst.st_mode)){
					fprintf(stderr, "파일 경로 대신 디렉토리 경로를 입력해야 합니다.\n");
					exit(1);
				}
				if(argc<6){
					fprintf(stderr,"recover할 새로운 경로를 지정해야합니다.\n");
					exit(1);
				}
				char *newpath = (char *)malloc(sizeof(char *)*STRMAX);
				realpath(argv[5], newpath);
				struct stat newpathcheckst;
				
				if(stat(path, &newpathcheckst)<0){
					fprintf(stderr, "ERROR: \'%s\' is not exist.\n", path);
					exit(1);
				}
           			if(S_ISREG(newpathcheckst.st_mode)){
					fprintf(stderr, "디렉토리 경로 대신 파일 경로를 입력해야 합니다.\n");	
					exit(1);
				}
				RecoverDN(path, newpath);
       			 	exit(0);
    			} 
    			else if (doption && roption && noption) {
            		        //-d나 r을 사용하였는데 파일 경로를 받은경우
            		        if(S_ISREG(pathcheckst.st_mode)){
					fprintf(stderr, "파일 경로 대신 디렉토리 경로를 입력해야 합니다.\n");
					exit(1);
				}
				if(argc<6){
					fprintf(stderr,"recover할 새로운 경로를 지정해야합니다.\n");
					exit(1);
				}
				char *newpath = (char *)malloc(sizeof(char *)*STRMAX);
				realpath(argv[5], newpath);
				struct stat newpathcheckst;
				
				if(stat(path, &newpathcheckst)<0){
					fprintf(stderr, "ERROR: \'%s\' is not exist.\n", path);
					exit(1);
				}
           			if(S_ISREG(newpathcheckst.st_mode)){
					fprintf(stderr, "디렉토리 경로 대신 파일 경로를 입력해야 합니다.\n");	
					exit(1);
				}
				RecoverDN(path, newpath);
       			 	exit(0);
    			} 
    			else if (doption && roption && loption) {
            		        //-d나 r을 사용하였는데 파일 경로를 받은경우
            		        if(S_ISREG(pathcheckst.st_mode)){
					fprintf(stderr, "파일 경로 대신 디렉토리 경로를 입력해야 합니다.\n");
					exit(1);
				}
             		 	RecoverRL(path);
       			 	exit(0);
    			} 
    			else if (doption && loption && noption) {
        			printf("Backup with -y option: %s\n", path);
       			 	exit(0);
    			}
    			if (doption && roption) {
            		        //-d나 r을 사용하였는데 파일 경로를 받은경우
            		        if(S_ISREG(pathcheckst.st_mode)){
					fprintf(stderr, "파일 경로 대신 디렉토리 경로를 입력해야 합니다.\n");
					exit(1);
				}
             		 	RecoverR(path);
       			 	exit(0);
    			}
    			if (doption &&loption) {
            		        //-d나 r을 사용하였는데 파일 경로를 받은경우
            		        if(S_ISREG(pathcheckst.st_mode)){
					fprintf(stderr, "파일 경로 대신 디렉토리 경로를 입력해야 합니다.\n");
					exit(1);
				}
				RecoverDL(path);
       			 	exit(0);
    			}
    			if (doption && noption) {
            		        //-d나 r을 사용하였는데 파일 경로를 받은경우
            		        if(S_ISREG(pathcheckst.st_mode)){
					fprintf(stderr, "파일 경로 대신 디렉토리 경로를 입력해야 합니다.\n");
					exit(1);
				}
				if(argc<6){
					fprintf(stderr,"recover할 새로운 경로를 지정해야합니다.\n");
					exit(1);
				}
				char *newpath = (char *)malloc(sizeof(char *)*STRMAX);
				realpath(argv[5], newpath);
				struct stat newpathcheckst;
				
				if(stat(path, &newpathcheckst)<0){
					fprintf(stderr, "ERROR: \'%s\' is not exist.\n", path);
					exit(1);
				}
           			if(S_ISREG(newpathcheckst.st_mode)){
					fprintf(stderr, "디렉토리 경로 대신 파일 경로를 입력해야 합니다.\n");	
					exit(1);
				}
				RecoverDN(path, newpath);
       			 	exit(0);
    			}
    			if (roption && loption) {
            		        //-d나 r을 사용하였는데 파일 경로를 받은경우
            		        if(S_ISREG(pathcheckst.st_mode)){
					fprintf(stderr, "파일 경로 대신 디렉토리 경로를 입력해야 합니다.\n");
					exit(1);
				}
             		 	RecoverRL(path);
       			 	exit(0);
    			}
    			if (roption && noption ) {
 //-d나 r을 사용하였는데 파일 경로를 받은경우
            		        if(S_ISREG(pathcheckst.st_mode)){
					fprintf(stderr, "파일 경로 대신 디렉토리 경로를 입력해야 합니다.\n");
					exit(1);
				}
				if(argc<6){
					fprintf(stderr,"recover할 새로운 경로를 지정해야합니다.\n");
					exit(1);
				}
				char *newpath = (char *)malloc(sizeof(char *)*STRMAX);
				realpath(argv[5], newpath);
				struct stat newpathcheckst;
				
				if(stat(path, &newpathcheckst)<0){
					fprintf(stderr, "ERROR: \'%s\' is not exist.\n", path);
					exit(1);
				}
           			if(S_ISREG(newpathcheckst.st_mode)){
					fprintf(stderr, "디렉토리 경로 대신 파일 경로를 입력해야 합니다.\n");	
					exit(1);
				}
				RecoverDN(path, newpath);
       			 	exit(0);
    			}
    			if (loption && noption ) {
           			//-d나 r을 사용하지 않았는데 디렉토리 경로를 받은경우
           			if(S_ISDIR(pathcheckst.st_mode)){
					fprintf(stderr, "디렉토리 경로 대신 파일 경로를 입력해야 합니다.\n");	
					exit(1);
				}
				if(argc<5){
					fprintf(stderr,"recover할 새로운 경로를 지정해야합니다.\n");
					exit(1);
				}
				char *newpath = (char *)malloc(sizeof(char *)*STRMAX);
				realpath(argv[4], newpath);
				struct stat newpathcheckst;
				
				if(stat(path, &newpathcheckst)<0){
					fprintf(stderr, "ERROR: \'%s\' is not exist.\n", path);
					exit(1);
				}
           			if(S_ISDIR(newpathcheckst.st_mode)){
					fprintf(stderr, "디렉토리 경로 대신 파일 경로를 입력해야 합니다.\n");	
					exit(1);
				}
				RecoverN(path, newpath);
    			
    			
       			 	exit(0);
    			}
           		// 옵션이 있는경우
            		if (doption) {
            		        //-d나 r을 사용하였는데 파일 경로를 받은경우
            		        if(S_ISREG(pathcheckst.st_mode)){
					fprintf(stderr, "파일 경로 대신 디렉토리 경로를 입력해야 합니다.\n");
					exit(1);
				}
				RecoverD(path);
            		}
            		if (roption) {
            		        //-d나 r을 사용하였는데 파일 경로를 받은경우
            		        if(S_ISREG(pathcheckst.st_mode)){
					fprintf(stderr, "파일 경로 대신 디렉토리 경로를 입력해야 합니다.\n");
					exit(1);
				}
             		 	RecoverR(path);
           		}	
           		if (loption) {
           			//-d나 r을 사용하지 않았는데 디렉토리 경로를 받은경우
           			if(S_ISDIR(pathcheckst.st_mode)){
					fprintf(stderr, "디렉토리 경로 대신 파일 경로를 입력해야 합니다.\n");
					exit(1);
				}
				RecoverL(path);
			}
           		if (noption) {

           			//-d나 r을 사용하지 않았는데 디렉토리 경로를 받은경우
           			if(S_ISDIR(pathcheckst.st_mode)){
					fprintf(stderr, "디렉토리 경로 대신 파일 경로를 입력해야 합니다.\n");	
					exit(1);
				}
				if(argc<5){
					fprintf(stderr,"recover할 새로운 경로를 지정해야합니다.\n");
					exit(1);
				}
				char *newpath = (char *)malloc(sizeof(char *)*STRMAX);
				realpath(argv[4], newpath);
				struct stat newpathcheckst;
				
				if(stat(path, &newpathcheckst)<0){
					fprintf(stderr, "ERROR: \'%s\' is not exist.\n", path);
					exit(1);
				}
           			if(S_ISDIR(newpathcheckst.st_mode)){
					fprintf(stderr, "디렉토리 경로 대신 파일 경로를 입력해야 합니다.\n");	
					exit(1);
				}
				RecoverN(path, newpath);
           		}
        	}
		
	}
	else if(!strcmp(argv[1],"list")){
		printf("list command\n");
	}
	else{
		fprintf(stderr, "올바른 명령어가 아닙니다.\n");
		exit(1);
	}	
}


