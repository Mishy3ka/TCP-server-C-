#define _POSIX_C_SOURCE 200112L

#define PORT "5000"
#define BACKLOG 10

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>


int main() {

    struct addrinfo hints, *res, *p;
    struct sockaddr_storage client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    int sockfd, clientfd;
    char client_IP[INET6_ADDRSTRLEN];
    char buf[256];
    int client_msg_size;

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;


    if(getaddrinfo(NULL, PORT, &hints, &res) != 0){
        perror("Serever:getaddrinfo");
        exit(1);
    }

    for(p = res; p != NULL; p = p->ai_next) {
        if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            perror("Server:socket\n");
            continue;
        }

        if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1){
            perror("Server:bind\n");
            continue;
        }
        break;
    }

    if(p == NULL){
        perror("Server:bind\n");
        exit(1);
    }

    printf("Bind success!\n");
    freeaddrinfo(res);

    if(listen(sockfd, BACKLOG) == -1){
        perror("Server:listen\n");
    }
    printf("Server is listening...\n");


    while(1){

        while(waitpid(-1, NULL, WNOHANG) > 0);

        if((clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_addr_size)) == -1){
            perror("Server:accept\n");
            continue;
        }
        printf("Client connected!\n");

        if(client_addr.ss_family == AF_INET){
            struct sockaddr_in *addr = (struct sockaddr_in*) &client_addr;
            inet_ntop(AF_INET, &(addr->sin_addr), client_IP, sizeof(client_IP));
        } else {
            struct sockaddr_in6* addr = (struct sockaddr_in6*) &client_addr;
            inet_ntop(AF_INET6, &(addr->sin6_addr), client_IP, sizeof(client_IP));
        }

        printf("client ip: %s\n", client_IP);

        if(fork() == 0) {
            close(sockfd);

            if((client_msg_size = recv(clientfd, buf, sizeof(buf), 0)) == -1){
                perror("Server:recv\n");
                exit(1);
            }

            printf("Client sent: %s", buf);

            if(send(clientfd, buf, client_msg_size, 0) == -1){
                perror("Server:send\n");
                exit(1);
            }
            printf("Data has sent!\n");
            close(clientfd);
            printf("Waiting for next connection...\n\n");
            exit (0);
        }
        close(clientfd);
    }


    return 0;
}