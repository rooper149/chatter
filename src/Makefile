## src/Makefile

SHELL=		/bin/sh
INSTALL=	/usr/local/bin
PROG=		chatter

SRC=		chatter.c client.c server.c #cmd.c
OBJ=		$(SRC:.c=.o)
VERSION=	0.0.1
OPTS=		-DVERSION=\"$(VERSION)\" #-DNDEBUG 

CC =		gcc
CFLAGS=		-std=c99 -pedantic-errors -g -Wall -Werror -Og
LFLAGS=		
LDFLAGS=	-lpthread #-lcrypto

.SUFFIXES: .c .o

all: $(PROG)

$(PROG): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LFLAGS) $(LDFLAGS)

.c.o:
	@echo -ne \\t Compiling $< ..
	@$(CC) -c $(OPTS) $(CFLAGS) $<
	@echo " done."

install:
	@echo -ne \\t Installing executable to $(INSTALL) ..
	@cp $(PROG) $(INSTALL)
	@echo " done."

clean:
	@echo -ne \\t Removing object files
	@rm -f $(PROG) *.o $(.CURDIR)/*.BAK 
	@echo " done."

.PHONY: clean all
