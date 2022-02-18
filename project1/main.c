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
    char dir[100];

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
        /* reset flags */
        argc = 0, background_process = 0, run_command = 0, pipe_process = 0, redirect_in = 0, redirect_out = 0;

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

        // causing problems
        /* if user enters nothing, restart shell loop */
        if (strcmp(cmdBuf, "\0") == 0)
        {
            continue;
        }

        // if (strcmp(cmdBuf, "exit") == 0)
        // {
        //     exit(0);
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
            }
        }
        else
        {
            cmd = cmdBuf;
            history = strdup(cmd);
        }

        // printf("command: %s\n", cmdBuf);

        /* parse the user's command into separate tokens and store as array of character strings, args[] */
        token = strtok(cmd," ");    /* divides user cmd into tokens at each whitespace*/
        while (token != NULL)       /* while there is tokens */
        {
            args[argc] = token;     /* store token in args[] array */
            token = strtok(NULL, " ");
            argc++;
        }
        args[argc] = '\0'; /* end with NULL to ensure correct execvp() functionality */

        // for (int i = 0; i < argc; i++)
        // {
        //     printf("args: %s\n", args[i]);
        // }

        /* when the user enters exit at prompt, program sets should_run = 0, and terminates */
        if (strcmp(args[0], "exit") == 0)
        {
            should_run = 0;
            continue;
        }

        /* since 'cd' is a "built in" shell function, it is implemented here */
        if (strcmp(args[0], "cd") == 0)
        {
            if (chdir(args[1]) == 0)    /* changes the current working directory, returns zero on success */
            {
                /* do nothing, since it was successful */
            }
            else
            {   
                /* otherwise, tell the user the directory does not exist */
                printf("osc: cd: %s: No such file or directory\n", args[1]); /* mimics bash output style */
            }
        }

        /* check whether user included & to determine whether or not the parent process should wait for child to exit */
        if (strcmp(args[argc-1], "&") == 0)
        {
            background_process = 1; /* set background_process flag */
            args[argc-1] = '\0';    /* replace & argument with NULL */
            // printf("I am behaving correctly!\n");
        }

        /* if there are more than 2 arguments, check for redirect operators */
        if (argc > 2)
        {
            if (strcmp(args[argc-2], ">") == 0)
            {
                args[argc-2] = '\0';    /* replace > argument with NULL */
                redirect_out = 1;       /* set redirect_out flag */
                // printf("I am behaving correctly!\n");
            }
            else if (strcmp(args[argc-2], "<") == 0)
            {
                args[argc-2] = '\0';    /* replace < argument with NULL */
                redirect_in = 1;        /* set redirect_in flag */
                // printf("I am behaving correctly!\n");
            }
        }
        // printf("args: %s\n", args[0]);

        /* child process is forked to execute command by the user */
        pid = fork();

        /* child process */
        if (pid < 0) 
        {
            printf("Fork Failed!");
            exit(0);
        }
        else if (pid == 0)
        {   
            /* if there is arguments, the child process will invoke execvp() */
            if (argc > 0)
            {
                execvp(args[0], args);
                //continue;
            }
            /* otherwise, continue loop */
            else
            {
                should_run = 0;
                printf("Enter an argument...\n");
            }
            exit(0); // kill child
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