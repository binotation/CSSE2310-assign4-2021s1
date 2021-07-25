CC=gcc
CFLAGS= -Wall -pedantic -std=gnu99 -pthread
.PHONY: client, clientd, server, serverd, all, alld
.DEFAULT_GOAL := all

alld: CFLAGS+= -g

serverd: CFLAGS+= -g

clientd: CFLAGS+= -g

all: server client

client: client.o helpers.o
	$(CC) $(CFLAGS) helpers.o client.o -o client

server: server.o helpers.o clientList.o
	$(CC) $(CFLAGS) helpers.o clientList.o server.o -o server

serverd: server

clientd: client

alld: all

clean:
	rm -f client server *.o
