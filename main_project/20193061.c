#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <pthread.h>
#include <fcntl.h>

#define BUF_SIZE 200000010

char *cmd; //argv
int **arr;
int m = 0;
int n = 0;
int **arr2;
int *part;	//파트 분배
int gen = 0;	//세대 수
int *thread_n;
pthread_t *tid;	//tid 배열
pid_t *pid;		//pid 배열
struct Nthreads {
	int nthread;	//n번째 thread
};
struct Buf {
	int row;
	int column;
	int value;	
};

struct Nthreads *nth;
struct Buf buf;

void sequential_process(void);
void multi_process(int);
void multi_thread(int);
int ask_multi(void);	//생성할 child process/thread 수 입력받는 함수
void ask_gen(void);	//세대 수 입력받는 함수
void make_matrixfile(int);	//자식세대 파일 생성 함수
void part_division(int);	//한 process/thread당 수행할 파트 분배
void *thread_work(void *arg);
void play_game(int);	//세포 다음 세대 생사 결정
void read_input(void);	//입력 파일 읽는 함수


void sequential_process(void){
	read_input();	//입력파일 읽기

	//게임진행
	for (int k=0; k<gen; k++){
		for (int i=0; i<m; i++){
			play_game(i);	//세포 다음 세대 생사 결정
		}
		for (int i = 0; i<m;i++){
			for(int j=0; j<n; j++){
				arr[i][j] = arr2[i][j];
			}
		}	
		make_matrixfile(k);	//gen_n.matrix 생성
	}	
}

void multi_process(int processes){
	int processes_size = 4*processes;
	int pipe_fd[2];

	read_input();	//입력파일 읽기

	//행분배
	part = (int*)malloc(processes_size);
	pid = (pid_t*)malloc(processes*sizeof(pid_t));
	part_division(processes);

	pipe(pipe_fd);


	//프로세스 생성 및 실행
	for (int k = 0; k<gen; k++){
		int count = 0;
		memset(pid, 0, processes*sizeof(pid_t));
		for (int p = 1; p<=processes; p++){		//현재 p번째 process 수행
			int line = 0;
			for (int q = 1; q<p; q++){
				line += part[q-1];
			}

			if ((pid[p-1] = fork()) == 0){
				for (int g=0; g<part[p-1]; g++){
					int i = g+line;
					play_game(i);
				}
				for (int g=0; g<part[p-1]; g++){
					int i = g+line;
					for (int x=0; x<n; x++){
						buf.row = i;
						buf.column = x;
						buf.value = arr2[i][x];
						write(pipe_fd[1], &buf, sizeof(struct Buf));
					}
				}
				exit(0);
			}

			else if (pid[p-1] < 0){
				fprintf(stderr, "fork error\n");
				exit(1);
			}
			else { 	//부모 프로세스
				;
			}
		}

		while((read(pipe_fd[0], &buf, sizeof(struct Buf))) > 0){
			int a = buf.row;
			int b = buf.column;
			arr2[a][b] = buf.value;
			count++;
			if (count == m*n){
				break;
			}
		}

		while( (wait((int*)0)) != -1 );		//자식 프로세스 다 끝날때까지 wait

		//pid 출력
		printf("pid : ");
		for (int a=0; a<processes; a++){
			printf("%d ", pid[a]);
		}
		printf("\n");

		//새로운 세대 파일 생성
		for (int a=0; a<m; a++){
			for (int b=0; b<n; b++){
				arr[a][b] = arr2[a][b];
			}
		}

		make_matrixfile(k);	//결과 파일 생성
	}
	return;
}


void multi_thread(int threads){
	read_input();	//입력파일 읽기

	//행 분배
	int threads_size = 4*threads;
	nth = (struct Nthreads *)malloc(threads*sizeof(struct Nthreads));	//구조체

	tid = (pthread_t *)malloc(threads*sizeof(pthread_t));
	part = (int*)malloc(threads_size);

	part_division(threads);

	//스레드 생성 및 실행
	for (int k=0; k<gen; k++){
		printf("tid : ");
		for (int t = 0; t<threads; t++){
			nth[t].nthread = t+1;
			if (pthread_create(&tid[t], NULL, thread_work, (void*)&nth[t]) != 0){
				fprintf(stderr, "pthread_create error\n");
				exit(1);
			}
		}

		for (int i=0; i<threads; i++){
			if ((pthread_join(tid[i], NULL)) != 0) {
				fprintf(stderr, "pthread_join error\n");
				exit(1);
			}
		}

		for (int i = 0; i<m; i++){		//다음 세대 실행 준비
			for (int j = 0; j<n; j++){
				arr[i][j] = arr2[i][j];
			}
		}
		make_matrixfile(k);	//결과 파일 생성	
		printf("\n");
	}
	return;
}



