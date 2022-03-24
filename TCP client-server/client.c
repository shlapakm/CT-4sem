#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "erproc.h"

int main(){
    int server = socket(AF_INET, SOCK_STREAM,0);
    struct sockaddr_in adr = {0};
    adr.sin_family = AF_INET;
    adr.sin_port =  htons(34543);
    Bind(server, (struct sockaddr *) &adr, sizeof adr);
    Listen(server, 5);
    printf("server: waiting for connections...\n");
    socklen_t adrlen = sizeof adr;
    int fd = Accept(server, (struct sockaddr *) &adr,&adrlen);
    ssize_t nread;
    char buf[256];
    nread = read(fd, buf, 256);
    if (nread == -1){
        perror("read failed");
        exit(EXIT_FAILURE);
    }
    if (nread ==0){
        printf("END OF FILE occured\n");
    }
    printf("server: received '%s'\n",buf);
    //write(STDOUT_FILENO, "thanks", 7);
    write(fd, "thanks", 7);
    //sleep(15);
    //close(fd);
    //close(server);
    return 0;
}

