## src/Makefile

SHELL=		/bin/sh
INSTALL=	/usr/local/bin
PROG=		chatter

SRC=		chatter.c client.c server.c
OBJ=		$(SRC:.c=.o)
VERSION=	0.0.3
OPTS=		-DVERSION=\"$(VERSION)\" #-DNDEBUG 

CC =		gcc
CFLAGS=		-std=c99 -pedantic-errors -g -Wall -Werror -O2
LFLAGS=		
LDFLAGS=	-lpthread #-lcrypto

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
