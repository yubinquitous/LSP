#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

int main(int argc, char* argv[]){
	int num = 0;
	int temp = 0;
	int fd1=0, fd2=0;
	int arr[1024];
	char str[20];
	char* fname1 = "before.txt";
	char* fname2 = "after.txt";

	printf("write number bigger than 9 : ");
	scanf("%d", &num);

	if ((fd1 = open(fname1, O_WRONLY|O_CREAT|O_TRUNC|O_APPEND, 0644)) < 0)
	{
		fprintf(stderr, "%s open error\n", fname1);
		exit(1);
	}
	if ((fd2 = open(fname2, O_WRONLY|O_CREAT|O_TRUNC|O_APPEND, 0644)) < 0)
	{
		fprintf(stderr, "%s open error\n", fname2);
		exit(1);
	}
	for (int i = 0; i<num; i++)
		arr[i] = (rand() % 100) +1;

	for (int i = 0; i<num; i++){
		sprintf(str, "%d", arr[i]);
		write(fd1, str, (int)strlen(str));
		write(fd1, "\n", 1);
	}

	for (int i = 0; i<num; i++){
		for (int j = 0; j<num-i-1; j++){
			if (arr[j] > arr[j+1]){
				temp = arr[j];
				arr[j] = arr[j+1];
				arr[j+1] = temp;
			}
		}
	}

	for (int i = 0; i<num; i++){
		sprintf(str, "%d", arr[i]);
		write(fd2, str, (int)strlen(str));
		write(fd2, "\n", 1);
	}
	exit(0);
}
