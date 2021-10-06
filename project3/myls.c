#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

char type(mode_t);
char* perm(mode_t);
void readStat(char*, char*, struct stat*); //stat 읽어오기
void readStat_a(char*, char*, struct stat*); //-a 옵션 stat 읽어오기
void printStat(void); //정렬 후 출력
void printStat_i(void);
void printStat_l(void);
void printStat_t(void);
void sortName(void);
void sortTime(void);
void printStat_file(char *, struct stat*);
void printStat_file_a(char *, struct stat*);
int count = 0;

typedef struct _Buf{
	char name[60];
	char ctype;
	char sperm[10];
	int nlink;
	char uid[20];
	char gid[20];
	int ssize;
	char mtime[30];
	time_t ttime;
	int inode;
}Buf;
Buf buf_file[50];
Buf buf_tmp[50];

int main(int argc, char** argv)
{ 
	DIR *dp;
	char *dir;
	struct stat st;
	struct dirent *d;
	char path[1024];
	int flag_i = 0, flag_l = 0, flag_t = 0, flag_a = 0;
	int opt; //option
	if(argc == 1) dir = "."; //실행파일명만 있음 => 현재 directory는 "."
	else{
		while((opt = getopt(argc, argv, "ilta")) != -1) {
			switch(opt){
				case 'i' : flag_i = 1; break;
				case 'l' : flag_l = 1; break;
				case 't' : flag_t = 1; break;
				case 'a' : flag_a = 1; break;
				case '?' : printf("error : unknown option, %c\n", 
								   opt);
						   exit(1);
			}
		}
		if (flag_i == 0 && flag_l == 0 && flag_t == 0 && flag_a == 0)
			dir = argv[1];
		else{
			if(argc == 2) dir = ".";
			else if (argc == 3) dir = argv[2];
		}
	}
	if ((dp = opendir(dir)) == NULL){//filename
		struct stat st_b;
		if(lstat(dir, &st_b) < 0){
			perror(path);
		}
		if(argc == 1){
			printf("%s\n", dir);
		}
		else if (argc == 2 && flag_i + flag_l + flag_t + flag_a == 0){
			printf("%s\n", dir);
		}
		if (flag_i){
			printf("%ld %s\n", st_b.st_ino, dir);
		}
		else if (flag_l){
			printStat_file(dir, &st_b);
		}
		else if (flag_t){
			printf("%s\n", dir);
		}
		else if(flag_a){
			printStat_file_a(dir, &st_b);
		}
		exit(0);
	}
	while ((d = readdir(dp)) != NULL){ //디렉토리의 각 파일에 대해 readdir값이 null이 될 때까지 읽어라
			sprintf(path, "%s/%s", dir, d->d_name); //파일경로명 만들기
		if (lstat(path, &st) < 0) //파일 상태 정보 가져오기
			perror(path);
		/*기본(옵션x)*/
		if(argc ==1){
			readStat(path, d->d_name, &st);
		}
		if(argc ==2 && flag_i + flag_l + flag_t + flag_a == 0){
			readStat(path, d->d_name, &st);
		}
		/*-i 옵션 구현*/
		if(flag_i){
			readStat(path, d->d_name, &st);
		}
		/*-l 옵션 구현*/
		if(flag_l){
			readStat(path, d->d_name, &st);
		}
		/*-t 옵션 구현*/
		if(flag_t){
			readStat(path, d->d_name, &st);
		}
		if(flag_a){
			readStat_a(path, d->d_name, &st);
		}
	}
	if ( (argc ==1) || (argc ==2 && flag_i + flag_l + flag_t + flag_a == 0)){
		sortName();
		printStat();
	}
	if (flag_i){
		sortName();
		printStat_i();
	}
	else if (flag_l){
		sortName();
		printStat_l();
	}
	else if (flag_a){
		sortName();
		printStat();
	}
	else if (flag_t){
		sortTime();
		printStat_t();
	}
	else ;
	printf("\n");
	closedir(dp);
	exit(0);
}

void sortName(){
	for(int p=0; p<count; p++){
		for(int q = 0; q<count-(p+1); q++){
			if(strcmp(buf_file[q].name, buf_file[q+1].name) >0)
			{
				buf_tmp[q] = buf_file[q];
				buf_file[q] = buf_file[q+1];
				buf_file[q+1] = buf_tmp[q];
			}
		}
	}
}

