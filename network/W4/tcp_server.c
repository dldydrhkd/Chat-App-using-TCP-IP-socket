#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(){
    //variables
    int serverSocketFd, clientSocketFd;
    struct sockaddr_in serverAddress, clientAddress;
    unsigned int clientAddressLength;
    
    //socket creation
    serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSocketFd == -1){
        printf("failed to create a socket\n");
        exit(1);
    } else{
        printf("socket: %d\n", serverSocketFd);
    }
    
    //bind
    bzero(&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family=AF_INET;
    serverAddress.sin_addr.s_addr=htonl(INADDR_ANY);
    serverAddress.sin_port = htons(4444);
    if(bind(serverSocketFd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) != 0){
        printf("failed to bind\n");
        exit(1);
    } else{
        printf("bound\n");
    }
    
    //listening
    if(listen(serverSocketFd, 1) != 0){
        printf("failed to listen\n");
        exit(1);
    } else{
        printf("listening...\n");
    }
    
    //accepting
    bzero(&clientAddress, sizeof(clientAddress));
    clientSocketFd=accept(serverSocketFd, (struct sockaddr*)&clientAddress, &clientAddressLength);
    if(clientSocketFd == -1){
        printf("failed to accept\n")    ;
        exit(1);
    }else{
        printf("accepted\n");
    }
    
    //writing (communicating)
    char* msg="Hello World!";
    write(clientSocketFd, msg, strlen(msg));
    printf("sent : \"%s\n",msg);
    
    //close
    close(clientSocketFd);
    printf("client scoket closed\n");
    close(serverSocketFd);
    printf("server socket closed\n");
    
    return 0;
}
