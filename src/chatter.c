#define _GNU_SOURCE /* getline(3) */
//#include <err.h>
//#include <errno.h>
#include <getopt.h>
#include <locale.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>
#include <unistd.h>

#include "server.h"
#include "client.h"
#include "queue.h"
#include "def.h"

pthread_mutex_t IO_mutex;
pthread_mutex_t QUEUE_mutex;

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

	/* Create the message queue */
	LIST_INIT(&head);
	n1 = malloc(sizeof *n1);
	LIST_INSERT_HEAD(&head, n1, entries);

	/* Setup the Server properties */
	ServerArg.listen = listenPort;

	/* Setup the Client properties */
	ClientArg.peerPort = peerPort;
	strncpy(ClientArg.peerIP, peerIP, sizeof(peerIP));

	/* Create a thread attribute to detach threads on creation */
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	/* Create and call the threads */
	pthread_create(&ServerThread, &attr, server_start, (void *)&ServerArg);
	pthread_create(&ClientThread, &attr, client_start, (void *)&ClientArg);

	pthread_attr_destroy(&attr);

	sleep(1);

	char *line = malloc(sizeof(char) * BUFLENGTH);
	size_t len = BUFLENGTH;
	while (strstr(line, TERMSTRING) == NULL) {
		pthread_mutex_lock(&IO_mutex);
			printf("> ");
		pthread_mutex_unlock(&IO_mutex);

		getline(&line, &len, stdin);
		line[len] = '\0';

		pthread_mutex_lock(&QUEUE_mutex);
			n2 = malloc(sizeof *n2);
			strncpy(n2->msg, line, BUFLENGTH);
			LIST_INSERT_HEAD(&head, n2, entries);
		pthread_mutex_unlock(&QUEUE_mutex);
	}

	/* Allow for the threads to properly kill themselves */
	/* .. It's only polite, afterall. */
	pthread_join(ClientThread, NULL);
	pthread_join(ServerThread, NULL);

	free(n1);
	free(n2);
	free(line);

	return (0);
}
