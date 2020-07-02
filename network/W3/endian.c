#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    unsigned short host_port = 0x1234;
    unsigned short net_port;
    unsigned long host_address = 0x12345678;
    unsigned long net_address;
    
    net_port = htons(host_port);   //컴퓨터가 보는방식으로 바꿈
    net_address=htonl(host_address);        //컴퓨터가 보는방식으로 바꿈
    
    printf("Host ordered port %#x \n", host_port);              //0x1234
    printf("Network ordered port %#x \n", net_port);            //0x3412
    printf("Host ordered address %#lx \n", host_address);       //0x12345678
    printf("Network ordered address %#lx \n", net_address);     //0x78563412
    
    unsigned long host_address2 = ntohl(net_address);       //다시 우리가 보는 방식으로 바꿈
    printf("\nHost address after converting back: %#lx \n", host_address2);
    
    
    return 0;
}
