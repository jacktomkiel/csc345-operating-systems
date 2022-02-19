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
#define READ_END 0 
#define WRITE_END 1

int main(void)
{
    char *args[MAX_LINE / 2 + 1]; /* command line arguments */
    char cmdBuf[MAX_LINE / 2 + 1];
    char *token;
    char *history = "\0";
    char *cmd; // store current command
    char dir[100];

    /* flags */
    int should_run = 1;
    int background_process = 0;
    int run_command = 0;
    int pipe_process = 0;
    int pipe_index = 0;
    int redirect_out = 0;
    int redirect_in = 0;

    int argc;
    int fd[2];

    pid_t pid;
    pid_t pid_pipe;

    while (should_run)
    {
        should_run = 1;
        /* reset flags */
        argc = 0, background_process = 0, run_command = 0, pipe_process = 0, pipe_index = 0, redirect_in = 0, redirect_out = 0;

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

        /* history feature, if user enters history command, execute */
        if (strcmp(cmdBuf, "!!") == 0)
        {
            if (strcmp(history, "\0") == 0) /* if history is empty, tell the user, restart shell loop */
            {
                printf("No commands in history.\n");
                continue;
            }
            else
            {
                cmd = strdup(history);   /* strdup returns a pointer to history string, store pointer */
                printf("%s\n", history); /* print history string */
            }
        }
        /* if user did not enter history command, record their command */
        else
        {
            cmd = cmdBuf;          /* pointer to command entered by user */
            history = strdup(cmd); /* store pointer to command entered by user*/
        }

        // printf("command: %s\n", cmdBuf);

        /* parse the user's command into separate tokens and store as array of character strings, args[] */
        token = strtok(cmd, " "); /* divides user cmd into tokens at each whitespace*/
        while (token != NULL)     /* while there is tokens */
        {
            args[argc] = token; /* store token in args[] array */
            token = strtok(NULL, " ");
            argc++;
        }
        args[argc] = '\0'; /* end with NULL to ensure correct execvp() functionality */

        /* check for pipes */
        for (int i = 0; i < argc; i++)
        {
            if (strcmp(args[i], "|") == 0)
            {
                pipe_process = 1; /* set pipe flag */
                pipe_index = i;
            }
            // printf("args: %s\n", args[i]);
        }

        /* when the user enters exit at prompt, program sets should_run = 0, restarts shell loop, forcing termination */
        if (strcmp(args[0], "exit") == 0)
        {
            should_run = 0;
            continue; /* jumps to top of while() loop */
        }

        /* since 'cd' is a "built in" shell functionality, it must be handled here */
        if (strcmp(args[0], "cd") == 0)
        {
            if (chdir(args[1]) == 0) /* changes the current working directory, returns zero on success */
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
        if (strcmp(args[argc - 1], "&") == 0)
        {
            background_process = 1; /* set background_process flag */
            args[argc - 1] = '\0';  /* replace & argument with NULL */
            // printf("I am behaving correctly!\n");
        }

        /* if there are more than 2 arguments, check for redirect operators */
        if (argc > 2)
        {
            if (strcmp(args[argc - 2], ">") == 0)
            {
                args[argc - 2] = '\0'; /* replace > argument with NULL */
                redirect_out = 1;      /* set redirect_out flag */
                // printf("I am behaving correctly!\n");
            }
            else if (strcmp(args[argc - 2], "<") == 0)
            {
                args[argc - 2] = '\0'; /* replace < argument with NULL */
                redirect_in = 1;       /* set redirect_in flag */
                // printf("I am behaving correctly!\n");
            }
        }
        // printf("args: %s\n", args[0]);

        /* child process is forked to execute command by the user */
        pid = fork();

        /* if fork() fails, exit */
        if (pid < 0)
        {
            fprintf(stderr, "Fork failed");
            return 1;
        }
        /* child process */
        /* if child process & there is arguments, let the child process run */
        else if ((pid == 0) && (argc > 0))
        {
            fflush(stdout);
            /* assume the commands will contain either one input or one output direction, not both*/
            if (redirect_in == 1)
            {
                int fd0;
                if ((fd0 = open(args[argc - 1], O_RDONLY)) < 0)
                {
                    perror("could not open input file");
                    exit(0);
                }
                /* duplicates fd to standard output (the terminal) */
                dup2(fd0, STDIN_FILENO); /* STDIN_FILENO expands to 0, either works */
                close(fd0);
            }
            else if (redirect_out == 1)
            {
                int fd1;
                if ((fd1 = creat(args[argc - 1], 0644)) < 0)
                {
                    perror("could not open output file");
                    exit(0);
                }
                /* duplicates fd to standard input (the terminal) */
                dup2(fd1, STDOUT_FILENO); /* STDOUT_FILENO expands to 1, either works */
                close(fd1);
            }
            if (pipe_process == 1)
            {
                if (pipe(fd) == -1)
                {
                    fprintf(stderr, "Pipe failed");
                    return 1;
                }
                // pipe(fd);
                args[pipe_index] = '\0';    /* for execvp() function requirements */

                /* fork a grandchild process */
                pid_pipe = fork();
                /* if fork() fails, exit */
                if (pid_pipe < 0)
                {
                    fprintf(stderr, "Fork failed");
                    return 1;
                }
                /* grandchild process */
                if (pid_pipe == 0)
                {
                    dup2(fd[WRITE_END], STDOUT_FILENO);
                    close(fd[READ_END]);

                    execvp(args[0], args); /* pass the first arguments to execvp() function */
                    exit(0);               /* kill grandchild process after its done executing args */
                }
                /* grandparent */
                else if (pid_pipe > 0)
                {
                    wait(NULL);
                    dup2(fd[READ_END], STDIN_FILENO);
                    close(fd[WRITE_END]);

                    execvp(args[pipe_index + 1], args + (pipe_index + 1));  /* pass the piped arguments to execvp() function */
                    exit(0);                   /* kill grandparent process after its done executing args */
                }
            }
            else
            {
                execvp(args[0], args); /* pass args to execvp() function */
                exit(0);
            }
        }
        /* parent process */
        else if (pid > 0)
        {
            /* parent will invoke wait() unless str included & (background_process) flag*/
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
        // else
        // {
        //     /* if user does not enter args */
        //     should_run = 0;
        //     printf("Enter an argument...\n");
        // }
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