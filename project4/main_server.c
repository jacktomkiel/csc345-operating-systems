#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#define PORT_NUM 1004

/* codes for the different color options */
char *colors[12] = {"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11"};

/* print errors */
void error(const char *msg)
{
	perror(msg);
	exit(1);
}

/* user struct, linked list storing all users */
typedef struct _USR
{
	int clisockfd;	   // socket file descriptor
	struct _USR *next; // for linked list queue
	char *uname;	   // username
	char *color;	   // assigned user color
	int roomNum;	   // room the user is in */
} USR;

/* intitialize linked list */
USR *head = NULL;
USR *tail = NULL;

/* thread ids */
pthread_t tid[5];
int TID;

/* string that will be populated with the stats for people in each room,
	and array that holds how many in each room */
char roomReport[256];
int roomArr[8];

/* generate roomReport and fill roomArr */
int reportRooms()
{
	/* track if anyone is here */
	int display = 0;

	/* initialize output string and array */
	char out[256];
	int arr[8];
	memset(arr, 0, sizeof arr);

	/* traverse the whole linked list */
	USR *cur = head;
	while (cur != NULL)
	{
		if (!(cur == tail))
		{
			// the tail is the user we want to place, do not count his invalid room number
			arr[(cur->roomNum - 1)] += 1; // increment arr for each person in the room
		}

		/* next in the linked list */
		cur = cur->next;
	}

	/* start generating message in out */
	sprintf(out, "12Server says following options are available:\n");

	/* loop through all rooms, only report those with someone in there */
	for (int i = 0; i < 8; i++)
	{

		if (arr[i] != 0)
		{
			/* limit the character display to one digit, 9+ if its  >9 */
			if (arr[i] == 1)
			{
				sprintf(out, "%s\t Room %d: %d person\n", out, i + 1, arr[i]);
			}
			else if (arr[i] < 10)
			{
				sprintf(out, "%s\t Room %d: %d people\n", out, i + 1, arr[i]);
			}
			else
			{
				sprintf(out, "%s\t Room %d: 9+ people\n", out, i + 1);
			}

			/* indicate that there is some room with someone in it */
			display = 1;
		}
	}

	/* end the message for the user to pick which room to go into  */
	sprintf(out, "%sChoose room or type \"new\" to make a new room:\n", out);

	/** make sure the output is properly terminated and copy out and arr to
		roomReport and roomArr, return if there is some room that is occupied */
	out[256] = '\0';
	strcpy(roomReport, out);
	memcpy(roomArr, arr, 8 * sizeof(int));

	return display;
}

/* return the lowest number room that is not occupied */
int findRoom()
{
	/* arr for tracking if a room is occupied */
	int arr[8];
	memset(arr, 0, sizeof arr);

	/* traverse the linked list of users */
	USR *cur = head;

	/* go thorugh whole list, don't count tail (we need to place him so he has invalid room rn
		also don't count any other invalid room numbers we find along the way, but that
		should not happen anyway */
	while (cur != NULL)
	{
		if (!(cur == tail) && cur->roomNum < 9 && cur->roomNum > 0)
		{
			arr[(cur->roomNum - 1)] = 1;
		}

		/* go to next link in list */
		cur = cur->next;
	}

	/* check the occupied arr, return the first one with nobody in it */
	for (int i = 0; i < 8; i++)
	{
		if (arr[i] == 0)
		{
			return (i + 1);
		}
	}

	/* if no room unoccupied, poor luck, return -1 to get rejected from server */
	return -1;
}

/* set the color of a user
	sequentially assign color based on user number
	each client randomizes the start point so colors will be random per client
	but consistent for each client */
void setColor(USR *user)
{
	/* track if a color has been used */
	int used = 0;
	int i = 0;

	/* loop through all 12 colors
		if we find an unsued one, we are gonna assign that one */
	for (i = 0; i < 12; i++)
	{
		USR *cur = head;
		used = 0;

		while (cur != NULL)
		{
			if (cur->color == colors[i])
			{
				used = 1;
			}
			cur = cur->next;
		}

		if (used == 0)
		{
			break; /* stop searching if we find a color that is unused */
		}
	}

	/* if we found an unused color, simply assign it */
	if (used == 0)
	{
		user->color = colors[i];
	}

	else /* did not find color, all colors are used, assign a random one */
	{
		srand(time(NULL));
		user->color = colors[rand() % 12];
	}
}

