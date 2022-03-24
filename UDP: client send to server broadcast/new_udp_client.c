#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "erproc.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
int main() {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in adr = {0};
    adr.sin_family = AF_INET;
    adr.sin_port = htons(34543);
    Inet_pton(AF_INET, "127.0.0.1", &adr.sin_addr);
    //Connect(fd, (struct sockaddr *) &adr, sizeof adr);
    //write(fd, letter[1], strlen(letter[1]));
    //char buf[256];
    //ssize_t nread;
    int broadcast=1;
    if (setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof broadcast) == -1) { 
        perror("setsockopt (SO_BROADCAST)"); 
        exit(1); 
    }
    char str[255];
    char *pstr;
    while (1){
        //printf("Enter your sms\n");
        pstr = fgets(str, 255, stdin);
        int numbytes = sendto(fd, str, strlen(str), 0, (struct sockaddr *) &adr, sizeof adr);
        printf("client: send '%s'\n",str);
        memset(&str, 0, sizeof(str));
    }
    //write(STDOUT_FILENO, buf, nread);
    sleep(100);
    close(fd);
    return 0;
}
