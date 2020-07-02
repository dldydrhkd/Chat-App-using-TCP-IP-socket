#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
	struct sockaddr_in addr1, addr2;
	char *str_ptr;
	char str_arr[20];
   
	addr1.sin_addr.s_addr=htonl(0x01020304);
	addr2.sin_addr.s_addr=htonl(0x01010101);
	
	str_ptr=inet_ntoa(addr1.sin_addr);      //사람이 보는 ip addresss로 바꿔준다
	strcpy(str_arr, str_ptr);
	printf("Dotted-Decimal notation1: %s \n", str_ptr);     //1.2.3.4
	
	inet_ntoa(addr2.sin_addr);
	printf("Dotted-Decimal notation2: %s \n", str_ptr);     //1.1.1.1
	printf("Dotted-Decimal notation3: %s \n", str_arr);     //1.2.3.4
    
    struct sockaddr_in newAddr;
    newAddr.sin_addr.s_addr=htonl(0x09080706);   //06070809로 바꿈
    str_ptr=inet_ntoa(newAddr.sin_addr);        //사람이 보는 ip주소로 바꿈
    printf("\nA new addr: %s \n",str_ptr);
    
    
	return 0;
}
