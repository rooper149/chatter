#include <arpa/inet.h>
//#include <err.h>
#include <pthread.h>
#include <netinet/in.h>
#include <netdb.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
//#include <unistd.h>

#include "server.h"

void *server_start(void *data)
{
	pthread_exit(NULL);
	return NULL;
}
