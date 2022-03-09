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

/* thread and child process check variables, no sync issue since we only set to 1 */
int check = 0;
int ccheck = 0;

/* parameter struct for threads */
typedef struct
{
	int row;
	int column;
} parameters;

/* global board so all threads can access it */
int board[9][9];

/* thread checks all squares */
void *squareCheck(void *param)
{
	/* if we already know the board is invalid don't bother checking */
	if (check == 1)
	{
		pthread_exit(NULL);
	}
	/* load parameters and prep iteration ints */
	parameters *p = (parameters *)param;
	int row = p->row;
	int col = p->column;
	int i, j;
	/* mark each index when we find a number, invalid if we find a duplicate */
	int validity[9] = {0};
	/* loop over the 3x3 square */
	for (i = row; i < row + 3; i++)
	{
		for (j = col; j < col + 3; j++)
		{
			/* temp the number in board, if its out of range or we already found it, invalid */
			int temp = board[i][j];
			if (validity[temp - 1] == 1 || temp < 1 || temp > 9)
			{
				/* if invalid, set flag and exit now */
				check = 1;
				pthread_exit(NULL);
			}
			else
			{
				/* mark that we saw this number */
				validity[temp - 1] = 1;
			}
		}
	}
	/* exit if the square was valid all the way through */
	pthread_exit(NULL);
}

/* child checks all squares */
int squaresCheckC()
{
	/* row and col to iterate over all squares, i and j to iteratea through each square */
	int i, j, row, col;
	for (row = 0; row < 3; row++)
	{
		for (col = 0; col < 3; col++)
		{
			/* new valid check for each square */
			int validity[9] = {0};
			for (i = row * 3; i < 3 * row + 3; i++)
			{

				for (j = col * 3; j < 3 * col + 3; j++)
				{
					/* same checking scheme as earlier */
					int temp = board[i][j];

					if (validity[temp - 1] == 1 || temp < 1 || temp > 9)
					{
						check = 1;
						return check;
					}
					else
					{
						validity[temp - 1] = 1;
					}
				}
			}
		}
	}
	/* return the value of check */
	return check;
}

/* thread checks one row, this is the same as squareCheck() but with a row instead of square */
void *rowCheck(void *param)
{
	if (check == 1)
	{
		pthread_exit(NULL);
	}

	parameters *p = (parameters *)param;
	int row = p->row;
	int col = p->column;

	int i, j;

	int validity[9] = {0};
	for (j = 0; j < 9; j++)
	{
		int temp = board[row][j];

		if (validity[temp - 1] == 1 || temp < 1 || temp > 9)
		{
			check = 1;
			pthread_exit(NULL);
		}
		else
		{
			validity[temp - 1] = 1;
		}
	}

	pthread_exit(NULL);
}

/* thread checks all rows, same as rowCheck() with an extra loop to go over all rows */
void *rowsCheck(void *param)
{
	if (check == 1)
	{
		pthread_exit(NULL);
	}
	int i, j;

	for (i = 0; i < 9; i++)
	{
		int validity[9] = {0};
		for (j = 0; j < 9; j++)
		{
			int temp = board[i][j];

			if (validity[temp - 1] == 1 || temp < 1 || temp > 9)
			{
				check = 1;
				pthread_exit(NULL);
			}
			else
			{
				validity[temp - 1] = 1;
			}
		}
	}
	pthread_exit(NULL);
}

/* child checks all rows */
int rowsCheckC()
{
	/* same as squaresCheckC() with i and j going through each row */
	int i, j;
	for (i = 0; i < 9; i++)
	{
		int validity[9] = {0};
		for (j = 0; j < 9; j++)
		{
			int temp = board[i][j];

			if (validity[temp - 1] == 1 || temp < 1 || temp > 9)
			{
				check = 1;
				return check;
			}
			else
			{
				validity[temp - 1] = 1;
			}
		}
	}
	return check;
}

