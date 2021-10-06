#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[]){
	pid_t *pid;
	if (argc != 2){
		fprintf(stderr, "%s <numofchild>\n", argv[0]);
		exit(0);
	}
	int numofchild = atoi(argv[1]);
	if (numofchild > 60){
		printf("1 - 60 사이의 값을 입력해주세요\n");
		exit(0);
	}
	pid = (pid_t *)malloc(sizeof(pid_t) * numofchild);
	pid[numofchild-1] = -1;
	for (int p = 1; p<=numofchild; p++){
		if ((pid[p-1] = fork()) == 0){
			while(1){
				sleep(5);
				printf("I'm child %d\n", p);
			}
		}
		else if (pid[p-1] < 0){
			fprintf(stderr, "fork error\n");
			exit(0);
		}
		else {
			;
		}
	}
	while(1){
		if (pid[numofchild-1] != -1)
			break;
	}
	sleep(5);
	for (int i = numofchild; i>0; i--){
		kill(pid[i-1], SIGKILL);
		sleep(1);
	}
	while(wait((int *)0) != -1);
	printf("종료\n");
	raise(SIGKILL);
}
