#define _POSIX_C_SOURCE 200112L


#define PORT "4500"
#define BACKLOG 10

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <arpa/inet.h>

int main() {

    struct addrinfo hints, *p, *res;
    struct sockaddr_storage client_addr;
    socklen_t size = sizeof(client_addr);
    char IP_client[INET6_ADDRSTRLEN];
    int sockfd, client_sockfd;
    char *msg = "Hello from network!\n";
    int len = strlen(msg);

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if(getaddrinfo(NULL, PORT, &hints, &res) != 0){
        printf("Error with getaddrinfo!\n");
        return 1;
    }

    for(p = res; p != NULL; p = p->ai_next){
        if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            perror("Serever:socket");
            continue;
        }
        if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("Server:bind");
            continue;
        }
        break;
    }

    if(p == NULL){
        printf("Error: server failed to bind\n");
        return 2;
    }
    printf("Bind sucsess!\n");

    freeaddrinfo(res);

    if(listen(sockfd, BACKLOG) == -1) {
        perror("Server:listen");
        exit(1);
    }

    printf("Server is listening...\n");

    if((client_sockfd = accept(sockfd, (struct sockaddr*) &client_addr, &size)) == -1){
        perror("Server:accept");
        exit(1);
    }

    if(client_addr.ss_family == AF_INET) {
        struct sockaddr_in *addr = (struct sockaddr_in*) &client_addr;
        inet_ntop(AF_INET, &(addr->sin_addr), IP_client, sizeof(IP_client));
    } else{
        struct sockaddr_in6 *addr = (struct sockaddr_in6*) &client_addr;
        inet_ntop(AF_INET6, &(addr->sin6_addr), IP_client, sizeof(IP_client));
    }
    printf("Client has connected!\n");
    printf("Clients IP addres: %s\n", IP_client);

    if(send(client_sockfd, msg, len, 0) == -1) {
        perror("Server:send");
        exit(1);
    }
    printf("Data has sended!\n");
    printf("Serever closed!\n");

    close(client_sockfd);    
    
    return 0;
}