void sortTime(){
	for(int p=0; p<count; p++){
		for(int q = 0; q<count-(p+1); q++){
			if(buf_file[q].ttime < buf_file[q+1].ttime)
			{
				buf_tmp[q] = buf_file[q];
				buf_file[q] = buf_file[q+1];
				buf_file[q+1] = buf_tmp[q];
			}
		}
	}
}

void printStat(){
	for(int j = 0; j<count; j++){
		printf("%-40s", buf_file[j].name);
		if(j%2 == 1) 
			printf("\n");
	}
}

void printStat_i(){
	for(int j = 0; j<count; j++){
		printf("%8d %-40s", buf_file[j].inode, buf_file[j].name);
		if(j%2 == 1)
			printf("\n");
	}
}

void printStat_l(){
	for(int j = 0; j<count; j++){
		printf("%c%s ", buf_file[j].ctype, buf_file[j].sperm);
		printf("%3d ", buf_file[j].nlink);
		printf("%s %s ", buf_file[j].uid, buf_file[j].gid);
		printf("%10d ", buf_file[j].ssize);
		printf("%.12s ", buf_file[j].mtime);
		printf("%s\n", buf_file[j].name);
	}
}

void printStat_file(char* file, struct stat * st){
	char* str = ".";
	char* str2 = "..";
	char* strr = "root";
	if((strcmp(file, str) != 0) && (strcmp(file, str2) != 0) &&
			(strcmp((getpwuid(st->st_uid)->pw_name), strr) != 0) &&
			(file[0] != '.'))
	{
		int l = strlen(file);
		memset(buf_file[0].name, 0, 60);
		buf_file[0].ctype = '\0';
		memset(buf_file[0].sperm, 0, 10);
		buf_file[0].nlink = 0;
		memset(buf_file[0].uid, 0, 20);
		memset(buf_file[0].gid, 0, 20);
		buf_file[0].ssize = 0;
		memset(buf_file[0].mtime, 0, 30);
		buf_file[0].inode = 0;
		strncpy(buf_file[0].name, file, l);
		buf_file[0].ctype = type(st->st_mode);
		strncpy(buf_file[0].sperm, perm(st->st_mode), 10);
		buf_file[0].nlink = st->st_nlink;
		strncpy(buf_file[0].uid, getpwuid(st->st_uid)->pw_name, 20);
		strncpy(buf_file[0].gid, getgrgid(st->st_gid)->gr_name, 20);
		buf_file[0].ssize = st->st_size;
		strncpy(buf_file[0].mtime, ctime(&st->st_mtime)+4, 30);
		buf_file[0].inode = st->st_ino;
		buf_file[0].ttime = st->st_mtime;
	}
	printf("%c%s ", buf_file[0].ctype, buf_file[0].sperm);
	printf("%3d ", buf_file[0].nlink);
	printf("%s %s ", buf_file[0].uid, buf_file[0].gid);
	printf("%10d ", buf_file[0].ssize);
	printf("%.12s ", buf_file[0].mtime);
	printf("%s\n", buf_file[0].name);
}

void printStat_file_a(char* file, struct stat * st){
	char* strr = "root";
	if(strcmp((getpwuid(st->st_uid)->pw_name), strr) != 0)
	{
		int l = strlen(file);
		memset(buf_file[0].name, 0, 60);
		buf_file[0].ctype = '\0';
		memset(buf_file[0].sperm, 0, 10);
		buf_file[0].nlink = 0;
		memset(buf_file[0].uid, 0, 20);
		memset(buf_file[0].gid, 0, 20);
		buf_file[0].ssize = 0;
		memset(buf_file[0].mtime, 0, 30);
		buf_file[0].inode = 0;
		strncpy(buf_file[0].name, file, l);
		buf_file[0].ctype = type(st->st_mode);
		strncpy(buf_file[0].sperm, perm(st->st_mode), 10);
		buf_file[0].nlink = st->st_nlink;
		strncpy(buf_file[0].uid, getpwuid(st->st_uid)->pw_name, 20);
		strncpy(buf_file[0].gid, getgrgid(st->st_gid)->gr_name, 20);
		buf_file[0].ssize = st->st_size;
		strncpy(buf_file[0].mtime, ctime(&st->st_mtime)+4, 30);
		buf_file[0].inode = st->st_ino;
		buf_file[0].ttime = st->st_mtime;
	}
	printf("%c%s ", buf_file[0].ctype, buf_file[0].sperm);
	printf("%3d ", buf_file[0].nlink);
	printf("%s %s ", buf_file[0].uid, buf_file[0].gid);
	printf("%10d ", buf_file[0].ssize);
	printf("%.12s ", buf_file[0].mtime);
	printf("%s\n", buf_file[0].name);
}

