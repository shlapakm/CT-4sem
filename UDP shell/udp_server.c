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
    int server = Socket(AF_INET, SOCK_DGRAM,0);
    struct sockaddr_in adr = {0};
    adr.sin_family = AF_INET;
    adr.sin_port =  htons(34543);
    Inet_pton(AF_INET,"127.0.0.1", &adr.sin_addr);
    Bind(server, (struct sockaddr *) &adr, sizeof adr);
    printf("server: waiting for connections...\n");
    socklen_t adrlen = sizeof adr;
    char command[255];
    FILE *fd2;
    FILE *fd3;
    FILE *fd4;
    int fd1;
    char str[255];
    fd1= open("out.txt", O_CREAT|O_WRONLY);
    int client = recvfrom(server, command,255,0, (struct sockaddr *) &adr,&adrlen);
    dup2(fd1,1);
    dup2(fd1,2);
    system(command);
    int n=0; //number of lines in the file
    fd2= fopen("out.txt", "r");
    while((fgets(str, 255, fd2))!=NULL){
        n+=1;
    }
    char num[1024];
    sprintf(num,"%d",n);
    int numberoflines = sendto(server, num, strlen(num), 0, (struct sockaddr *) &adr, sizeof adr);
    close(fd1);
    fclose(fd2);
    fd3 = fopen("out.txt", "r");
    while((fgets(str, 256, fd3))!=NULL){
        int sendstr = sendto(server, str, strlen(str), 0, (struct sockaddr *) &adr, sizeof adr);
        memset(&str, 0, sizeof(str));
    }
    fd4= fopen("out.txt", "w"); //clean our output file
    fclose(fd4);
    close(server);
    return 0;
}
