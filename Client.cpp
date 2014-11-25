#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
using namespace std;

#define MAXLEN 4096

int main(){

    int socket_fd;
    char buffer[MAXLEN + 1];
    struct sockaddr_in servaddr;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd < 0){
        printf("Socket opening failure\n");
        return 0;
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(1050);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if(inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0){
        printf("address conversion error\n");
        return 0;
    }

    if(connect(socket_fd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
    {
        printf("connection error\n");
        return 0;
    }

    int input;
    printf("Enter Number : ");
    scanf("%s",buffer);

    write(socket_fd, buffer, strlen(buffer));

    char reply[MAXLEN];
    int n = read(socket_fd, &reply, MAXLEN);

    printf("Server Response -\n Number : %d\n", n);
    close(socket_fd);

    return 0;
}
