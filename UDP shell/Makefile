CC ?= gcc
CFLAGS ?= -Wall -Werror
RM ?= rm
all: client server
server: udp_server.c erproc.c
	$(CC) $(CFLAGS) udp_server.c erproc.c -o server
client: udp_client.c erproc.c
	$(CC) $(CFLAGS) udp_client.c erproc.c -o client
clean:
	$(RM) client server
