#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>

static void msg(const char *msg){
    fprintf(stderr, "MSG: %s\n", msg);
}

static void die(const char *msg){
    int err = errno;
    fprintf(stderr, "ERROR: %s (%d: %s)\n", msg, err, strerror(err));
    abort();
}


static void do_something(int connfd){
    char rbuf[64] = {};
    ssize_t n = read(connfd, rbuf, sizeof(rbuf)-1);
    if(n < 0){
        msg("read error");
        return;
    }

    printf(
        "read %zd bytes: %s\n",
        n,
        rbuf
    );
    

    char wbuf[64] = "world, nice to meet you!";

    write(connfd, wbuf, strlen(wbuf));
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

    if(bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0){
        die("bind error");
    }

    if(listen(sockfd, 5) < 0){
        die("listen error");
    }

    for(;;){
        struct sockaddr_in peer_addr = {};
        socklen_t peer_addrlen = sizeof(peer_addr);
        int connfd = accept(sockfd, (struct sockaddr*)&peer_addr, &peer_addrlen);
        if(connfd < 0){
            die("accept error");
        }

        char peer_ip[INET_ADDRSTRLEN] = {};
        if(inet_ntop(AF_INET, &peer_addr.sin_addr, peer_ip, sizeof(peer_ip)) == NULL){
            die("inet_ntop error");
        }

        printf("accept a connection from %s:%d\n", peer_ip, ntohs(peer_addr.sin_port));

        do_something(connfd);

        close(connfd);
    }

    close(sockfd);
    return 0;
}