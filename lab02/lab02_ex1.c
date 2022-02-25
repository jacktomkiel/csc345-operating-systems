/** Jack Tomkiel
CSC345-01
Lab 2 Excercise 1 */

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>

int value = 5;

int main(int argc, char **argv)
{
        pid_t pid;
        pid = fork();
        if (pid == 0) // Child process
        {
                value += 15;
                return 0;
        }
        else if (pid > 0) // Parent process
        {
                wait(NULL);
                printf("PARENT: value = %d\n", value); // Value here is 5 because the value is only changed in the child process
                return 0;
        }
}