/* thread checks one column, same as rowCheck() but checking a col instead */
void *colCheck(void *param)
{
	if (check == 1)
	{
		pthread_exit(NULL);
	}

	parameters *p = (parameters *)param;
	int row = p->row;
	int col = p->column;

	int i, j;

	int validity[9] = {0};
	for (j = 0; j < 9; j++)
	{
		int temp = board[j][col];

		if (validity[temp - 1] == 1 || temp < 1 || temp > 9)
		{
			check = 1;
			pthread_exit(NULL);
		}
		else
		{
			validity[temp - 1] = 1;
		}
	}

	pthread_exit(NULL);
}

/* thread checks all the columns, same as rowsCheck() but checking all cols */
void *colsCheck(void *param)
{
	if (check == 1)
	{
		pthread_exit(NULL);
	}
	int i, j;
	for (i = 0; i < 9; i++)
	{
		int validity[9] = {0};
		for (j = 0; j < 9; j++)
		{
			int temp = board[j][i];

			if (validity[temp - 1] == 1 || temp < 1 || temp > 9)
			{
				check = 1;
				pthread_exit(NULL);
			}
			else
			{
				validity[temp - 1] = 1;
			}
		}
	}
	pthread_exit(NULL);
}

/* child checks all the columns, same as rowsCheckC() but checking all columns */
int colsCheckC()
{

	int i, j;
	for (i = 0; i < 9; i++)
	{
		int validity[9] = {0};
		for (j = 0; j < 9; j++)
		{
			int temp = board[j][i];

			if (validity[temp - 1] == 1 || temp < 1 || temp > 9)
			{
				check = 1;
				return check;
			}
			else
			{
				validity[temp - 1] = 1;
			}
		}
	}
	return check;
}

/* print the board */
void printBoard()
{
	printf("BOARD STATE IN input.txt:\n");
	/* loop through whole board, outputting the value and a space */
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
			printf("%d ", board[i][j]);
		printf("\n");
	}
}

