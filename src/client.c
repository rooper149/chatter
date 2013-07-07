#include <arpa/inet.h> //#include <err.h>
#include <pthread.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

#define BUFLENGTH 100

#include "client.h"

extern pthread_mutex_t IO_mutex;
extern pthread_mutex_t BUF_mutex;

extern char *line;

void *client_start(void *args)
{
	struct Client_arg *data = (struct Client_arg *) args;

	struct sockaddr_in SockAddr;
	int SocketFD;
	int Res;

	/* Consider making this idiom a MACRO */
	pthread_mutex_lock(&IO_mutex);
		printf("Connecting to %s:%d\n", data->peerIP, data->peerPort);
	pthread_mutex_unlock(&IO_mutex);

	/* TODO Make this more robust */
	/* Can I have this thread halt until a certain trigger point later in the
	 * code executes in another thread? Mutexes are a possibility, but they're
	 * ugly and not intended for that. Perhaps Conditional variables?
	 */
	/* XXX Sleep for a second to allow the server to initialize */
	sleep(1);

	SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (SocketFD == -1) {
		perror("Can't create a socket");
		exit(EXIT_FAILURE);
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
		exit(EXIT_FAILURE);
	}

	if (connect(SocketFD, (struct sockaddr *)&SockAddr,
				sizeof(struct sockaddr_in)) == -1) {
		perror("Failed to connect");
		close(SocketFD);
		exit(EXIT_FAILURE);
	}

	/* This loop will terminate when the '/exit' command is recieved from the
	 * user, it will close the connection with its peer.
	 */
	while (strcmp(line, "/exit")) {
		pthread_mutex_lock(&BUF_mutex);
			send(SocketFD, line, BUFLENGTH, 0);
		pthread_mutex_unlock(&BUF_mutex);
	}

	/* Terminate the connection with our peer*/
	shutdown(SocketFD, SHUT_RDWR);
	close(SocketFD);

	pthread_mutex_lock(&IO_mutex);
		printf("Client finished\n");
	pthread_mutex_unlock(&IO_mutex);

	pthread_exit(NULL);
	return NULL;
}
