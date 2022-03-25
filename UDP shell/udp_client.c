#include <sys/types.h>
#include <sys/socket.h>
# include <sys/select.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "erproc.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#define _XOPEN_SOURCE 600
#include <fcntl.h>
#define __USE_BSD
#include <termios.h>
int main() {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in adr = {0};
    adr.sin_family = AF_INET;
    adr.sin_port = htons(34543);
    Inet_pton(AF_INET,"127.0.0.1", &adr.sin_addr);
    int broadcast=1;
    if (setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof broadcast) == -1) { 
        perror("setsockopt (SO_BROADCAST)"); 
        exit(1); 
    }
    char str[255];
    char *pstr;
    socklen_t adrlen = sizeof adr;
    pstr = fgets(str, 255, stdin);
    int numbytes = sendto(fd, str, strlen(str), 0, (struct sockaddr *) &adr, sizeof adr);
    char num[10240];
    int server1 = recvfrom(fd, num, 10240, 0, (struct sockaddr *) &adr,&adrlen);
    int n = atoi(num);
    int i = 0;
    printf("numbers of string: %d\n", n);
    while (i <= n){
        int server = recvfrom(fd, str,255,0, (struct sockaddr *) &adr,&adrlen);
        printf("%s",str);
        i+=1;
        memset(&str, 0, sizeof(str));
    }
    sleep(100);
    close(fd);
    return 0;
}