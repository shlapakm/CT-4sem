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
#include <termios.h>
#include <poll.h>
#include <syslog.h>
//#include <security/pam_appl.h>
//#include <security/pam_misc.h>
#define MAX_BUFFER 255
#define MAX_LEN 255
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
    adr.sin_addr.s_addr = htonl(INADDR_ANY);
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
                char buff[MAX_BUFFER] = "";
                char slave[MAX_LEN] = "";
                int master_fd = 0;
                char username[MAX_LEN] = "";
                int result = Recv(fd, username, MAX_LEN-1, 0);
                pid_t pid = pty_fork(&master_fd, slave, MAX_LEN, NULL, NULL);
                if (pid == 0)
                {
                    result = login_into_user(username);
                    if (result == -1){
                        close(fd);
                        return 0;
                    }
                    //result = set_id(username);
                    //if (result == -1){
                        //close(fd);
                        //return -1;
                    //}
                    char* bash_argv[] = {"bash", NULL};
                    execvp("bash", bash_argv);
                    return -1;
                }
                struct pollfd master[2] = {{.fd = master_fd, .events = POLLIN}, {.fd = fd, .events = POLLIN}};
                size_t n_write = 0;
                size_t n_read  = 0;
                while (1)
                {
                    int event = poll(master, 2, 100);
                    if (event > 0)
                    {
                        if (master[0].revents == POLLIN)
                        {
                            n_read = read(master_fd, buff, sizeof(buff));
                            if (n_read == -1)
                            {
                                perror("read");
                                return -1;
                            }
                            n_write = Send(fd, buff, n_read, 0);
                            if (n_write == -1)
                            {
                                perror("rudp_recv(SOCK_STREAM)");
                                return -1;
                            }
                        }
                        if (master[1].revents == POLLIN)
                        {
                            n_read = Recv(fd, buff, sizeof(buff), 0);
                            buff[n_read] = '\0';
                            if (n_read == -1)
                            {
                                perror("rudp_send(SOCK_STREAM)");
                                return -1;
                            }
                            if (!n_read)
                            {
                                close(fd);
                                return 1;             
                            }
                            n_write = write(master_fd, buff, n_read);
                            if (n_write == -1)
                            {
                                perror("write()");
                                return -1;
                            }
                        }
                    }
                    else if (event == 0) continue;
                    else
                    {
                        break;
                    }
                }
                return 0;
            }
                
        }

        if (FD_ISSET(server2, &rset)){
            char s[INET6_ADDRSTRLEN];
            char buff[MAX_BUFFER] = "";
            char slave[MAX_LEN] = "";
            int master_fd = 0;
            char username[MAX_LEN] = "";
            Recvfrom(server2, username, MAX_LEN -1 ,0,(struct sockaddr *) &client_adr, &client_adrlen);
            inet_ntop(AF_INET, &client_adr.sin_addr, s, sizeof s);
            printf("server: got UDP connection from %s\n", s);
            if (strcmp(username, "broadcast")){
                pid_t pid = pty_fork(&master_fd, slave, MAX_LEN, NULL, NULL);
                if (pid == 0)
                {
                    int result = login_into_user(username);
                    if (result == -1){
                        close(server2);
                        return -1;
                    }
                    char* bash_argv[] = {"bash", NULL};
                    execvp("bash", bash_argv);
                    return -1;
                }
                struct pollfd master[2] = {{.fd = master_fd, .events = POLLIN}, {.fd = server2, .events = POLLIN}};
                size_t n_write = 0;
                size_t n_read  = 0;
                while (1)
                {
                    int event = poll(master, 2, 100);
                    if (event > 0)
                    {
                        if (master[0].revents == POLLIN)
                        {
                            n_read = read(master_fd, buff, sizeof(buff));
                            if (n_read == -1)
                            {
                                perror("read");
                                return -1;
                            }
                            n_write = Sendto(server2, buff, n_read, 0, (struct sockaddr *) &client_adr, sizeof client_adr);
                            if (n_write == -1)
                            {
                                perror("rudp_recv(SOCK_DGRAM)");
                                return -1;
                            }
                        }
                        if (master[1].revents == POLLIN)
                        {
                            n_read = Recvfrom(server2, buff, sizeof(buff), 0, (struct sockaddr *) &client_adr, sizeof client_adr);
                            buff[n_read] = '\0';
                            if (n_read == -1)
                            {
                                perror("rudp_send(SOCK_STREAM)");
                                return -1;
                            }
                            n_write = write(master_fd, buff, n_read);
                            if (n_write == -1)
                            {
                                perror("write()");
                                return -1;
                            }
                        }
                    }
                    else if (event == 0) continue;
                    else
                    {
                        break;
                    }
                }

            }
            else {
                Sendto(server2, "hi", strlen("hi"), 0,(struct sockaddr *) &client_adr, sizeof client_adr);
            }
        }
}
}
