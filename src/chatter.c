#define _GNU_SOURCE /* readline(3) */
#include <err.h>
#include <getopt.h>
#include <locale.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "server.h"
#include "client.h"

#define BUFLENGTH 100

pthread_mutex_t IO_mutex;
pthread_mutex_t BUF_mutex;

char *line;

int main(int argc, char **argv)
{
	setlocale(LC_ALL, "");

	/* Choosing sane default values */
	char peerIP[32] = "127.0.0.1";
	int listenPort = 1025, peerPort = 1025;

	pthread_attr_t attr;
	pthread_t ClientThread, ServerThread;
	struct Server_arg ServerArg;
	struct Client_arg ClientArg;

	/* Parse command line options */
	int opt = 0;
	while ((opt = getopt(argc, argv, "h:l:p:")) != -1) {
		switch (opt)
		{
			case 'h': /* Host (Peer IP) */
				strncpy(peerIP, optarg, sizeof(peerIP));
				break;
			case 'l': /* Local listen port */
				/* XXX Make this more robust */
				listenPort = atoi(optarg);
				break;
			case 'p': /* Host port */
				/* XXX Make this more robust */
				peerPort = atoi(optarg);
				break;
			default:
				exit(EXIT_FAILURE);
		}
	}
	argc -= optind;
	argv += optind;

	/* Setup the Server properties */
	ServerArg.listen = listenPort;

	/* Setup the Client properties */
	ClientArg.peerPort = peerPort;
	strncpy(ClientArg.peerIP, peerIP, sizeof(peerIP));

	/* Create a thread attribute to detach threads on creation */
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	/* Create and call the threads */
	pthread_create(&ServerThread, &attr, server_start, (void *)&ServerArg);
	pthread_create(&ClientThread, &attr, client_start, (void *)&ClientArg);

	pthread_attr_destroy(&attr);

	line = malloc(sizeof(char) * BUFLENGTH);
	size_t len = BUFLENGTH;
	while (strstr(line, "/exit") != line) {
		pthread_mutex_lock(&BUF_mutex);
			printf(": ");
			getline(&line, &len, stdin);
		pthread_mutex_unlock(&BUF_mutex);
	}

	free(line);
	return (0);
}