void printStat_t(){
	for(int j = 0; j<count; j++){
		printf("%-40s", buf_file[j].name);
		if(j%2 == 1) 
			printf("\n");
	}
}

void readStat(char *pathname, char *file, struct stat *st){
	char* str = ".";
	char* str2 = "..";
	char* strr = "root";
	if((strcmp(file, str) != 0) && (strcmp(file, str2) != 0) &&
			(strcmp((getpwuid(st->st_uid)->pw_name), strr) != 0) &&
			(file[0] != '.'))
	{
		int l = strlen(file);
		memset(buf_file[count].name, 0, 60);
		buf_file[count].ctype = '\0';
		memset(buf_file[count].sperm, 0, 10);
		buf_file[count].nlink = 0;
		memset(buf_file[count].uid, 0, 20);
		memset(buf_file[count].gid, 0, 20);
		buf_file[count].ssize = 0;
		memset(buf_file[count].mtime, 0, 30);
		buf_file[count].inode = 0;
		strncpy(buf_file[count].name, file, l);
		buf_file[count].ctype = type(st->st_mode);
		strncpy(buf_file[count].sperm, perm(st->st_mode), 10);
		buf_file[count].nlink = st->st_nlink;
		strncpy(buf_file[count].uid, getpwuid(st->st_uid)->pw_name, 20);
		strncpy(buf_file[count].gid, getgrgid(st->st_gid)->gr_name, 20);
		buf_file[count].ssize = st->st_size;
		strncpy(buf_file[count].mtime, ctime(&st->st_mtime)+4, 30);
		buf_file[count].inode = st->st_ino;
		buf_file[count].ttime = st->st_mtime;
		count++;
	}
}

void readStat_a(char *pathname, char *file, struct stat *st){
	char* strr = "root";
	char* str2 = "..";
	if((strcmp((getpwuid(st->st_uid)->pw_name), strr) != 0) || (strcmp(file,
					str2) == 0))
	{
		int l = strlen(file);
		memset(buf_file[count].name, 0, 60);
		buf_file[count].ctype = '\0';
		memset(buf_file[count].sperm, 0, 10);
		buf_file[count].nlink = 0;
		memset(buf_file[count].uid, 0, 20);
		memset(buf_file[count].gid, 0, 20);
		buf_file[count].ssize = 0;
		memset(buf_file[count].mtime, 0, 30);
		buf_file[count].inode = 0;
		strncpy(buf_file[count].name, file, l);
		buf_file[count].ctype = type(st->st_mode);
		strncpy(buf_file[count].sperm, perm(st->st_mode), 10);
		buf_file[count].nlink = st->st_nlink;
		strncpy(buf_file[count].uid, getpwuid(st->st_uid)->pw_name, 20);
		strncpy(buf_file[count].gid, getgrgid(st->st_gid)->gr_name, 20);
		buf_file[count].ssize = st->st_size;
		strncpy(buf_file[count].mtime, ctime(&st->st_mtime)+4, 30);
		buf_file[count].inode = st->st_ino;
		buf_file[count].ttime = st->st_mtime;
		count++;
	}
}

char type(mode_t mode){
	if(S_ISREG(mode)) return('-');
	if(S_ISDIR(mode)) return('d');
	if(S_ISCHR(mode)) return('c');
	if(S_ISBLK(mode)) return('b');
	if(S_ISLNK(mode)) return('l');
	if(S_ISFIFO(mode)) return('p');
	if(S_ISSOCK(mode)) return('s');
}

char* perm(mode_t mode){
	int i = 0;
	static char perms[10];
	for(int j = 0; j<9; j++){
		perms[j] = '-';
	}
	perms[9] = '\0';
	for (i=0; i<3; i++){
		if(mode & (S_IRUSR >> i*3))
			perms[i*3] = 'r';
		if(mode & (S_IWUSR >> i*3))
			perms[i*3+1] = 'w';
		if(mode & (S_IXUSR >> i*3))
			perms[i*3+2] = 'x';
	}
	return(perms);
}