void *thread_work(void *arg){
	struct Nthreads *t = (struct Nthreads *)arg;
	printf("%lu ", pthread_self());
	int line = 0;
	for (int i=1; i<t->nthread; i++){	//현재까지 수행된 행 
		line += part[i-1];
	}	

	for (int k=0; k<part[t->nthread-1]; k++){
		int i = k+line;
		play_game(i);
	}
	pthread_exit(NULL);
}


void part_division(int multi){
	int a = m/multi;
	int b = m%multi;
	for (int i=0; i<multi; i++){
		part[i] = a;
		if (i<b)
			part[i]++;
	}
}

void make_matrixfile(int k){
	FILE *fp_gen;
	char str1[] = "gen_";
	char str2[] = ".matrix";
	char gen_n[40];	//파일 이름
	char *buf;	//2차원 배열을 줄단위로 담을 버퍼
	int l = 2*n;
	buf = (char*)malloc(l);
	int offset = 0;
	char c = ' ';
	char enter = '\n';

	if (k == gen-1){	//최종 결과 파일
		fp_gen = fopen("output.matrix", "w+");
	}
	else {	//세대별 중간 결과 파일
		memset(gen_n, 0, 40);
		sprintf(gen_n, "%s%d%s", str1,k+1,str2);
		fp_gen = fopen(gen_n, "w+");
	}

	fseek(fp_gen, 0, SEEK_SET);

	//2차원 배열 -> matrix로
	for (int i=0; i<m; i++){
		memset(buf, 0, l);
		for (int j=0; j<n; j++){
			offset = j*2;
			sprintf(buf+offset, "%d", arr[i][j]);
			if (j == n-1)	sprintf(buf+offset+1, "%c", enter);
			else	sprintf(buf+offset+1, "%c", c);
		}
		fwrite(buf, l, 1, fp_gen);
	}		
	fclose(fp_gen);
}

int ask_multi(){
	int multi=0;
	while(1){
		printf("생성할 process/thread 수를 입력해주세요 : ");
		scanf("%d", &multi);
		if (multi < 1){
			printf("1 이상의 수를 입력해 주세요\n");
		}
		else break;
	}
	return multi;
}

void ask_gen(){
	while(1){
		printf("세대 수를 입력해주세요 : ");
		scanf("%d", &gen);
		if (gen < 1){
			printf("1 이상의 수를 입력해 주세요\n");
		}
		else break;
	}
	return;
}

void play_game(int i){
	for (int j =0; j<n; j++){
		int count = 0;
		if (j==0);
		else 
			if (arr[i][j-1] == 1)	count++;	//1. arr[i][j-1] 체크

		if ((j==0) || (i==0))	;
		else 
			if (arr[i-1][j-1] == 1)	count++;	//2. arr[i-1][j-1] 체크

		if (i==0)	;
		else
			if (arr[i-1][j] == 1)	count++;	//3. arr[i-1][j] 체크

		if ((i==0) || (j==n-1))	;
		else 
			if (arr[i-1][j+1] == 1)	count++;	//4. arr[i-1][j+1] 체크

		if (j == n-1)	;
		else 
			if (arr[i][j+1] == 1)	count++;	//5. arr[i][j+1] 체크

		if ((j== n-1)|| (i== m-1))	;
		else
			if (arr[i+1][j+1] == 1)	count++;	//6. arr[i+1][j+1] 체크

		if (i== m-1)	;
		else 
			if (arr[i+1][j] == 1)	count++;	//7. arr[i+1][j] 체크

		if ((i == m-1) || (j==0))	;
		else
			if (arr[i+1][j-1] == 1)	count++;	//8. arr[i+1][j-1]	체크

		//다음 세대 세포 생사 결정
		if (arr[i][j] == 1){ 	//현재 살아있을 경우
			if (count <= 2 || count >=7)	arr2[i][j] = 0;
			else if (count >= 3 && count <= 6) arr2[i][j] = 1;
			else ;
		}
		else if (arr[i][j] == 0){	//현재 죽어있을 경우
			if (count == 4)	arr2[i][j] = 1;
			else if (count != 4) arr2[i][j] = 0;
			else ;
		}
		else ;
	}
	return;
}

