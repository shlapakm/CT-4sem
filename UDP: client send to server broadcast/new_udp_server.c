#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "erproc.h"
#include <string.h>
int main(){
    int server = socket(AF_INET, SOCK_DGRAM,0);
    struct sockaddr_in adr = {0};
    adr.sin_family = AF_INET;
    adr.sin_port =  htons(34543);
    Bind(server, (struct sockaddr *) &adr, sizeof adr);
    //Listen(server, 5);
    printf("server: waiting for connections...\n");
    socklen_t adrlen = sizeof adr;
    char str[255];
    while (1){
        int fd = recvfrom(server, str,255,0, (struct sockaddr *) &adr,&adrlen);
        printf("server: received '%s'",str);
        memset(&str, 0, sizeof(str));
    }
    //write(STDOUT_FILENO, "thanks", 7);
    sleep(150);
    //close(fd);
    close(server);
    return 0;
}