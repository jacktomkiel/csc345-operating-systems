#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>

int check = 0;
int ccheck = 0;

typedef struct {
	int row;
	int column;
} parameters;

int board[9][9];

void *squareCheck(void* param) {
	if(check == 1)
	{
		pthread_exit(NULL);
	}
	parameters *p = (parameters*) param;
	int row = p->row;
	int col = p->column;
	int i,j;	

	int validity[9] = {0};
	for(i = row; i < row + 3; i++)
	{
		for(j = col; j < col + 3;j++)
		{
			int temp = board[i][j];
			
			if(validity[temp -1] == 1 || temp < 1 || temp > 9)
			{
				check = 1;
				pthread_exit(NULL);
			}
			else
			{
				validity[temp -1] = 1;
			}
		}
	}
	pthread_exit(NULL);
}

int squaresCheckC() {
	
	int i,j,row,col;
	for(row = 0; row < 3; row++)
	{
		for(col = 0; col < 3; col++)
		{
			int validity[9] = {0};
			for(i = row*3; i < 3*row + 3; i++)
			{
				
				for(j = col*3; j < 3*col + 3; j++)
				{
					int temp = board[i][j];
					
					if(validity[temp -1] == 1 || temp < 1 || temp > 9)
					{
						check = 1;
					}
					else
					{
						validity[temp -1] = 1;
					}
				} 
			}
		}
	}
	return check;
}

void *rowCheck(void* param) {
	if(check == 1)
	{
		pthread_exit(NULL);
	}
	
	parameters *p = (parameters*) param;
	int row = p->row;
	int col = p->column;
	
	int i,j;	
	
	int validity[9] = {0};
	for(j = 0; j < 9; j++)
	{
		int temp = board[row][j];
		
		if(validity[temp -1] == 1 || temp < 1 || temp > 9)
		{
			check = 1;
			pthread_exit(NULL);
		}
		else
		{
			validity[temp -1] = 1;
		}
	} 
	
	pthread_exit(NULL);
}

void *rowsCheck(void* param) {
	if(check == 1)
	{
		pthread_exit(NULL);
	}
	int i,j;	
	for(i = 0; i < 9; i++)
	{
		int validity[9] = {0};
		for(j = 0; j < 9; j++)
		{
			int temp = board[i][j];
			
			if(validity[temp -1] == 1 || temp < 1 || temp > 9)
			{
				check = 1;
				pthread_exit(NULL);
			}
			else
			{
				validity[temp -1] = 1;
			}
		} 
	}
	pthread_exit(NULL);
}

int rowsCheckC() {
	
	int i,j;	
	for(i = 0; i < 9; i++)
	{
		int validity[9] = {0};
		for(j = 0; j < 9; j++)
		{
			int temp = board[i][j];
			
			if(validity[temp -1] == 1 || temp < 1 || temp > 9)
			{
				check = 1;
			}
			else
			{
				validity[temp -1] = 1;
			}
		} 
	}
	return check;
}

void *colCheck(void* param) {
	if(check == 1)
	{
		pthread_exit(NULL);
	}
	
	parameters *p = (parameters*) param;
	int row = p->row;
	int col = p->column;
	
	int i,j;	
	
	int validity[9] = {0};
	for(j = 0; j < 9; j++)
	{
		int temp = board[j][col];
		
		if(validity[temp -1] == 1 || temp < 1 || temp > 9)
		{
			check = 1;
			pthread_exit(NULL);
		}
		else
		{
			validity[temp -1] = 1;
		}
	} 
	
	pthread_exit(NULL);
}

void *colsCheck(void* param) {
	if(check == 1)
	{
		pthread_exit(NULL);
	}
	int i,j;	
	for(i = 0; i < 9; i++)
	{
		int validity[9] = {0};
		for(j = 0; j < 9; j++)
		{
			int temp = board[j][i];
			
			if(validity[temp -1] == 1 || temp < 1 || temp > 9)
			{
				check = 1;
				pthread_exit(NULL);
			}
			else
			{
				validity[temp -1] = 1;
			}
		} 
	}
	pthread_exit(NULL);
}

int colsCheckC() {
	
	int i,j;	
	for(i = 0; i < 9; i++)
	{
		int validity[9] = {0};
		for(j = 0; j < 9; j++)
		{
			int temp = board[j][i];
			
			if(validity[temp -1] == 1 || temp < 1 || temp > 9)
			{
				check = 1;
			}
			else
			{
				validity[temp -1] = 1;
			}
		} 
	}
	return check;
}

