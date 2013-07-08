#define _POSIX_C_SOURCE 200809L /* getline(3) */
#include <err.h>
#include <errno.h>
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

#if XOPEN_SOURCE >= 600 || _BSD_SOURCE || _SVID_SOURCE || \
					_ISOC99_SOURCE || _POSIX_C_SOURCE >= 200112L
#else
#error This project requires a C99 compiler.
#endif

pthread_mutex_t IO_mutex;
pthread_mutex_t QUEUE_mutex;

pthread_mutex_t NETWORK_mutex;
pthread_cond_t NETWORK_cond;

static pthread_t ClientThread, ServerThread;

static struct Server_arg ServerArg = { .listen = 1025 };
static struct Client_arg ClientArg = {
	.peerIP = "127.0.0.1", .peerPort = 1025
};

static void
usage(void)
{
	extern char *__progname;
	fprintf(stderr, "Usage: %s\n"
			"\t [-v] for versioning information\n"
			"\t [-l] local port to listen on\n"
			"\t [-p] remote port to connect to\n"
		   	"\t [-h] remote host to connect to\n",
			__progname);
}

void
cleanup(void)
{
	pthread_join(ClientThread, NULL);
	pthread_join(ServerThread, NULL);

	/* Cleanup the queue structures */
	free(n1);
	free(n2);

	while (head.lh_first != NULL)
		LIST_REMOVE(head.lh_first, entries);
}

static void *
init(void)
{
	/* Create the message queue */
	LIST_INIT(&head);
	n1 = malloc(sizeof *n1);
	LIST_INSERT_HEAD(&head, n1, entries);

	/* Create a thread attribute to detach threads on creation */
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	/* Create and call the threads */
	pthread_create(&ServerThread, &attr, server_start, (void *)&ServerArg);
	pthread_create(&ClientThread, &attr, client_start, (void *)&ClientArg);

	pthread_attr_destroy(&attr);

	return NULL;
}

static int
chatter(const char *prompt)
{
	char *line = malloc(BUFLENGTH);
	char *exitSTR = TERMSTRING;
	extern int errno;
	size_t len = 0;
	ssize_t read = 0;

	while (strstr(line, exitSTR) == NULL) {
		pthread_mutex_lock(&IO_mutex);
			printf("%s", prompt);
		pthread_mutex_unlock(&IO_mutex);

		errno = 0;
		if ((read = getline(&line, &len, stdin)) == -1) {
			warn("%ld: EOF recieved: %s", (unsigned long) read, strerror(errno));
		} else {
			line[read] = '\0';
		}

		n2 = malloc(sizeof *n2);

		pthread_mutex_lock(&QUEUE_mutex);
			strncpy(n2->msg, line, BUFLENGTH);
			LIST_INSERT_HEAD(&head, n2, entries);
		pthread_mutex_unlock(&QUEUE_mutex);
	}

	free(line);
	return (0);
}

int
main(int argc, char **argv)
{
	extern char *__progname;
	setlocale(LC_ALL, "");
	atexit(cleanup);

	/* Parse command line options */
	int opt = 0;
	while ((opt = getopt(argc, argv, "h:l:np:v")) != -1) {
		switch (opt)
		{
			case 'h': /* Host (Peer IP) */
				strncpy(ClientArg.peerIP, optarg, sizeof(ClientArg.peerIP));
				break;
			case 'l': /* Local listen port */
				ServerArg.listen = strtol(optarg, NULL, 0);
				break;
			case 'p': /* Host port */
				ClientArg.peerPort = strtol(optarg, NULL, 0);
				break;
			case 'v': /* falls through to exit() */
#if defined(VERSION)
				printf("%s: %s\n", __progname, VERSION);
#else
				printf("%s: version number undefined\n", __progname);
#endif
				exit(EXIT_SUCCESS);
				break;
			default: /* Fall through */
				usage();
				exit(EXIT_FAILURE);
		}
	}
	argc -= optind;
	argv += optind;

	init();
	chatter("> ");

	return (0);
}