int main(int argc, char **argv)
{
	/* struct for tracking computation time */
	struct timeval start, end;
	/* thread tracking ints */
	int threadNum, threadI;
	/* shared mem for child processes */
	const char *name = "MEM";
	const int SIZE = 4096;
	int shm_fd;
	void *ptr;
	/* child process tracking int */
	int pidI;

	/* check for valid arguments */
	if (argc != 2 || (atoi(argv[1]) < 1) || (atoi(argv[1]) > 3))
	{
		printf("ERROR: Enter a single input parameter 1, 2, or 3\n");
		return 1;
	}

	/* open file, put data into 2-dimensional array */
	FILE *fp = fopen("input.txt", "r");
	/* make sure file could be opened first */
	if (fp == NULL)
	{
		printf("ERROR: Could not open \"input.txt\"\n");
		return 1;
	}
	else
	{
		/* loop through all positions and store in board[i][j] */
		for (int i = 0; i < 9; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				fscanf(fp, "%d", &board[i][j]);
			}
		}
		fclose(fp);
		/* print soduku board */
		printBoard();
	}
	/* Assume board is correct before checking */
	check = 0;
	ccheck = 0;

	/* grab start time */
	gettimeofday(&start, NULL);

	/* check which checking scheme we will use */
	int swVar = atoi(argv[1]);

	/* we will have at most 27 threads and 3 child processes */
	pthread_t threadID[27];
	pid_t pidID[3];

	/* switch based off the input paramter to decide which techinique to use */
	switch (swVar)
	{
	case 1: /* 11 threads, case 1 */
		/* printf("case1\n"); */
		threadNum = 11;
		threadI = 0;
		/* one thread to check rows, one thread to check cols */
		pthread_create(&threadID[threadI], NULL, rowsCheck, NULL);
		threadI++;
		pthread_create(&threadID[threadI], NULL, colsCheck, NULL);
		threadI++;
		/* looop through the whole board, make thread for square when we are at the top left of a square */
		for (int i = 0; i < 9; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				/* the top left of a square has both coordinates as multiples of 3 */
				if (i % 3 == 0 && j % 3 == 0)
				{
					/* allocate for parameter passing, only need to pass the starting position */
					parameters *param = (parameters *)malloc(sizeof(parameters));
					param->row = i;
					param->column = j;
					/* make thread, passing param */
					pthread_create(&threadID[threadI], NULL, squareCheck, param);
					threadI++;
				}
			}
		}
		/* wait for all 11 threads to be done */
		for (int i = 0; i < threadNum; i++)
		{
			/* printf("waiting for thread %d\n",i); */
			pthread_join(threadID[0], NULL);
		}

		break;

	case 2: /* 27 threads, case 2 */
		/* printf("case2\n"); */
		threadNum = 27;
		threadI = 0;
		/* loop through whole board, make threads in appropriate starts of rows cols or squares */
		for (int i = 0; i < 9; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				/* check row when we are at the start of one (j ==0) */
				if (j == 0)
				{
					/* allocate params and make thread */
					parameters *param = (parameters *)malloc(sizeof(parameters));
					param->row = i;
					param->column = j;

					pthread_create(&threadID[threadI], NULL, rowCheck, param);
					threadI++;
				}
				/* check col when we are at the start of one (i == 0) */
				if (i == 0)
				{
					parameters *param = (parameters *)malloc(sizeof(parameters));
					param->row = i;
					param->column = j;

					pthread_create(&threadID[threadI], NULL, colCheck, param);
					threadI++;
				}
				/* check square when are at the start of one (i %3 and j%3 ==0) */
				if (i % 3 == 0 && j % 3 == 0)
				{
					parameters *param = (parameters *)malloc(sizeof(parameters));
					param->row = i;
					param->column = j;

					pthread_create(&threadID[threadI], NULL, squareCheck, param);
					threadI++;
				}
			}
		}
		/* wait for all 27 threads to finish */
		for (int i = 0; i < threadNum; i++)
		{
			/* printf("waiting for thread %d\n",i); */
			pthread_join(threadID[0], NULL);
		}
		break;

	case 3: /* 3 children processes, case 3 */
		/* printf("case3\n"); */
		pidI = 0;
		/* fork first child */
		pidID[pidI] = fork();
		if (pidID[pidI] == 0) /* squares child */
		{
			/* open shared mem to store result accessible to parent */
			shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
			ftruncate(shm_fd, SIZE);
			ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
			/* store result in shared mem */
			sprintf(ptr, "%d", squaresCheckC());
			return 0;
		}
		else
		{
			pidI++;
			/* fork second child */
			pidID[pidI] = fork();
			if (pidID[pidI] == 0) /* rows child */
			{
				/* open shared mem */
				shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
				ftruncate(shm_fd, SIZE);
				ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
				/* store result but do not overwrite shared mem */
				ptr += 4;
				sprintf(ptr, "%d", rowsCheckC());
				return 0;
			}
			else
			{
				pidI++;
				/* fork thrid child */
				pidID[pidI] = fork();
				if (pidID[pidI] == 0) /* cpl child */
				{
					/* open shared mem */
					shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
					ftruncate(shm_fd, SIZE);
					ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
					/* store reuslt in open space in shared mem */
					ptr += 8;
					sprintf(ptr, "%d", colsCheckC());
					return 0;
				}
				else /* parent will wait for each child to finish */
				{
					/* printf("Waiting for child 3\n"); */
					wait(NULL);
				}
				/* printf("waiting for child 2\n"); */
				wait(NULL);
			}
			/* printf("waiting for child 1\n"); */
			wait(NULL);
		}
		/* open shared mem to look at child results */
		shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
		ftruncate(shm_fd, SIZE);
		ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
		for (int i = 0; i < 3; i++)
		{
			/* check the result of each child, update ccheck if necessary */
			if (strcmp((char *)ptr, "1") == 0 || ccheck == 1)
			{
				ccheck = 1;
			}
			ptr += 4;
		}
		/* close  shared mem */
		shm_unlink(name);
		break;

	default:
		printf("ERROR: Enter a single input parameter 1, 2, or 3\n");
		return 1;
		break;
	}

	/* get time and compute the full time it took, time stored in second and usec portion */
	gettimeofday(&end, NULL);
	double fullTime = (double)(end.tv_sec - start.tv_sec) + (((double)(end.tv_usec - start.tv_usec)) / 1000000);

	/* print if board is valid or not, as well as full time */
	if (check == 0 && ccheck == 0)
	{
		printf("SOLUTION: YES (%f seconds)\n", fullTime);
	}
	else
	{
		printf("SOLUTION: NO (%f seconds)\n", fullTime);
	}

	return 0;
}
