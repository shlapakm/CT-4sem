#include <sys/types.h>
#define ERPROC_H
#include <sys/socket.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
int login_into_user(char* username);
int pty_master_open(char* slave_name, size_t slave_name_len);
pid_t pty_fork(int* master_fd, char* slave_name, size_t slave_name_len,const struct termios* slave_termios, const struct winsize* slave_winsize);
int Socket(int domain, int type, int protocol);
void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
void Listen(int sockfd, int backlog);
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
void Inet_pton(int af, const char *src, void *dst);
ssize_t Send(int sockfd, const void *buf, size_t len, int flags);
ssize_t Sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
ssize_t Recv(int sockfd, void *buf, size_t len, int flags);
ssize_t Recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
//int Slave_terminal(int fds, int fdm, int fd_for_dir_name);
//int Terminals_config(int* fdm, int* fds);

