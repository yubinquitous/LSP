#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char* argv[]){
	if(argc!=4){
		printf("%s <파일명>\n", argv[0]);
		exit(0);
	}

	char *cmd, *str;
	int fd1;
	char buf[BUFSIZ];
	int num;
	char c;

	num = atoi(argv[1]);
	str = argv[2];
	cmd = argv[3];
	c = str[0];

	fd1 = open(cmd, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
	if ( fd1 < 0){
		fprintf(stderr, "open error for %s\n", cmd);
		exit(1);
	}

	//1을 표준출력이 아닌 fd로 바꿈
	dup2(fd1, 1);

	//num이 양수 또는 0일 때
	if (num>=0){
		for (int i = 1; i<=num;i++){
			for (int j = 1; j<=i; j++){
				printf("%c", c);
			}
			printf("\n");
		}
	}
	else if (num <0){
		for (int i = 1; i<=-num; i++){
			for (int j = -num; j>=i; j--){
				printf("%c", c);
			}
			printf("\n");
		}
	}
	else ;
	exit(0);
}
