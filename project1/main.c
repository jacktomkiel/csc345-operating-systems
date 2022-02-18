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
    char cmdBuf[MAX_LINE / 2 + 1];
    char *token;
    char *history = "0";
    char *cmd; // store current command
    char dir[300];

    /* flags */
    int should_run = 1;         
    int background_process = 0; 
    int pipe_process = 0;       
    int redirect_out = 0;       
    int redirect_in = 0;        

    int argc;
    int pipefd[2]; 

    pid_t pid;
    pid_t pid_pipe;

    while (should_run)
    {
        should_run = 1;
        argc = 0, background_process = 0, pipe_process = 0, redirect_in = 0, redirect_out = 0;

        printf("osh>%s$ ", getcwd(dir, sizeof(dir)));
        fflush(stdout);

        /* get user input, a newline-terminated string of finite length from STREAM */
        fgets(cmdBuf, MAX_LINE, stdin); 

        /* since fgets() considers \n as a valid character, we need to set it to NULL, \0 */
        if (cmdBuf[strlen(cmdBuf) - 1] == '\n')
        {
            cmdBuf[strlen(cmdBuf) - 1] = '\0';
        }

        /* echo user command for testing */
        // printf("You Entered: %s\n", cmdBuf);


        if (strcmp(cmdBuf, "\n") == 0)
        {
            continue;
        }

        /* parse cmdBuf into args[] */
        token = strtok(cmdBuf," ");
        while (token != NULL) 
        {
            args[argc] = token;
            token = strtok(NULL, " ");
            argc++;
        }
        args[argc] = '\0'; // end with NULL for execvp() functionality

        if (strcmp(args[0], "exit\n") == 0)
        {
            should_run = 0;
        }

        if (strcmp(args[0], "cd") == 0)
        {
            if (chdir(args[1]) == 0)
            {
                chdir(args[1]);
            }
            else
            {
                printf("Directory does not exist!\n");
            }
        }

        if (strcmp(args[argc-1], "&\n") == 0)
        {
            background_process = 1;
            args[argc-1] = '\0';
        }

        if (argc > 2)
        {
            if (strcmp(args[argc-2], ">") == 0)
            {
                args[argc-2] = '\0';
                redirect_out = 1;
            }
            else if (strcmp(args[argc-2], "<") == 0)
            {
                args[argc-2] = '\0';
                redirect_in = 1;
            }
        }

        pid = fork();

        /* child process */
        if (pid == 0)
        {   
            /* if there is arguments, the child process will invoke execvp() */
            if (argc > 0)
            {
                execvp(args[0],args);
            }
            /* otherwise, continue loop */
            else
            {
                continue;
                // printf("Enter an argument...\n");
            }
        }
        /* parent will invoke wait() unless str included & */
        else if (pid > 0)
        {
            if (background_process = 0)
            {
                wait(NULL);
            }
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

// if (strcmp(cmdBuf, "!!") == 0)
        // {
        //     if (strcmp(history, "0") == 0)
        //     {
        //         printf("No commands in history.\n");
        //         continue;
        //     }
        //     else
        //     {
        //         cmd = strdup(history);
        //         printf("test %s\n", history);
        //     }
        // }
        // else
        // {
        //     cmd = cmdBuf;
        //     history - strdup(cmd);
        // }


        /* echo user command for testing */
        // printf("You Entered: %s\n", cmdBuf);

        // for (int i = 0; i < argc; i++)
        // {
        //     printf("args: %s\n", args[i]);
        // }



        // history stuff
        // if (strcmp(cmdBuf,"!!") == 0)
        // {
        //     if (history_count <= 0){
        //         printf("no history\n");
        //     }
        //     else
        //     {
        //         printf("history\n");
        //     }
            
        // }

        // if (strcmp(cmdBuf,"exit\0") == 0)
        // {
        //     return 0;
        // }

        // for (int i = 0; i < argc; i++)
        // {
        //     break;
        // }