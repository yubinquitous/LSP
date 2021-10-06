#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char*argv[]){
	int timelimit = 0;
	pid_t pid;
	int status;
	if (argc == 2){
		timelimit = atoi(argv[1]);
	}
	if (timelimit == 0){
		while(1){
			if ((pid = fork()) == 0){
				execlp("date", "date", (char*)0);
			}
			else if (pid < 0){
				fprintf(stderr, "fork error\n");
			}
			else {
				waitpid(pid, &status, 0);
				sleep(1);
			}
		}
	}
	else {
		int i = timelimit;
		while(1){
			if ((pid = fork()) == 0){
				execlp("date", "date", (char*)0);
			}
			else if (pid < 0){
				fprintf(stderr, "fork error\n");
			}
			else {
				waitpid(pid, &status, 0);
				if (i == 0)	raise(SIGKILL);
				if (i <= 5)
					printf("종료 %d초 전\n", i);
				i--;
				sleep(1);
			}
		}
	}
	return 0;
}
