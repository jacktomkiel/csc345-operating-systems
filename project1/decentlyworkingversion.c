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
    char *history = "default";
    char *cmd; // store current command
    char dir[300];

    /* flags */
    int should_run = 1;         
    int background_process = 0;
    int run_command = 0;
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
        argc = 0, background_process = 0, run_command = 0, pipe_process = 0, redirect_in = 0, redirect_out = 0;

        printf("osh>%s$ ", getcwd(dir, sizeof(dir)));
        // printf("osh>");
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

        // this is causing problems idk why maybe use args[] instead
        // if (strcmp(cmdBuf, "\n") || strcmp(cmdBuf, "\0") == 0)
        // {
        //     continue;
        // }

        if (strcmp(cmdBuf, "!!") == 0)
        {
            if (strcmp(history, "default") == 0)
            {
                printf("No commands in history.\n");
                continue;
            }
            else
            {
                cmd = strdup(history);
                printf("%s\n", history);
                run_command = 1;
            }
        }
        else
        {
            cmd = cmdBuf;
            history = strdup(cmd);
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
        // for (int i = 0; i < argc; i++)
        // {
        //     printf("args: %s\n", args[i]);
        // }

        if (strcmp(args[0], "exit") == 0)
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

        if (strcmp(args[argc-1], "&") == 0)
        {
            background_process = 1;
            args[argc-1] = '\0';
            // printf("I am behaving correctly!\n");
        }

        if (argc > 2)
        {
            if (strcmp(args[argc-2], ">") == 0)
            {
                args[argc-2] = '\0';
                redirect_out = 1;
                // printf("I am behaving correctly!\n");
            }
            else if (strcmp(args[argc-2], "<") == 0)
            {
                args[argc-2] = '\0';
                redirect_in = 1;
                // printf("I am behaving correctly!\n");
            }
        }

        pid = fork();

        /* child process */
        if (pid == 0)
        {   
            /* if there is arguments, the child process will invoke execvp() */
            if (argc > 0)
            {
                execvp(args[0], args);
                continue;
            }
            /* otherwise, continue loop */
            else
            {
                should_run = 0;
                // printf("Enter an argument...\n");
            }
        }
        /* parent will invoke wait() unless str included & */
        else if (pid > 0)
        {
            if (background_process == 0)
            {
                wait(NULL);
                // printf("Child process finished...\n");
            }
            else
            {
                printf("I'm not waiting for you!\n");
            }
        }
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