int main(int argc, char**argv){
	struct timeval start, end;
	int threadNum, threadI;
	const char* name = "MEM";
	const int SIZE = 4096;
	int shm_fd;
	void *ptr;
	int pidI;
	
	if(argc == 1 || argc > 2 || (atoi(argv[1]) < 1) || (atoi(argv[1]) > 3))
	{
		printf("ERROR: Need single input paramter 1, 2, or 3\n");
		return 0;
	}
	
	
	
	FILE* fptr = fopen("input.txt", "r");
	if(fptr == NULL)
	{
		printf("ERROR: Could not open \"input.txt\"\n");
		return 0;
	}
	
	int b0,b1,b2,b3,b4,b5,b6,b7,b8;
	for(int i = 0; i <9;i++)
	{
		fscanf(fptr, "%d %d %d %d %d %d %d %d %d",&b0,&b1,&b2,&b3,&b4,&b5,&b6,&b7,&b8);
		board[i][0] = b0;
		board[i][1] = b1;
		board[i][2] = b2;
		board[i][3] = b3;
		board[i][4] = b4;
		board[i][5] = b5;
		board[i][6] = b6;
		board[i][7] = b7;
		board[i][8] = b8;
	}
	
	printf("BOARD STATE IN input.txt:\n");
	for(int i=0;i<9;i++){
		for(int j=0;j<9;j++)
			printf("%d ",board[i][j]);
		printf("\n");
	}
	
	check = 0;
	ccheck = 0;
	gettimeofday(&start,NULL);
	
	int swVar = atoi(argv[1]);
	
	pthread_t threadID[27];
	pid_t pidID[3];
	
	switch(swVar)
	{
		case 1:
			printf("case1\n");
			threadNum = 11;
			threadI = 0;
			pthread_create(&threadID[threadI], NULL, rowsCheck, NULL);
			threadI++;
			pthread_create(&threadID[threadI], NULL, colsCheck, NULL);
			threadI++;
			for(int i = 0; i < 9; i++)
			{
				for(int j = 0; j < 9; j++)
				{
					if(i%3 == 0 && j%3 == 0)
					{
						parameters* param = (parameters*)malloc(sizeof(parameters));
						param-> row = i;
						param->column = j;
						
						pthread_create(&threadID[threadI], NULL, squareCheck, param);
						threadI++;
					}
				}
			}
			for(int i = 0; i < threadNum; i++)
			{
				printf("waiting for %d\n",i);
				pthread_join(threadID[0],NULL);
			}
			
		break;
		
		case 2:
			printf("case2\n");
			threadNum = 27;
			threadI = 0;
			for(int i = 0; i < 9; i++)
			{
				for(int j = 0; j < 9; j++)
				{
					if(i==0)
					{
						parameters* param = (parameters*)malloc(sizeof(parameters));
						param-> row = i;
						param->column = j;
						
						pthread_create(&threadID[threadI], NULL, rowCheck, param);
						threadI++;
					}
					if(j==0)
					{
						parameters* param = (parameters*)malloc(sizeof(parameters));
						param-> row = i;
						param->column = j;
						
						pthread_create(&threadID[threadI], NULL, colCheck, param);
						threadI++;
					}
					if(i%3 == 0 && j%3 == 0)
					{
						parameters* param = (parameters*)malloc(sizeof(parameters));
						param-> row = i;
						param->column = j;
						
						pthread_create(&threadID[threadI], NULL, squareCheck, param);
						threadI++;
					}
				}
			}
			for(int i = 0; i < threadNum; i++)
			{
				printf("waiting for %d\n",i);
				pthread_join(threadID[0],NULL);
			}
		break;
		
		case 3:
			printf("case3\n");
			pidI = 0;
			
			pidID[pidI] = fork();
			if(pidID[pidI]==0)
			{
				shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
				ftruncate(shm_fd, SIZE);
				ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
				sprintf(ptr,"%d", squaresCheckC());
				return 0;
			}
			else
			{
				pidI++;
				pidID[pidI] = fork();
				if(pidID[pidI]==0)
				{
					shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
					ftruncate(shm_fd, SIZE);
					ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
					ptr+=4;
					sprintf(ptr,"%d", rowsCheckC());
					return 0;
				}
				else
				{
					pidI++;
					pidID[pidI] = fork();
					if(pidID[pidI]==0)
					{
						shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
						ftruncate(shm_fd, SIZE);
						ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
						ptr+=8;
						sprintf(ptr,"%d", colsCheckC());
						return 0;
					}
					else
					{
						printf("Waiting for child 3\n");
						wait(NULL);
					}
					printf("waiting for child 2\n");
					wait(NULL);
				}
				printf("waiting for child 1\n");
				wait(NULL);
			}
			
			shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
			ftruncate(shm_fd, SIZE);
			ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
			for(int i = 0; i < 3; i++)
			{
				if(strcmp((char *)ptr,"1")==0 || ccheck == 1)
				{
					ccheck = 1;
				}
			}
			shm_unlink(name);
		break;
		
		default : 
		printf("ERROR: Need single input paramter 1, 2, or 3\n");
		return 0;
		break;
	}
	
	gettimeofday(&end,NULL);
	
	double fullTime = (double)(end.tv_sec - start.tv_sec) + (((double)(end.tv_usec - start.tv_usec))/1000000);
	
	if(check == 0 && ccheck == 0) {
		printf("SOLUTION: YES (%f seconds)\n", fullTime );
	}
	else {
		printf("SOLUTION: NO (%f seconds)\n", fullTime );
	}
	
	return 0;
}
