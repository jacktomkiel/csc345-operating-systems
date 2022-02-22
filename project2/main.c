#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <sys/time.h>
#include <pthread.h>

int main(int argc, char**argv){
	struct timeval start, end;
	
	
	if(argc == 1 || argc > 2 || (atoi(argv[1]) < 1) || (atoi(argv[1]) > 3))
	{
		printf("ERROR: Need single input paramter 1, 2, or 3\n");
		return 0;
	}
	
	int board[9][9];
	
	FILE* ptr = fopen("input.txt", "r");
	if(ptr == NULL)
	{
		printf("ERROR: Could not open \"input.txt\"\n");
		return 0;
	}
	
	int b0,b1,b2,b3,b4,b5,b6,b7,b8;
	for(int i = 0; i <9;i++)
	{
		fscanf(ptr, "%d %d %d %d %d %d %d %d %d",&b0,&b1,&b2,&b3,&b4,&b5,&b6,&b7,&b8);
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
	
	gettimeofday(&start,NULL);
	switch(atoi(argv[1]))
	{
		case 1:
		printf("case1\n");
		break;
		
		case 2:
		printf("case2\n");
		break;
		
		case 3:
		printf("case3\n");
		break;
		
		default : 
		printf("ERROR: Need single input paramter 1, 2, or 3\n");
		return 0;
		break;
	}
	usleep(1524700);
	gettimeofday(&end,NULL);
	printf("SOLUTION: idkbro (%f seconds)\n", (double)(end.tv_sec - start.tv_sec) + (((double)(end.tv_usec - start.tv_usec))/1000000) );
	
	return 0;
}
