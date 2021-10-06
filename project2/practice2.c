#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#define len 50

int main(int argc, char*argv[]){
	if(argc != 2){
		printf("%s <파일명>", argv[0]);
		exit(0);
	}

	char *cmd1, *cmd2;
	int l, fd1, fd2;
	char str1[len], str2[len];
	char buf1[BUFSIZ], buf2[BUFSIZ];

	cmd1 = argv[1];
	strcpy(str1, cmd1);
	l = strlen(str1);
	for (int i = 0; i<l; i++){
		if('A'<=str1[i] && str1[i] <='Z')
			str2[i] = str1[i]+32;
		else if ('a'<=str1[i] && str1[i] <='z')
			str2[i] = str1[i]-32;
		else str2[i] = str1[i];
	}

	cmd2 = str2;
	fd1 = open(cmd1, O_RDONLY);
	fd2 = open(cmd2, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);

	read(fd1, buf1, sizeof(buf1));
	for (int j = 0; j<BUFSIZ; j++){
		if('A'<=buf1[j] && 'Z' >=buf1[j])
			buf2[j] = buf1[j] + 32;
		else if ('a' <= buf1[j] && 'z' >= buf2[j])
			buf2[j] = buf1[j] - 32;
		else buf2[j] = buf1[j];
	}
	write(fd2, buf2, sizeof(buf2));
	
	exit(0);
}
