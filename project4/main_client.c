#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>

#define PORT_NUM 1004

int randomOffset; /* random offset for random colors for each user */
char *roomNum;	  /* roomNum for user input of the desired room number */

/* ANSI escape codes for different color text */
char colors[13][19] = {
	"\033[0;31m",
	"\033[0;32m",
	"\033[0;33m",
	"\033[0;34m",
	"\033[0;35m",
	"\033[0;36m",
	"\033[0;91m",
	"\033[0;92m",
	"\033[0;93m",
	"\033[0;94m",
	"\033[0;95m",
	"\033[0;96m",
	"\033[0;00m"};

/* error reporting */
void error(const char *msg)
{
	perror(msg);
	exit(0);
}

/* reverse string, useful for pre-pending data to outputs */
char *strrev(char *str)
{
	/* checks */
	if (!str || !*str)
	{
		return str;
	}
	/* intialize i, j, and temp ch */
	int i = strlen(str) - 1;
	int j = 0;
	char ch;
	/* go thorugh string and swap */
	while (i > j)
	{
		ch = str[i];
		str[i] = str[j];
		str[j] = ch;
		i--;
		j++;
	}
	return str;
}

/* args for send and rec threads */
typedef struct _ThreadArgs
{
	int clisockfd;
} ThreadArgs;

/* receiver thread */
void *thread_main_recv(void *args)
{
	/* detatch, allows process to exit without waiting for this thread */
	pthread_detach(pthread_self());

	/* connection details */
	int sockfd = ((ThreadArgs *)args)->clisockfd;
	free(args);

	// keep receiving and displaying message from server
	char buffer[512];
	int n;

	/* always receive */
	do
	{
		/* reset buffer, and read in data, check for error */
		memset(buffer, 0, 512);
		n = recv(sockfd, buffer, 512, 0);
		if (n < 0)
		{
			error("ERROR recv() failed");
		}

		/* extract color from the buffer, first 2 chars */
		char color[3];
		strncpy(color, buffer, 2);

		/* if the message is from the server give it normal color */
		if (strcmp(color, "12") == 0)
		{
			printf("\n%s%s\n", colors[atoi(color)], &buffer[2]);

			/* only case where the client must be killed based on something it receives
				which is when the desired room cannot be made */
			if (!strcmp(&buffer[2], "Cannot Create New Room"))
			{
				exit(0);
			}
		}

		/* else print the message along with the offset color */
		else
		{
			printf("\n%s%s\n", colors[(atoi(color) + randomOffset) % 12], &buffer[2]);
		}

		/* reset color so the users typing is always the same gray/white */
		printf("\033[0m \n");

	} while (n > 0);

	return NULL;
}

/* send thread */
void *thread_main_send(void *args)
{
	// pthread_detach(pthread_self()); // process must wait for the sending to conclude

	/* set up connection */
	int sockfd = ((ThreadArgs *)args)->clisockfd;
	free(args);

	/* keep sending messages to the server */
	char buffer[256];
	int n;
	/* details for username */
	int hasName = 0;
	char *name;

	/* always send messages */
	while (1)
	{
		/** You will need a bit of control on your terminal
		   console or GUI to have a nice input window. */

		// printf("\nPlease enter the message: ");

		/* first time the user will have no name, prompt for one */
		if (hasName == 0)
		{
			printf("\nPlease enter a username with a max of 15 characters:\n");
		}

		/* clear buffer and read from terminal */
		memset(buffer, 0, 256);
		fgets(buffer, 256, stdin);

		/* if the input was too long > 255, clear stdin so there is no overflow into next message */
		if (strlen(buffer) == 255)
		{
			while (getchar() != '\n')
				;
		}

		/* if the string was empty, put a null at the start */
		if (strlen(buffer) == 1)
		{
			buffer[0] = '\0';
		}

		/* replace newline at the end of the buffer with null */
		if (buffer[strlen(buffer) - 1] == 10)
		{
			buffer[strlen(buffer) - 1] = '\0';
		}

		/* first time we need to make sure the name is formatted well */
		if (hasName == 0)
		{
			/* no empty names */
			if (!strcmp(buffer, ""))
			{
				strcpy(buffer, "Anon");
			}

			else if (buffer[0] == '0' || buffer[0] == '1' || buffer[0] == '2' || buffer[0] == '3' || buffer[0] == '4' || buffer[0] == '5' || buffer[0] == '6' || buffer[0] == '7' || buffer[0] == '8' || buffer[0] == '9') /* no leading numbers in names */
			{
				/* pre-pend the name that starts with a number with #*/
				strrev(strcat(strrev(buffer), "#"));
			}

			/* make sure this doesn't run again since we just assigned the name */
			hasName = 1;
			name = (char *)malloc(strlen(buffer) + 1);
			buffer[16] = '\0';

			/* store the name */
			strcpy(name, buffer);

			/*prepend the name with the desired room number */
			strrev(strcat(strrev(buffer), roomNum));
		}

		/* make sure the buffer terminates properly, then send data */
		buffer[255] = '\0';
		n = send(sockfd, buffer, strlen(buffer), 0);

		if (n < 0)
			error("ERROR writing to socket");

		if (n == 0)
			break; // we stop transmission when user type empty string
	}

	free(name);
	return NULL;
}

int main(int argc, char *argv[])
{
	/* seed for random colors */
	srand(time(NULL));
	randomOffset = rand();

	/* check args */
	if (argc < 2)
		error("Please speicify hostname");

	/* set up socket */
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0)
	{
		error("ERROR opening socket");
	}

	struct sockaddr_in serv_addr;
	socklen_t slen = sizeof(serv_addr);
	memset((char *)&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(PORT_NUM);

	/** check the desired room, make sure it is 1-8 or new (0),
		if no room specified, set it to 9, the server will
		handle prompting user for desired room later,
		if invalid room request, kill the process */

	if (argc == 2)
	{
		roomNum = "9";
	}

	else if (!strcmp("new", argv[2]))
	{
		roomNum = "0";
	}

	else if ((atoi(argv[2]) < 9 && atoi(argv[2]) > 0))
	{
		roomNum = argv[2];
	}

	else
	{
		printf("INVAID Room Number (1-8 or \"new\")\n");
		exit(1);
	}

	/* try connect */
	printf("Try connecting to %s...\n", inet_ntoa(serv_addr.sin_addr));

	int status = connect(sockfd, (struct sockaddr *)&serv_addr, slen);

	if (status < 0)
	{
		error("ERROR connecting");
	}

	pthread_t tid1;
	pthread_t tid2;

	/* launch the threads for sending and receiving */

	ThreadArgs *args;

	args = (ThreadArgs *)malloc(sizeof(ThreadArgs));
	args->clisockfd = sockfd;
	pthread_create(&tid1, NULL, thread_main_send, (void *)args);

	args = (ThreadArgs *)malloc(sizeof(ThreadArgs));
	args->clisockfd = sockfd;
	pthread_create(&tid2, NULL, thread_main_recv, (void *)args);

	// parent will wait for sender to finish (= user stop sending message and disconnect from server)
	pthread_join(tid1, NULL);

	close(sockfd);

	return 0;
}
