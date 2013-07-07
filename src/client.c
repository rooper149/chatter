#include <arpa/inet.h> //#include <err.h>
#include <pthread.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <time.h>

#include "client.h"

extern pthread_mutex_t IO_mutex;
extern pthread_mutex_t SERV_mutex;

void *client_start(void *args)
{
	struct Client_arg *data = (struct Client_arg *) args;

	struct sockaddr_in SockAddr;
	int SocketFD;
	int Res;

	pthread_mutex_lock(&IO_mutex);
	printf("Connecting to %s:%d\n", data->peerIP, data->peerPort);
	pthread_mutex_unlock(&IO_mutex);

	/* Sleep for a second to allow the server to initialize */
	sleep(1);

	SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (SocketFD == -1) {
		perror("can not create a new socket.");
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

	int i = 0;
	char buf[100];
	char *info = "test\n";
	while (i++ < 2) {
		/* Lock the buffer mutex to prevent writing to it */
		strncpy(buf, info, sizeof(buf));
		send(SocketFD, buf, sizeof(buf), 0);
		/* unlock the buffer mutex */
	}

	/* Terminate the connection XXX */
	send(SocketFD, "/exit", 6, MSG_EOR);

	shutdown(SocketFD, SHUT_RDWR);
	close(SocketFD);

	pthread_mutex_lock(&IO_mutex);
	printf("Client finished\n");
	pthread_mutex_unlock(&IO_mutex);
	pthread_exit(NULL);
	return NULL;
}