/* prints the clients name and room number to server console */
void printClients()
{
	printf("\nCurrent Clients Connected:");

	/* no head = nobody is connected at the moment */
	if (head == NULL)
	{
		printf("\nNo clients connected :(\n\n");
		return;
	}

	/* traverse the linked list, print the username and room num for each client */
	USR *cur = head;
	while (cur != NULL)
	{
		printf("\n%s - %d", cur->uname, cur->roomNum);

		cur = cur->next;
	}

	printf("\n\n");
}

/* add tail to linked list, dependent on the current status of the LL
	empty -  new head and tail
	not empty - new tail, old tail->next = new tail */
void add_tail(int newclisockfd)
{
	if (head == NULL)
	{
		head = (USR *)malloc(sizeof(USR));
		head->clisockfd = newclisockfd;
		head->next = NULL;
		tail = head;
	}

	else
	{
		tail->next = (USR *)malloc(sizeof(USR));
		tail->next->clisockfd = newclisockfd;
		tail->next->next = NULL;
		tail = tail->next;
	}
}

/* broadcast the message, who it goes to depends on the message and the room */
void broadcast(int fromfd, char *message, int c)
{
	// figure out sender address
	struct sockaddr_in cliaddr;
	socklen_t clen = sizeof(cliaddr);
	if (getpeername(fromfd, (struct sockaddr *)&cliaddr, &clen) < 0)
		error("ERROR Unknown sender!");

	/* find out who sent the message */
	USR *curS = head;
	USR *prev = head;
	int tfd;

	while (curS != NULL)
	{
		tfd = curS->clisockfd;

		if (tfd == fromfd)
		{
			break;
		}

		if (curS != head)
		{
			prev = prev->next;
		}

		curS = curS->next;
	}

	/* special server message, no more rooms to be made, send keyword to reject and kill client */
	if (c == 2)
	{
		char buffer[512];
		sprintf(buffer, "12Cannot Create New Room");
		int nmsg = strlen(buffer);
		int nsen = send(curS->clisockfd, buffer, nmsg, 0);
		return;
	}

	else if (c == 3) /* special server message, sending the status of all rooms */
	{
		char buffer[512];
		sprintf(buffer, "%s", message);
		int nmsg = strlen(buffer);
		int nsen = send(curS->clisockfd, buffer, nmsg, 0);
		return;
	}

	// traverse through all connected clients
	USR *cur = head;
	while (cur != NULL)
	{
		// check if cur is not the one who sent the message
		/* also only send to those in same room */
		if (cur->clisockfd != fromfd && cur->roomNum == curS->roomNum)
		{
			char buffer[512];

			// prepare message
			if (c == 1) /* server announcement to room */
				sprintf(buffer, "12%s (%s) %s", curS->uname, inet_ntoa(cliaddr.sin_addr), message);
			else /* client message to other clients */
				sprintf(buffer, "%s[%s (%s)]: %s", curS->color, curS->uname, inet_ntoa(cliaddr.sin_addr), message);
			int nmsg = strlen(buffer);

			// send!
			int nsen = send(cur->clisockfd, buffer, nmsg, 0);
			if (nsen != nmsg)
				error("ERROR send() failed");
		}

		cur = cur->next;
	}
}

/* thread args struct */
typedef struct _ThreadArgs
{
	int clisockfd;
} ThreadArgs;

