/** Jack Tomkiel
CSC345-01
Lab 2 Excercise 4 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>

int numDigits(int n)
{
	if (n < 10)
	{
		return 1;
	}
	return 1 + numDigits(n / 10);
}

int main(int argc, char **argv)
{
	// If user does not provide 1 argument, exit with error
	if (argc != 2)
	{
		printf("Error, enter only 1 argument.\n");
		return 1;
	}

	const char *name = "COLLATZ"; // file name
	const int SIZE = 4096;		  // file size
	int n = atoi(argv[1]);		  // get CLI args

	int shm_fd; // file descriptor, from shm_open()
	void *ptr;	// pointer to shared memory object

	pid_t pid = fork();

	// create shared memory area
	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);

	// configure the size of the shared memory object
	ftruncate(shm_fd, SIZE);

	// map the shared memory segment to the address space of the process
	ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

	// producer - child
	if (pid == 0)
	{
		while (n > 1)
		{
			// write to the shared memory object
			sprintf(ptr, " %d", n);
			// printf("n = %s memory address: %p \n",(char *)ptr, ptr); // useful for debugging/visualizing
			ptr += (numDigits(n) + 1);
			if (n % 2 == 0)
			{
				n = n / 2;
			}
			else
			{
				n = 3 * n + 1;
			}
		}
		sprintf(ptr, " %d", n);
		ptr += numDigits(n);
	}

	// consumer - parent
	else
	{
		// wait for child process to finish
		wait(NULL);

		// print from shared mem
		printf("Output: %s \n", (char *)ptr);

		// remove (unlink) shared mem object
		shm_unlink(name);
	}

	return 0;
}
