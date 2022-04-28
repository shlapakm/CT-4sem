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
#include <fcntl.h>
#include <termios.h>
//Attention! You can send only one command!
int main(int argc, char *argv[]){
    if (argc==1){
        fprintf(stderr,"You must specify the -t(TCP)/-u(UDP)/-b(broadcast by UDP) connection type\n");
	    exit(1);
    }
    if ((argc==2) && (strcmp(argv[1],"-b"))){
        fprintf(stderr,"You must specify the IP address\n");
	    exit(1);
    }
//TCP connection
    if (strcmp(argv[1],"-t")==0){
        int client = Socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in adr = {0};
        adr.sin_family = AF_INET;
        adr.sin_port = htons(34543);
        Inet_pton(AF_INET,argv[2], &adr.sin_addr);
        int broadcast=1;
        if (setsockopt(client, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof broadcast) == -1) { 
            perror("setsockopt (SO_BROADCAST)"); 
            exit(1); 
        }
        Connect(client,(struct sockaddr *) &adr, sizeof adr);
        char str[255];
       // socklen_t adrlen = sizeof adr;
        fgets(str, 255, stdin);
        
        //printf("STRLEN = %d", strlen(str));
        str[strlen(str)] = '\0';

        Send(client, str, strlen(str),0);
        char num[1024];
        Recv(client, num, 1024, 0);
        
        
        int n = atoi(num); //convert string to integer
        int i = 0;
        // printf("numbers of string: %d\n", n); - if you want to check, how many line in file
        memset(&str, 0, sizeof(str));
        while (i < n){ //receive lines
            int length = Recv(client, str,255,0);
            str[length] = '\0';
            //printf("legnth =  %d\n", length);
            printf("%s",str);
            i+=1;
            memset(&str, 0, sizeof(str));
        }
        close(client);
        return 0;
    }
//UDP connection
    if (strcmp(argv[1],"-u")==0){
        int client = Socket(AF_INET, SOCK_DGRAM, 0);
        struct sockaddr_in adr = {0};
        adr.sin_family = AF_INET;
        adr.sin_port = htons(34543);
        Inet_pton(AF_INET, argv[2], &adr.sin_addr);
        char str[255];
        socklen_t adrlen = sizeof adr;
        fgets(str, 255, stdin);
        Sendto(client, str, strlen(str), 0, (struct sockaddr *) &adr, sizeof adr);
        char num[1024];
        Recvfrom(client, num, 1024, 0, (struct sockaddr *) &adr,&adrlen);
        int n = atoi(num); //convert string to integer
        int i = 0;
        // printf("numbers of string: %d\n", n); - if you want to check, how many line in file
        memset(&str, 0, sizeof(str));
        while (i < n){ //receive lines
            Recvfrom(client, str,255,0, (struct sockaddr *) &adr,&adrlen);
            printf("%s",str);
            i+=1;
            memset(&str, 0, sizeof(str));
        }
        close(client);
        return 0;
    }
//broadcast
    if (strcmp(argv[1],"-b")==0){
        int client = Socket(AF_INET, SOCK_DGRAM, 0);
        struct sockaddr_in adr = {0};
        struct sockaddr_in server_adr = {0};
        adr.sin_family = AF_INET;
        adr.sin_port = htons(34543);
        Inet_pton(AF_INET, "255.255.255.255", &adr.sin_addr);
        int broadcast = 1;
        if (setsockopt(client, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof broadcast) == -1) { 
            perror("setsockopt (SO_BROADCAST)"); 
            exit(1); 
        }
        char str[] = "broadcast";
        socklen_t adrlen = sizeof adr;
        socklen_t server_adrlen = sizeof server_adr;
        //fgets(str, 255, stdin);
        Sendto(client, str, strlen(str), 0, (struct sockaddr *) &adr, sizeof adr);
        char num[1024];
        Recvfrom(client, num, 1024, 0, (struct sockaddr *) &server_adr,&server_adrlen);
        int n = atoi(num); //convert string to integer
        int i = 0;
        // printf("numbers of string: %d\n", n); - if you want to check, how many line in file
        memset(&str, 0, sizeof(str));
        while (i < n){ //receive lines
            Recvfrom(client, str,255,0, (struct sockaddr *) &server_adr,&server_adrlen);
            printf("%s",str);
            i+=1;
            memset(&str, 0, sizeof(str));
        }
        char s[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET, &server_adr.sin_addr, s, sizeof s);
        printf("Server found IP %s by broadcast\n", s);
        close(client);
        return 0;
    }
}
