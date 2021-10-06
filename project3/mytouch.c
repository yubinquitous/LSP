#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <utime.h>

int main(int argc, char *argv[]){
	if (argc != 2){
		fprintf(stderr, "%s <file_name>\n", argv[0]);
		exit(0);
	}
	struct stat st;
	int fd = 0;
	if((fd = open(argv[1], O_RDWR)) < 0 ){
		fd = open(argv[1], O_RDWR | O_CREAT);
		lstat(argv[1], &st);
		st.st_mode &= 0;
		st.st_mode |= S_IRUSR;
		st.st_mode |= S_IWUSR;
		st.st_mode |= S_IRGRP;
		st.st_mode |= S_IWGRP;
		st.st_mode |= S_IROTH;
		chmod(argv[1], st.st_mode);
	}
	utime(argv[1], NULL);
	close(fd);
}
