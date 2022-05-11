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
#include <termios.h>
#include <poll.h>
#include <syslog.h>
//#include <security/pam_appl.h>
//#include <security/pam_misc.h>
#define MAX_BUFFER 255
#define MAX_LEN 255
#define POLLIN 0x001

int main(int argc, char *argv[]){
    openlog("sshclient", LOG_PID, LOG_USER);
    if (argc==1){
        fprintf(stderr,"You must specify the -t(TCP)/-u(UDP)/-b(broadcast by UDP) connection type\n");
        syslog(LOG_PERROR, "no entering info");
	    exit(1);
    }
    if ((argc==2) && (strcmp(argv[1],"-b"))){
        fprintf(stderr,"You must specify the IP address\n");
        syslog(LOG_PERROR, "always no entering info");
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
        char username[255];
        strncpy(username, argv[3], 255);
        struct termios term = {};
        struct termios oldt = {};

        if (tcgetattr(STDIN_FILENO, &oldt) == -1)
        {
            return -1;
        }
        cfmakeraw(&term);
        if (tcsetattr(STDIN_FILENO, TCSANOW, &term) == -1)
        {
            return -1;
        }
        char buff[MAX_BUFFER] = "";
        int result = Send(client, username, strlen(username),0);
        struct pollfd master[2] = {{.fd = client, .events = POLLIN}, {.fd = STDIN_FILENO, .events = POLLIN}};
        size_t n_write = 0;
        size_t n_read  = 0;
        while (1)
        {
        int event = poll(master, 2, 100);
        if (event > 0)
        {   
            if (master[0].revents == POLLIN)
            {
                    n_read = Recv(client, buff, sizeof(buff), 0);
                    if (n_read == -1)
                    {
                        return -1;
                    }

                    if (n_read == 0)
                    {
                        close(client);
                        break;
                    }
                    n_write = write(STDOUT_FILENO, buff, n_read);
                    if (n_write == -1)
                    {
                        return -1;
                    }

                }
                
                if (master[1].revents == POLLIN)
                {
                    n_read = read(STDIN_FILENO, buff, sizeof(buff) - 1);
                    if (n_read == -1)
                    {
                        return -1;
                    }
                    n_write = Send(client, buff, n_read, 0);
                    if (n_write == -1)
                    {
                        return -1;
                    }
                }
        }
        else if (event ==0){
            continue;
        }
        else{
            break;
        }
        }
        if (tcsetattr(STDIN_FILENO, TCSANOW, &oldt) == -1)
    {
        return -1;
    }

    return 0;
    }

                

//UDP connection
    if (strcmp(argv[1],"-u")==0){
        int client = Socket(AF_INET, SOCK_DGRAM, 0);
        struct sockaddr_in adr = {0};
        adr.sin_family = AF_INET;
        adr.sin_addr.s_addr = htonl(INADDR_ANY);
        adr.sin_port = htons(34543);
        socklen_t adrlen = sizeof adr;
        Inet_pton(AF_INET, argv[2], &adr.sin_addr);
        char username[255];
        strncpy(username, argv[3], 255);
        struct termios term = {};
        struct termios oldt = {};

        if (tcgetattr(STDIN_FILENO, &oldt) == -1)
        {
            perror("tcsetattr()");
            return -1;
        }
        cfmakeraw(&term);
        if (tcsetattr(STDIN_FILENO, TCSANOW, &term) == -1)
        {
            perror("tcsetattr()");
            return -1;
        }
        char buff[MAX_BUFFER] = "";
        int result = Sendto(client, username, strlen(username),0, (struct sockaddr *) &adr, sizeof adr);
        struct pollfd master[2] = {{.fd = client, .events = POLLIN}, {.fd = STDIN_FILENO, .events = POLLIN}};
        size_t n_write = 0;
        size_t n_read  = 0;
        while (1)
        {
        int event = poll(master, 2, 100);
        if (event > 0)
        {   
            if (master[0].revents == POLLIN)
            {
                    n_read = Recvfrom(client, buff, sizeof(buff), 0, (struct sockaddr *) &adr,&adrlen);
                    if (n_read == -1)
                    {
                        perror("rudp_recv(SOCK_STREAM)");
                        return -1;
                    }

                    if (n_read == 0)
                    {
                        close(client);
                        break;
                    }
                    n_write = write(STDOUT_FILENO, buff, n_read);
                    if (n_write == -1)
                    {
                        perror("write()");
                        return -1;
                    }

                }
                
                if (master[1].revents == POLLIN)
                {
                    n_read = read(STDIN_FILENO, buff, sizeof(buff) - 1);
                    if (n_read == -1)
                    {
                        perror("read()");
                        return -1;
                    }
                    n_write = Send(client, buff, n_read, 0);
                    if (n_write == -1)
                    {
                        perror("rudp_send(SOCK_STREAM)");
                        return -1;
                    }
                }
        }
        else if (event ==0){
            continue;
        }
        else{
            break;
        }
        }
        if (tcsetattr(STDIN_FILENO, TCSANOW, &oldt) == -1)
    {
        perror("tcsetattr()");
        return -1;
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
