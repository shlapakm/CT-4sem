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
//#include <security/pam_appl.h>
//#include <security/pam_misc.h>

int max(int x, int y)
{
    if (x > y)
        return x;
    else
        return y;
}

#define BD_NO_CHDIR 01
#define BD_NO_CLOSE_FILES 02
#define BD_NO_REOPEN_STD_DFS 04
#define BD_NO_UMASK0 010
#define BD_MAX_CLOSE 8192
int becomeDaemon(int flags);

int becomeDaemon(int flags){
    int maxfd, fd;
    switch (fork()){
        case -1: return -1;
        case 0: break;
        default: _exit(EXIT_SUCCESS);
    }
    if (setsid()==-1)
        return -1;
    switch (fork()) {
        case -1: return -1;
        case 0: break;
        default: _exit(EXIT_SUCCESS);
    }
    if (!(flags & BD_NO_UMASK0))
        umask(0);
    if (!(flags & BD_NO_CHDIR))
        chdir("/");
    if (!(flags & BD_NO_CLOSE_FILES)){
        maxfd = sysconf(_SC_OPEN_MAX);
        if (maxfd == -1)
            maxfd = BD_MAX_CLOSE;
        for (fd = 0; fd < maxfd; fd++)
            close(fd);
    }

    if(!(flags & BD_NO_REOPEN_STD_DFS))
    {
        close(STDIN_FILENO);
    
        fd = open("/dev/null", O_RDWR);

        if (fd != STDIN_FILENO)
            return -1;
        if (dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO)
            return -1;
        if (dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO)
            return -1;
    }


    return 0;


}



int main(){
    //if (becomeDaemon(0)==1)
        //exit(EXIT_FAILURE);
    fd_set rset;
    pid_t childpid;
    int server1 = Socket(AF_INET, SOCK_STREAM,0);
    int server2 = Socket(AF_INET, SOCK_DGRAM,0);
    struct sockaddr_in adr;
    struct sockaddr_in client_adr;
    adr.sin_family = AF_INET;
    adr.sin_port =  htons(34543);
    int yes = 1;
    if (setsockopt(server1,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    } 
    Bind(server1, (struct sockaddr *) &adr, sizeof adr);
    if (setsockopt(server2,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    } 
    Bind(server2, (struct sockaddr *) &adr, sizeof adr);
    Listen(server1,10);
    socklen_t adrlen = sizeof adr;
    socklen_t client_adrlen = sizeof client_adr;
    printf("server: waiting for connections...\n");
    // clear the descriptor set
    FD_ZERO(&rset);
    // get maxfd
    int maxfdp1 = max(server1, server2) + 1;
    for (;;) {
        // set listenfd and udpfd in readset
        FD_SET(server1, &rset);
        FD_SET(server2, &rset);
        // select the ready descriptor
        int nready = select(maxfdp1, &rset, NULL, NULL, NULL);
        if (FD_ISSET(server1, &rset)){
            int fd = Accept(server1, (struct sockaddr *) &client_adr,&client_adrlen);
            if ((childpid = fork()) == 0) {
				close(server1);
                char s[INET6_ADDRSTRLEN];
                inet_ntop(AF_INET, &client_adr.sin_addr, s, sizeof s);
                printf("server: got TCP connection from %s\n", s);
                char command[255];
                FILE *fd2;
                FILE *fd3;
                FILE *fd4;
                int fd1;
                char str[255];
                if ( (fd1= open("out1.txt", O_CREAT|O_WRONLY, 0666)) == -1 )
                printf("MOLODETS\n");
                int length = Recv(fd, command,255,0);
                command[length] = '\0';
                //printf("length =  %d\n", length);
                //printf("cmd = [%s]\n", command);
                int oldstdout = dup(1);
                int oldstderr = dup(2);
                dup2(fd1,1);
                dup2(fd1,2);
                system(command);
                int n=0; //number of lines in the file
                fd2= fopen("out1.txt", "r");
                while((fgets(str, 255, fd2))!=NULL){
                    n+=1;
                }
                char num[1024];
                sprintf(num,"%d",n);
                Send(fd, num, strlen(num), 0);
                close(fd1);
                fclose(fd2);
                fd3 = fopen("out1.txt", "r");
                while((fgets(str, 256, fd3))!=NULL){
                    Send(fd, str, strlen(str), 0);
                    memset(&str, 0, sizeof(str));
                }
                fd4= fopen("out1.txt", "w"); //clean our output file
                fclose(fd4);
                close(fd);
                dup2(oldstdout,1);
                dup2(oldstderr,2);
                exit(0);
            }
            close(fd);
        }

        if (FD_ISSET(server2, &rset)){
            char s[INET6_ADDRSTRLEN];
            char command[255];
            FILE *fd2;
            FILE *fd3;
            FILE *fd4;
            int fd1;
            char str[255];
            if ( (fd1= open("out2.txt", O_CREAT|O_WRONLY, 0666)) == -1 )
                printf("MOLODETS\n");
            Recvfrom(server2, command,255,0,(struct sockaddr *) &client_adr, &client_adrlen);
            inet_ntop(AF_INET, &client_adr.sin_addr, s, sizeof s);
            printf("server: got UDP connection from %s\n", s);
            //printf("cmd = %s\n", command);
            if (strcmp(command, "broadcast")){
            int oldstdout = dup(1);
            int oldstderr = dup(2);
            dup2(fd1,STDOUT_FILENO);
            dup2(fd1,STDERR_FILENO);
            system(command);
            int n=0; //number of lines in the file
            fd2= fopen("out2.txt", "r");
            while((fgets(str, 255, fd2))!=NULL){
                n+=1;
            }
            char num[1024];
            sprintf(num,"%d",n);
            Sendto(server2, num, strlen(num), 0,(struct sockaddr *) &client_adr, sizeof client_adr);
            close(fd1);
            fclose(fd2);
            fd3 = fopen("out2.txt", "r");
            while((fgets(str, 256, fd3))!=NULL){
                Sendto(server2, str, strlen(str), 0, (struct sockaddr *) &client_adr, sizeof client_adr);
                memset(&str, 0, sizeof(str));
            }
            fd4= fopen("out2.txt", "w"); //clean our output file
            fclose(fd4);
            dup2(oldstdout,1);
            dup2(oldstderr,2);
            }
            else {
                Sendto(server2, "hi", strlen("hi"), 0,(struct sockaddr *) &client_adr, sizeof client_adr);
            }
        }
}
}
