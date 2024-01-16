#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>


static void die(const char *msg){
    int err = errno;
    fprintf(stderr, "ERROR: %s (%d: %s)\n", msg, err, strerror(err));
    abort();
}

int main(){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        die("socket error");
    }

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0){
        die("connect error");
    }

    char wbuf[64] = "hello";
    write(sockfd, wbuf, strlen(wbuf));

    char rbuf[64] = {};
    ssize_t n = read(sockfd, rbuf, sizeof(rbuf)-1);
    if(n < 0){
        die("read error");
    }

    printf(
        "read %zd bytes: %s\n",
        n,
        rbuf
    );

    return 0;
}