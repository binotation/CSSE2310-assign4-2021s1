CC=gcc
CFLAGS=-Wall -pedantic -std=gnu99 -pthread

.PHONY: clientd, server, serverd, all, alld
.PHONY: $(shell mkdir -p obj) client
.DEFAULT_GOAL:=all

alld: CFLAGS+= -g

serverd: CFLAGS+= -g

clientd: CFLAGS+= -g

all: server client

obj/%.o: %.c
	$(CC) -c $^ -o $@

client: obj/client.o obj/util.o
	$(CC) $(CFLAGS) obj/util.o obj/client.o -o client

server: server.o util.o list.o
	$(CC) $(CFLAGS) util.o list.o server.o -o server

serverd: server

clientd: client

alld: all

clean:
	rm -f client server obj/*.o
