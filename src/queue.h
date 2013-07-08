#ifndef QUEUE_H_
#define QUEUE_H

#include <sys/queue.h>
#include "def.h"

LIST_HEAD(listhead, MSG_queue) head;

struct MSG_queue {
	char msg[BUFLENGTH];
	LIST_ENTRY(MSG_queue) entries;
} *n1, *n2;

#endif
