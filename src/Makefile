## src/Makefile

SHELL=		/bin/sh
INSTALL=	/usr/local/bin
PROG=		chatter

SRC=		chatter.c client.c server.c
OBJ=		$(SRC:.c=.o)
VERSION=	v0.0.3
OPTS=		-DVERSION=\"$(VERSION)\" -DCRYPTO #-DNDEBUG 

CC =		gcc
CFLAGS=		-std=c99 -pedantic-errors -g -Wall -Werror -O2 -Wstack-protector -fstack-protector-all
LFLAGS=		
LDFLAGS=	-lpthread #-lssl

.SUFFIXES: .c .o

all: $(PROG)

$(PROG): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LFLAGS) $(LDFLAGS)

.c.o:
	$(CC) -c $(OPTS) $(CFLAGS) $<

install:
	cp $(PROG) $(INSTALL)

clean:
	rm -f $(PROG) *.o

.PHONY: clean all
