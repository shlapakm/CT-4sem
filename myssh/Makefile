CC ?= gcc
CFLAGS ?=  -Werror
RM ?= rm
all: client server
server: server.c erproc.c erproc.h
	$(CC) $(CFLAGS)  server.c erproc.c erproc.h -o server
client: client.c erproc.c erproc.h
	$(CC)  $(CFLAGS) client.c erproc.c erproc.h -o client
clean:
	$(RM) client server
