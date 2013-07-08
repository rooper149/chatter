#ifndef SERVER_H_
#define SERVER_H_

struct Server_arg {
	int listen;
};

void *
server_start(void *);

#endif
