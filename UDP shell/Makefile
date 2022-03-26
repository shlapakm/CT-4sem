CC ?= gcc
CFLAGS ?= -Wall -Werror
RM ?= rm
all: client server
server: udp_server.c erproc.c erproc.h
	$(CC) $(CFLAGS) udp_server.c erproc.c erproc.h -o server
client: udp_client.c erproc.c erproc.h
	$(CC) $(CFLAGS) udp_client.c erproc.c erproc.h -o client
clean:
	$(RM) client server
