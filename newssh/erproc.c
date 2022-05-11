#define _XOPEN_SOURCE 600
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
#include <sys/select.h>
#include <sys/ioctl.h>
#define MAX_LEN 1000
#define __USE_BSD


#include <security/pam_appl.h>
#include <security/pam_misc.h>
#define BUF_SIZE 255
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
ssize_t Send(int sockfd, const void *buf, size_t len, int flags){
    ssize_t res = send(sockfd,buf,len,flags);
    if (res==-1){
        printf("send failed");
        exit(EXIT_FAILURE);
    }
    return res;
}
ssize_t Sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen){
    ssize_t res = sendto(sockfd,buf,len,flags,dest_addr,addrlen);
    if (res==-1){
        printf("sendto failed");
        exit(EXIT_FAILURE);
    }
    return res;
}
ssize_t Recv(int sockfd, void *buf, size_t len, int flags){
    ssize_t res = recv(sockfd,buf,len,flags);
    if (res==-1){
        printf("recv failed");
        exit(EXIT_FAILURE);
    }
    return res;
}
ssize_t Recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen){
    ssize_t res = recvfrom(sockfd,buf,len,flags, src_addr, addrlen);
    if (res==-1){
        printf("recvfrom failed");
        exit(EXIT_FAILURE);
    }
    return res;
}
int pty_master_open(char* slave_name, size_t slave_name_len)
{
    int master_fd = posix_openpt(O_CREAT|O_RDWR);
    if (master_fd == -1) 
    {
        perror("ERROR: posix_openpt()");
        return  0;
    }
    
    if (grantpt(master_fd) == -1)
    {
        perror("ERROR: grantpt()");
        close(master_fd);
        return  0;       
    }

    if (unlockpt(master_fd) == -1)
    {
        perror("ERROR: unlockpt()");
        close(master_fd);
        return  0;
    }

    char* pathname = ptsname(master_fd);
    if (pathname == NULL)
    {
        perror("ERROR: ptsname()");
        close(master_fd);
        return  0;
    }

    if (strlen(pathname) < slave_name_len)
    {
        strncpy(slave_name, pathname, slave_name_len);
    }
    else
    {  
        close(master_fd);
        errno = EOVERFLOW;
        perror("ERROR: overflow slave name buffer");
        return 0;
    }

    return master_fd;
}

pid_t pty_fork(int* master_fd, char* slave_name, size_t slave_name_len,                \
               const struct termios* slave_termios, const struct winsize* slave_winsize)
{
    char slave_name_buffer[MAX_LEN] = "";

    int mfd = pty_master_open(slave_name_buffer, MAX_LEN);
    if (mfd < 0) return mfd;

    if (slave_name != NULL)
    {
        if (strlen(slave_name_buffer) < slave_name_len)
        {
            strncpy(slave_name, slave_name_buffer, slave_name_len);
        }
        else
        {
            close(mfd);
            errno = EOVERFLOW;
            perror("ERROR: overflow slave name buffer");
            return 0;
        }
    }

    pid_t pid = fork(); 

    if (pid == -1)
    {
        perror("ERROR: fork()");
        close(mfd);
        return  0;      
    }

    if (pid) // parent
    {
        *master_fd = mfd;
        return pid;
    }   

    // child
    if (setsid() == -1)
    {
        perror("ERROR: pty_fork:setsid()");
        exit(EXIT_FAILURE);
    }

    close(mfd);

    int slave_fd = open(slave_name_buffer, O_RDWR);
    if (slave_fd == -1)
    {
        perror("ERROR: pty_fork:open()");
        exit(EXIT_FAILURE);
    }

#ifdef TIOCSCTTY

    if (ioctl(slave_fd, TIOCSCTTY, 0) == -1)
    {
        perror("ERROR: pty_fork:ioctl(TIOCSCTTY)");
        exit(EXIT_FAILURE);
    }

#endif // TIOCSTTY

    if (slave_termios != NULL)
    {
        if (tcsetattr(slave_fd, TCSANOW, slave_termios) == -1)
        {
            perror("ERROR: pty_fork:tcsetattr()");
            exit(EXIT_FAILURE);
        }
    }

    if (slave_winsize != NULL)
    {
        if (ioctl(slave_fd, TIOCSWINSZ, slave_winsize) == -1)
        {
            perror("ERROR: pty_fork:icontl(TIOCSWINSZ)");
            exit(EXIT_FAILURE);
        }
    }

    if (dup2(slave_fd, STDIN_FILENO) != STDIN_FILENO)
    {
        perror("ERROR: pty_fork:dup2 - STDIN_FILENO");
        exit(EXIT_FAILURE);
    }
    if (dup2(slave_fd, STDOUT_FILENO) != STDOUT_FILENO)
    {
        perror("ERROR: pty_fork:dup2 - STDOUT_FILENO");
        exit(EXIT_FAILURE);
    }
    if (dup2(slave_fd, STDERR_FILENO) != STDERR_FILENO)
    {
        perror("ERROR: pty_fork:dup2 - STDERR_FILENO");
        exit(EXIT_FAILURE);
    }

    if (slave_fd > STDERR_FILENO)
    {
        close(slave_fd);
    }

    return 0; // child
}   
struct pam_conv conv = {misc_conv, NULL};

