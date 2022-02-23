#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>

/* The ith index in this array corresponds to the ith worker thread. If a worker sets 
its corresponding value to 1, it is indicating that its region of the Sudoku puzzle 
is valid. A value of 0 indicates otherwise. */
int threadResult[27] = {0};

typedef struct
{
	int row;
	int column;
} parameters;

void isBoardValid(int threadResult[], int numThreads){
	for (int i = 0; i < numThreads; i++)
	{
		if (threadResult[i] == 0)
		{
			printf("SOLUTION: NO ");
			return;
		}
	}
	printf("SOLUTION: YES ");
}

void printBoard(int board[9][9])
{
	printf("BOARD STATE IN input.txt:\n");
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
			printf("%d ", board[i][j]);
		printf("\n");
	}
}

int main(int argc, char** argv)
{
/* begin main */

	/* initialize 9x9 soduku board */
	int board[9][9];
	int numThreads;

	/* check for valid arguments */
	if ( argc != 2 || (atoi(argv[1]) < 1) || (atoi(argv[1]) > 3) )
	{
		printf("ERROR: Enter a single input parameter 1, 2, or 3\n");
		return 1;
	}

	/* open file, put data into 2-dimensional array */
	FILE *fp = fopen("input.txt", "r");
	if (fp == NULL)
	{
		printf("ERROR: Could not open \"input.txt\"\n");
		return 1;
	}
	else
	{
		for (int i = 0; i < 9; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				fscanf(fp, "%d", &board[i][j]);
			}
		}
		fclose(fp);
		/* print soduku board */
		printBoard(board);
	}

	/* based off user argument, use desired multithreaded / multiprocess implementation */
	switch (atoi(argv[1]))
	{
		case 1: /* 11 threads */
			printf("case1\n");
			numThreads = 11;
			//stuff
			isBoardValid(threadResult, numThreads);
			break;
		case 2: /* 27 threads */
			printf("case2\n");
			numThreads = 27;
			//stuff
			isBoardValid(threadResult, numThreads);
			break;
		case 3: /* multiprocess */
			printf("case3\n");
			numThreads = 0;
			//stuff
			break;
		default:
			return 1;
	}

/* end main */
return 0;
}