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
#include <sys/stat.h>
#include <fcntl.h>

int main(){
    int fds;
    int server = socket(AF_INET, SOCK_DGRAM,0);
    struct sockaddr_in adr = {0};
    adr.sin_family = AF_INET;
    adr.sin_port =  htons(34543);
    Inet_pton(AF_INET,"127.0.0.1", &adr.sin_addr);
    Bind(server, (struct sockaddr *) &adr, sizeof adr);
    printf("server: waiting for connections...\n");
    socklen_t adrlen = sizeof adr;
    char str[255];
    FILE *fd3;
    FILE *fd2;
    FILE *fd4;
    int fd1;
    char cc[512];
    fd1= open("out.txt", O_CREAT|O_WRONLY);
    int fd = recvfrom(server, str,255,0, (struct sockaddr *) &adr,&adrlen);
    dup2(fd1,1);
    system(str);
    int n=0;
    fd2= fopen("out.txt", "r");
    while((fgets(cc, 512, fd2))!=NULL){
        n+=1;
    }
    char num[1024];
    printf("numbers: %d", n);
    sprintf(num,"%d",n);
    int numbytes1 = sendto(server, num, strlen(num), 0, (struct sockaddr *) &adr, sizeof adr);
    close(fd1);
    fclose(fd2);
    fd3 = fopen("out.txt", "r");
    while((fgets(cc, 256, fd3))!=NULL){
        int numbytes = sendto(server, cc, strlen(cc), 0, (struct sockaddr *) &adr, sizeof adr);
    }
    fd4= fopen("out.txt", "w");
    fclose(fd4);
    sleep(150);
    close(server);
    return 0;
}
