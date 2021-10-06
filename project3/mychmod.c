#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char* argv[]){
	struct stat st;
	if (argc != 3){
		fprintf(stderr, "%s <perm> <file_name>\n", argv[0]);
		exit(1);
	}
	int a[4];
	int bnum[13]; 
	int num = atoi(argv[1]);
	a[0] = atoi(argv[1]) / 1000;
	a[1] = (atoi(argv[1]) % 1000) / 100;
	a[2] = (atoi(argv[1]) % 100) / 10;
	a[3] = atoi(argv[1]) % 10;
	int i = 0;
	for(i = 0; i<4; i++){
		if (a[i] == 0) {
			bnum[3*i] = 0; bnum[3*i + 1] = 0; bnum[3*i + 2] = 0;}
		else if (a[i] == 1){
			bnum[3*i] = 0; bnum[3*i + 1] = 0; bnum[3*i + 2] = 1;}
		else if (a[i] == 2){
			bnum[3*i] = 0; bnum[3*i + 1] = 1; bnum[3*i + 2] = 0;}
		else if (a[i] == 3){
			bnum[3*i] = 0; bnum[3*i + 1] = 1; bnum[3*i + 2] = 1;}
		else if (a[i] == 4){
			bnum[3*i] = 1; bnum[3*i + 1] = 0; bnum[3*i + 2] = 0;}
		else if (a[i] == 5){
			bnum[3*i] = 1; bnum[3*i + 1] = 0; bnum[3*i + 2] = 1;}
		else if (a[i] == 6){
			bnum[3*i] = 1; bnum[3*i + 1] = 1; bnum[3*i + 2] = 0;}
		else if (a[i] == 7){
			bnum[3*i] = 1; bnum[3*i + 1] = 1; bnum[3*i + 2] = 1;}
	}
	lstat(argv[2], &st);
	for (int k = 0; k<12; k++){
		if(bnum[k] == 1){
			st.st_mode |= S_ISUID >> k;
			chmod(argv[2], st.st_mode);
		}
	}
	return 0;
}
