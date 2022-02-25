/** Jack Tomkiel
CSC345-01
Lab 2 Excercise 3 */

// zombie process example

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>

int main(int argc, char **argv)
{
	pid_t pid = fork();

	// Parent and child processes execute concurrently
	if (pid == 0) // Child process
	{
		printf("child process pid=%d\n", pid);
	}
	else if (pid > 0) // Parent process
	{
		printf("parent process pid=%d\n", pid);
		sleep(10);
	}

	return 0;
}