/* main thread for each client */
void *thread_main(void *args)
{

	// make sure thread resources are deallocated upon return
	pthread_detach(pthread_self());

	// get socket descriptor from argument
	int clisockfd = ((ThreadArgs *)args)->clisockfd;
	free(args);

	//-------------------------------
	// Now, we receive/send messages
	char buffer[256];
	int nsen, nrcv;

	/* track current user in linked list, and the user behind */
	USR *cur = head;
	USR *prev = head;
	int tfd;
	while (cur != NULL)
	{
		tfd = cur->clisockfd;
		if (tfd == clisockfd)
		{
			break;
		}
		if (cur != head)
		{
			prev = prev->next;
		}
		cur = cur->next;
	}

	/*prep buffer and receive first message, this will hold the username and room */
	memset(buffer, 0, 256);
	nrcv = recv(clisockfd, buffer, 255, 0);
	if (nrcv < 0)
		error("ERROR recv() failed");

	/* copy name, and room num, first char is room num, rest is name */
	cur->uname = (char *)malloc(strlen(buffer) + 1);
	strcpy(cur->uname, &buffer[1]);
	cur->roomNum = atoi(&buffer[0]);

	/* check if the room number must be assigned (0) */
	if (cur->roomNum == 0)
	{

		/* find the next open room */
		int newRoom = findRoom();

		/* no open rooms, tell the client to kill themselves, remove them from the LL */
		if (newRoom == -1)
		{

			broadcast(clisockfd, "", 2);
			/* only user here, null head and tail */
			if (cur == head && cur == tail)
			{
				head = NULL;
				tail = NULL;
			}
			else if (cur == head) /* user the head, set my next guy as head */
			{
				head = cur->next;
			}
			else if (cur == tail) /* user the tail, set prev to tail and his next = NULL */
			{
				tail = prev;
				prev->next = NULL;
			}
			else /* user in middle of LL, set my prev's next to my next  */
			{
				prev->next = cur->next;
			}
			return NULL;
		}

		else /* we have a room, assign it */
		{
			cur->roomNum = newRoom;
		}
	}
	/* 9 means that we need to give them options before they select a room */
	if (cur->roomNum == 9)
	{
		/* generate roomReport, and return check */
		int check = reportRooms();

		/* we need to send the report since at least one room is occupied */
		if (check == 1)
		{

			/* broadcast the roomReport to the user who needs it */
			broadcast(clisockfd, roomReport, 3);

			/* receive the users response */
			memset(buffer, 0, 256);
			nrcv = recv(clisockfd, buffer, 255, 0);

			if (nrcv < 0)
			{
				error("ERROR recv() failed");
			}

			/* if they want a new room */
			if (!strcmp(buffer, "new"))
			{
				/* check if a room is available */
				int newRoom = findRoom();

				/* no room avilable, reject user and reorganize LL as we did earlier */
				if (newRoom == -1)
				{

					broadcast(clisockfd, "", 2);

					if (cur == head && cur == tail)
					{
						head = NULL;
						tail = NULL;
					}
					else if (cur == head)
					{
						head = cur->next;
					}
					else if (cur == tail)
					{
						tail = prev;
						prev->next = NULL;
					}
					else
					{
						prev->next = cur->next;
					}
					return NULL;
				}
				else /* we can accomodate a new room */
				{
					cur->roomNum = newRoom;
				}
			}

			else /* not new was requested */
			{

				/* get the requested number, must be in 1-8*/
				cur->roomNum = atoi(&buffer[0]);

				/* request not in range or room is empty,
					reject user and reorganize LL as we did earlier */
				if (cur->roomNum < 1 || cur->roomNum > 8 || roomArr[cur->roomNum - 1] == 0)
				{
					broadcast(clisockfd, "", 2);

					if (cur == head && cur == tail)
					{
						head = NULL;
						tail = NULL;
					}
					else if (cur == head)
					{
						head = cur->next;
					}
					else if (cur == tail)
					{
						tail = prev;
						prev->next = NULL;
					}
					else
					{
						prev->next = cur->next;
					}
					return NULL;
				}
			}
		}

		else /* nobody is here, auto call for "new" room */
		{
			/* search for empty room, ablige if possible, if not reject as usual */
			int newRoom = findRoom();

			if (newRoom == -1)
			{
				broadcast(clisockfd, "", 2);
				if (cur == head && cur == tail)
				{
					head = NULL;
					tail = NULL;
				}
				else if (cur == head)
				{
					head = cur->next;
				}
				else if (cur == tail)
				{
					tail = prev;
					prev->next = NULL;
				}
				else
				{
					prev->next = cur->next;
				}
				return NULL;
			}
			else
				cur->roomNum = newRoom;
		}
	}

	/* print username and room that just entered to console */
	printf("%s to room %d\n", cur->uname, cur->roomNum);

	/* print cients to console */
	printClients();

	/* broadcast to room that a client has connected */
	broadcast(clisockfd, "has connected!!", 1);

	/* get first message from this user */
	memset(buffer, 0, 256);
	nrcv = recv(clisockfd, buffer, 255, 0);
	if (nrcv < 0)
	{
		error("ERROR recv() failed");
	}

	/* keep broadcasting messages to room as long as its not empty */
	while (nrcv > 0)
	{
		// we send the message to everyone except the sender
		broadcast(clisockfd, buffer, 0);

		/* clear buffer and receive next message */
		memset(buffer, 0, 256);
		nrcv = recv(clisockfd, buffer, 255, 0);

		if (nrcv < 0)
		{
			error("ERROR recv() failed");
		}

		/* break once the input is empty */
		if (nrcv == 0)
		{
			break;
		}
	}

	/* refind me and the guy before me since conditions could have changed since I last checked */
	cur = head;
	prev = head;

	while (cur != NULL)
	{
		tfd = cur->clisockfd;
		if (tfd == clisockfd)
		{
			break;
		}
		if (cur != head)
		{
			prev = prev->next;
		}
		cur = cur->next;
	}

	/* print uname and room num of guy who just left */
	printf("Disconnected: %s from room %d\n", cur->uname, cur->roomNum);

	/* broadcast disconnect message to whole room */
	broadcast(clisockfd, "has disconnected!!", 1);

	/* update LL, need special case for head and tail user, like before */
	if (cur == head)
	{

		head = cur->next;
	}
	else if (cur == tail)
	{

		prev->next = NULL;
		tail = prev;
	}
	else
	{

		prev->next = cur->next;
	}

	/* free cur var */
	free(cur);

	/* print the current client list to the terminal server */
	printClients();

	/* close socket for this client */
	close(clisockfd);

	return NULL;
}

