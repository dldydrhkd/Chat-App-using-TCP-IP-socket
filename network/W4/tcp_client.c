#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(){
    // variables
    int socketFd;
    struct sockaddr_in serverAddress;
    
    
    // socket creation
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if(socketFd==-1){
        printf("failed to create a socket\n");
        exit(1);
    } else{
        printf("socket : %d\n", socketFd);
    }
    
    // connect
    bzero(&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(4444);
    if(connect(socketFd, (struct sockaddr*)&serverAddress, sizeof(serverAddress))!=0){
        printf("failed to connect\n");
        exit(1);
    } else{
        printf("connected\n");
    }
    
    // read
    char buf[128];
    read(socketFd, buf, 128);
    printf("received : %s\n",buf);
    
    // close
    close(socketFd);
    printf("client socket closed");
    
    return 0;
}
