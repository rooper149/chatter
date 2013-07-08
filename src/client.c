#include <arpa/inet.h>
//#include <err.h>
//#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/queue.h>
#include <unistd.h>

#include "client.h"
#include "def.h"
#include "queue.h"

extern pthread_mutex_t IO_mutex;
extern pthread_mutex_t QUEUE_mutex;
extern pthread_mutex_t NETWORK_mutex;

extern pthread_cond_t NETWORK_cond;

void *
client_start(void *args)
{
	struct Client_arg *data = (struct Client_arg *) args;
	struct sockaddr_in SockAddr;
	int SocketFD;
	int Res;
	char line[BUFLENGTH];
	char *exitSTR = TERMSTRING;

	/* Consider making this idiom a MACRO */
	pthread_mutex_lock(&IO_mutex);
		printf("Connecting to %s:%d\n", data->peerIP, data->peerPort);
	pthread_mutex_unlock(&IO_mutex);

	pthread_cond_wait(&NETWORK_cond, &NETWORK_mutex);

	SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (SocketFD == -1) {
		perror("Can't create a socket");
		pthread_exit(NULL);
	}

	memset(&SockAddr, 0, sizeof (struct sockaddr_in));

	SockAddr.sin_family = AF_INET;
	SockAddr.sin_port = htons(data->peerPort);
	Res = inet_pton(AF_INET, data->peerIP, &SockAddr.sin_addr);

	if (Res < 0) {
		perror("Not a valid address family");
		close(SocketFD);
		exit(EXIT_FAILURE);
	} else if (Res == 0) {
		perror("Not a valid IP address");
		close(SocketFD);
		pthread_exit(NULL);
	}

	/* TODO loop this until we can connect */
	if (connect(SocketFD, (struct sockaddr *)&SockAddr,
				sizeof(struct sockaddr_in)) == -1) {
		perror("Failed to connect");
		close(SocketFD);
		pthread_exit(NULL);
	}

	/* Pop the next message off the list */
	while(sleep(1), strstr(line, exitSTR) == NULL) {
		pthread_mutex_lock(&QUEUE_mutex);
			if (head.lh_first != NULL) {
				strncpy(line, head.lh_first->msg, BUFLENGTH);
				send(SocketFD, line, BUFLENGTH, 0);
				LIST_REMOVE(head.lh_first, entries);
			}
		pthread_mutex_unlock(&QUEUE_mutex);
	}

	/* Terminate the connection with our peer*/
	shutdown(SocketFD, SHUT_RDWR);
	close(SocketFD);

	sleep(2);

	pthread_exit(NULL);
	return NULL;
}
