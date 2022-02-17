/** Jack Tomkiel
CSC345-01
Project #1: User Interface */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAX_LINE 80 /* The maximum length str */

int main(void)
{
    char *args[MAX_LINE / 2 + 1]; /* command line arguments */
    char str[MAX_LINE / 2 + 1];
    char *token;
    char *history = malloc(sizeof(char) * MAX_LINE);
    char *cmd;

    /* flags */
    int should_run = 1;         /* determine when to exit program */
    int background_process = 0; /* determine when to exit program */
    int pipe_process = 0;       /* determine when to exit program */
    int redirect_out = 0;       /* determine when to exit program */
    int redirect_in = 0;        /* determine when to exit program */

    int argc;
    int len;
    int history_count = 0;
    int pipefd[2]; 

    pid_t pid;
    pid_t pid_pipe;

    while (should_run)
    {
        int should_run = 1;         /* determine when to exit program */
        int background_process = 0; /* determine when to exit program */
        int pipe_process = 0;       /* determine when to exit program */
        int redirect_out = 0;       /* determine when to exit program */
        int redirect_in = 0;        /* determine when to exit program */
        printf("osh>");
        fflush(stdout);

        /* get user input, a newline-terminated string of finite length from STREAM */
        fgets(str, MAX_LINE, stdin); 

        if (strlen(str) <= 0)
        {
            return 0;
        }

        else
        {
            for(int i = 0; i < strlen(str); i++)
            {
                
            }
        }

        /* since fgets() considers \n as a valid character, we need to set it to NULL, \0 */
        if ((strlen(str) > 0) && (str[strlen(str) - 1] == '\n'))
        {
            str[strlen(str) - 1] = '\0';
        }
        
        /* echo user command for testing */
        printf("You Entered: %s\n", str);

        // history stuff
        if (strcmp(str,"!!") == 0)
        {
            if
            printf("You Entered: %s\n", str);
        }

        if (strcmp(str,"exit") == 0)
        {
            return 0;
        }
        /**
         * After reading user input, the steps are:
         * (1) fork a child process using fork()
         * (2) the child process will invoke execvp()
         * (3) parent will invoke wait() unless str included &
         */
    }
    return 0;
}
