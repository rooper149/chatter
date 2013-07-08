#ifndef CLIENT_H_
#define CLIENT_H_

struct Client_arg {
	char peerIP[32];
	int peerPort;
};

void *
client_start(void *);

#endif
