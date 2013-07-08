#include <arpa/inet.h>
//#include <err.h>
//#include <errno.h>
#include <pthread.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "server.h"
#include "def.h"

extern pthread_mutex_t IO_mutex;

extern pthread_cond_t NETWORK_cond;

void *
server_start(void *args)
{
	struct Server_arg *data = (struct Server_arg *) args;
	struct sockaddr_in SockAddr;
	int SocketFD;
	char *exitSTR = TERMSTRING;

	pthread_mutex_lock(&IO_mutex);
		printf("Server listen Port: %d\n", data->listen);
	pthread_mutex_unlock(&IO_mutex);

	SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (SocketFD == -1) {
		perror("Can't create a new socket.");
		pthread_exit(NULL);
	}

	memset(&SockAddr, 0, sizeof(struct sockaddr_in));

	SockAddr.sin_family = AF_INET;
	SockAddr.sin_port = htons(data->listen);
	SockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(SocketFD, (struct sockaddr *)&SockAddr,
				sizeof(struct sockaddr_in)) == -1) {
		perror("Bind failed");
		close(SocketFD);
		pthread_exit(NULL);
	}

	if (listen(SocketFD, 10) == -1) {
		perror("Listen failed");
		close(SocketFD);
		pthread_exit(NULL);
	}

	/* Server is finished initializing, tell the client it can connect */
	while (pthread_cond_signal(&NETWORK_cond))
		/* Waiting for client to catch up */
		;

	/* TODO Interesting things planned with this while loop, be patient */
#if 0
	while (1){
#endif
		/* Wait for a connection */
		socklen_t ConLen;
		struct sockaddr ConAddr;
		int ConnectFD = accept(SocketFD, &ConAddr, &ConLen);
		if (ConnectFD < 0) {
			perror("Accept failed");
			close(SocketFD);
			pthread_exit(NULL);
		}

		/* Get the IP of the connecting peer */
		char peerIP[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, ConAddr.sa_data, peerIP, INET_ADDRSTRLEN);
	
		printf("Recieved connection from user: %s\n", peerIP);

		char netBuf[BUFLENGTH];
		while (sleep(1), strstr(netBuf, exitSTR) == NULL) {
			pthread_mutex_lock(&IO_mutex);
				recv(ConnectFD, (void *)netBuf, BUFLENGTH, 0);
				fprintf(stderr, "%s> %s", peerIP, netBuf);
			pthread_mutex_unlock(&IO_mutex);
		}

		shutdown(ConnectFD, SHUT_RDWR);
		close(ConnectFD);
#if 0
	}
#endif

	close(SocketFD);

	pthread_exit(NULL);
	return NULL;
}
