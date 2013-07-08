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
extern pthread_mutex_t BUF_mutex;

void *server_start(void *args)
{
	struct Server_arg *data = (struct Server_arg *) args;
	struct sockaddr_in SockAddr;
	int SocketFD;

	pthread_mutex_lock(&IO_mutex);
		printf("Server listen Port: %d\n", data->listen);
	pthread_mutex_unlock(&IO_mutex);

	SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (SocketFD == -1) {
		perror("Can't create a new socket.");
		exit(EXIT_FAILURE);
	}

	memset(&SockAddr, 0, sizeof(struct sockaddr_in));

	SockAddr.sin_family = AF_INET;
	SockAddr.sin_port = htons(data->listen);
	SockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(SocketFD, (struct sockaddr *)&SockAddr, sizeof(struct sockaddr_in)) == -1) {
		perror("Bind failed");
		close(SocketFD);
		exit(EXIT_FAILURE);
	}

	if (listen(SocketFD, 10) == -1) {
		perror("Listen failed");
		close(SocketFD);
		exit(EXIT_FAILURE);
	}

	int ConnectFD = accept(SocketFD, NULL, NULL);

	if (ConnectFD < 0) {
		perror("Accept failed");
		close(SocketFD);
		exit(EXIT_FAILURE);
	}

	char netBuf[BUFLENGTH];
	while (sleep(1), strstr(netBuf, TERMSTRING) == NULL) {
		pthread_mutex_lock(&IO_mutex);
			recv(ConnectFD, (void *)netBuf, BUFLENGTH, 0);
			fprintf(stderr, "< %s", netBuf);
		pthread_mutex_unlock(&IO_mutex);
	}

	shutdown(ConnectFD, SHUT_RDWR);
	close(ConnectFD);

	close(SocketFD);

	pthread_exit(NULL);
	return NULL;
}
