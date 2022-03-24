#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
int Socket(int domain, int type, int protocol){
    int res = socket(domain, type, protocol);
    if (res == -1){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    return res;
}
void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    int res = bind(sockfd, addr, addrlen);
    if (res == -1){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

}
void Listen(int sockfd, int backlog){
    int res = listen(sockfd, backlog);
    if (res == -1){
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
}
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen){
    int res = accept(sockfd, addr, addrlen);
    if (res ==-1){
        perror("accept failed");
        exit(EXIT_FAILURE);
    }
    return res;
}
int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen){
    int res = connect(sockfd, addr, addrlen);
    if (res==-1){
        perror("connect failed");
        exit(EXIT_FAILURE);
    }
    return res;
}
void Inet_pton(int af, const char *src, void *dst){
    int res = inet_pton(af, src, dst);
    if (res==0){
        printf("inet_pton failed: src does not contain a character");
        exit(EXIT_FAILURE);
    }
    if (res==-1){
        perror("inet_pton failed");
        exit(EXIT_FAILURE);
    }
}