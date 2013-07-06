#include <err.h>
#include <getopt.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "server.h"
#include "client.h"

int main(int argc, char **argv)
{
	/* Choosing sane default values */
	char peerIP[32] = "127.0.0.1";
	int listenPort = 1025, peerPort = 1025;
	int c = 0;

	/* Parse command line options */
	while((c = getopt(argc, argv, "l:p:h:")) != -1) {
		switch(c)
		{
			case 'l': /* Listen port */
				listenPort = atoi(optarg);
				break;
			case 'p': /* Communicate port */
				peerPort = atoi(optarg);
				break;
			case 'h': /* Host (Peer IP) */
				strncpy(peerIP, optarg, sizeof(peerIP));
				break;
			default:
				exit(EXIT_FAILURE);
		}
	}
	argc -= optind;
	argv += optind;

	printf("Listening on port %d\n", listenPort);
	printf("Connecting to %s:%d\n", peerIP, peerPort);

	/* Create a thread attribute to detach threads on creation */
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	/* threads for listening to incoming/outgoing messages */
	pthread_t client, server;

	/* XXX Define these functions: client_start, server_start */
	/* XXX Define these structures: client_arg, server_arg */
	pthread_create(&client, &attr, client_start, client_arg);
	pthread_create(&server, &attr, server_start, server_arg);

	/* Destroy the attribute object */
	pthread_attr_destroy(&attr);

	/*
	 * ----------------------------------------------------
	 * Some point in here the program will call the chatter
	 * ----------------------------------------------------
	 */

	return (0);
}
