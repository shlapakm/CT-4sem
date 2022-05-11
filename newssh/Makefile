CC ?= gcc
RM ?= rm
all: client1 server1
server1: sshserver.c  erproc.c erproc.h 
	$(CC)  sshserver.c  erproc.c erproc.h  -lpam -lpam_misc -o server1
client1: sshclient.c erproc.c erproc.h 
	$(CC)  sshclient.c erproc.c erproc.h -lpam -lpam_misc -o client1
clean:
	$(RM) client1 server1