void read_input(void){
	FILE *fp;
	char *buffer;
	char *buffer2;
	int l=0;	//buffer길이

	buffer = (char*)malloc(BUF_SIZE);

	//m, n 읽어오기
	int fd = open(cmd, O_RDONLY);
	int file_size = 0, temp=0;

	file_size = read(fd, buffer, BUF_SIZE);

	while(1){
		temp = read(fd, buffer, BUF_SIZE);
		if (temp > 0){
			file_size += temp;
		}
		else break;
	}

	lseek(fd, 0, 0);
	read(fd, buffer, BUF_SIZE);
	close(fd);

	fp = fopen(cmd, "r+");

	while(1){
		if(buffer[l] == '\n'){
			l++;
			break;
		}
		else if (buffer[l] == '0' || buffer[l] == '1'){
			n++;
			l++;
		}
		else l++;
	}

	n = l/2;
	m = file_size/(l-1);

	buffer2 = (char*)malloc(l);
	//2차원 배열에 matrix 값 저장
	arr = (int**)malloc(sizeof(int*)*m);
	for (int i=0; i<m; i++){
		arr[i] = (int*)malloc(sizeof(int)*n);
	}

	arr2 = (int**)malloc(sizeof(int*)*m);
	for (int i=0; i<m; i++){
		arr2[i] = (int*)malloc(sizeof(int)*n);
	}
	fseek(fp, 0, SEEK_SET);

	//2차원 배열 초기화
	for (int i=0;i<m;i++){
		for(int j=0;j<n;j++){
			arr[i][j] = 0;
		}
	}
	fseek(fp, 0, SEEK_SET);

	for (int i=0; i<m; i++){
		memset(buffer2, 0, l);
		fread(buffer2, l-1, 1, fp);
		int k = 0;
		for (int j=0;j<l-1;j++){
			if (buffer2[j] == '0'){
				arr[i][k] = 0;
				k++;
			}
			else if (buffer2[j] == '1'){
				arr[i][k] = 1;
				k++;
			}
			else ;
		}
		fseek(fp, 1, SEEK_CUR);
	}

	free(buffer);
	free(buffer2);
	fclose(fp);
	return;
}


int main(int argc, char* argv[]){
	int num=0;	//동작 번호
	//int i=0, j=0, k=0;	//반복문
	struct timeval start_time, end_time;	//시간 측정용
	double delay_time;
	int nprocess=0, nthread=0;
	cmd = argv[1];

	//동작 번호 입력
	while(1){
		printf("번호를 입력해주세요(1: 프로그램 종료/ 2: 순차처리/ 3: Process 병렬처리/ 4: Thread 병렬처리) : ");
		scanf("%d", &num);

		if (num == 1){
			printf("프로그램 종료\n");
			exit(0);
		}
		else if (num == 2){	//순차처리
			ask_gen();
			gettimeofday(&start_time, NULL);	//시간측정 시작
			sequential_process();	//순차처리
			gettimeofday(&end_time, NULL);		//시간측정 종료
			delay_time = ((end_time.tv_sec - start_time.tv_sec)*1000) + ((end_time.tv_usec -
						start_time.tv_usec)*0.001);
			printf("총 수행 시간 : %f ms\n", delay_time);

		}
		else if (num == 3){	//프로세스 병렬
			nprocess = ask_multi();	//생성할 프로세스 수
			ask_gen();	//생성할 자식 파일 수
			gettimeofday(&start_time, NULL);
			multi_process(nprocess);
			gettimeofday(&end_time, NULL);
			delay_time = ((end_time.tv_sec - start_time.tv_sec) * 1000) + ((end_time.tv_usec -
						start_time.tv_usec)*0.001);
			//pid, 총 수행시간 출력
			printf("총 수행 시간 : %f ms\n", delay_time);
		}
		else if (num == 4){	//스레드 병렬
			nthread = ask_multi();	//생성할 스레드 수
			ask_gen();	//생성할 자식 파일 수
			gettimeofday(&start_time, NULL);
			multi_thread(nthread);
			gettimeofday(&end_time, NULL);
			delay_time = ((end_time.tv_sec - start_time.tv_sec)*1000) + ((end_time.tv_usec -
						start_time.tv_usec)*0.001);

			//tid, 총 수행시간 출력
			printf("총 수행 시간 : %f ms\n", delay_time);
		}
		else{
			printf("1-4 사이의 수를 입력해주세요\n");
		}
	}

	free(arr);
	free(arr2);
	exit(0);
}