/* main */
int main(int argc, char *argv[])
{
	/* set up socket stuff */
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");

	struct sockaddr_in serv_addr;
	socklen_t slen = sizeof(serv_addr);
	memset((char *)&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	// serv_addr.sin_addr.s_addr = inet_addr("192.168.1.171");
	serv_addr.sin_port = htons(PORT_NUM);

	int status = bind(sockfd, (struct sockaddr *)&serv_addr, slen);
	if (status < 0)
		error("ERROR on binding");

	listen(sockfd, 5); // maximum number of connections = 5

	/* always print at the start when empty */
	printClients();

	/* thread ID */
	TID = 0;

	while (1)
	{ /* always allow clients to connect */
		/* set up socket info for next client */
		struct sockaddr_in cli_addr;
		socklen_t clen = sizeof(cli_addr);
		int newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clen);

		if (newsockfd < 0)
		{
			error("ERROR on accept");
		}

		/* when new guy connects, begin the terminal output indiciating that */
		printf("Connected: %s with name: ", inet_ntoa(cli_addr.sin_addr));
		add_tail(newsockfd); // add this new client to the client list

		/* set the color of the new tail user we just added */
		setColor(tail);

		// prepare ThreadArgs structure to pass client socket
		ThreadArgs *args = (ThreadArgs *)malloc(sizeof(ThreadArgs));
		if (args == NULL)
			error("ERROR creating thread argument");

		args->clisockfd = newsockfd;

		if (pthread_create(&tid[TID], NULL, thread_main, (void *)args) != 0)
		{
			error("ERROR creating a new thread");
		}

		/* increment TID for next thread */
		TID++;
	}

	return 0;
}