int login_into_user(char* username)
{
    pam_handle_t* pam = NULL;

    int ret = pam_start("power_ssh", username, &conv, &pam);
    if (ret != PAM_SUCCESS)
    {
        fprintf(stderr, "ERROR: pam_start()\n");
        return -1;
    }



    ret = pam_authenticate(pam, 0);
    if (ret != PAM_SUCCESS)
    {

        fprintf(stderr, "Incorrect password\n");
        return -1;
    }

    ret = pam_acct_mgmt(pam, 0);
    if (ret != PAM_SUCCESS)
    {
        fprintf(stderr, "User account expired!\n");
        return -1;
    }

    errno = 0;
    return 0;
}

/*int set_id(const char* username)
{
    struct passwd* info = getpwnam(username);
    if (!info)
    {
        perror("getpwnam()");
        return -1;
    }

    if (setgid(info->pw_gid) == -1)
    {
        perror("setgid()");
        return -1;
    }

    if (setuid(info->pw_uid) == -1)
    {
        perror("setuid()");
        return -1;
    }

    return 0;
}*/
/*int Slave_terminal(int fds, int fdm, int fd_for_dir_name) {
    
    struct termios slave_orig_term_settings; // Saved terminal settings
    struct termios new_term_settings; // Current terminal settings
    char dir_name[BUF_SIZE] = {'\0'};
    int ret = read(fd_for_dir_name, dir_name, BUF_SIZE);
    if (ret <= 0) {
        exit(EXIT_FAILURE);
    }
    if (chdir(dir_name) < 0) {
        exit(EXIT_FAILURE);
    }
    // Close the master side of the PTY
    close(fdm);
    // Save the defaults parameters of the slave side of the PTY
    tcgetattr(fds, &slave_orig_term_settings);

    // Set RAW mode on slave side of PTY
    new_term_settings = slave_orig_term_settings;
    cfmakeraw (&new_term_settings);
    tcsetattr (fds, TCSANOW, &new_term_settings);

    // The slave side of the PTY becomes the standard input and outputs of the child process
    close(STDERR_FILENO);
    close(STDIN_FILENO);
    close(STDOUT_FILENO); 

    if (dup(fds) < 0 || dup(fds) < 0 || dup(fds) < 0) {
        exit(EXIT_FAILURE);
    }
   
    // Now the original file descriptor is useless
    close(fds);

    // Make the current process a new session leader
    if (setsid() < 0) {
        exit(EXIT_FAILURE);
    }

    // As the child is a session leader, set the controlling terminal to be the slave side of the PTY
    // (Mandatory for programs like the shell to make them manage correctly their outputs)
    if (ioctl(0, TIOCSCTTY, 1) < 0) {
        exit(EXIT_FAILURE);
    }

    // Execution of the program
    char *bash_argv[] = {"sh", NULL};
    if (execvp("sh", bash_argv) < 0) {
        exit(EXIT_FAILURE);
    }
    return 1;
}
/*int Terminals_config(int* fdm, int* fds) {
    *fdm = posix_openpt(O_RDWR);
    if (*fdm < 0) {
        return -1;
    }

    int rc = grantpt(*fdm);
    if (rc != 0) {
        return -1;
    }

    rc = unlockpt(*fdm);
    if (rc != 0)  {
        return -1;
    }

    // Open the slave side ot the PTY
    *fds = open(ptsname(*fdm), O_RDWR);
    return 0;
}*/
