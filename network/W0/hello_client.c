#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void error_handling(char *message);

int main(int argc, char* argv[])
{
	int sock;
	struct sockaddr_in serv_addr;   //serv_addr 구조체 생성
	char message[30];   //message는 30 제한
	int str_len;    //길이
	
	if(argc!=3){            //인자가 3개가 아니면 에러
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	sock=socket(PF_INET, SOCK_STREAM, 0);   //sock변수에 socket생성 후 반환
	if(sock == -1)                          //생성 실패시 오류 반환
		error_handling("socket() error");
	
	memset(&serv_addr, 0, sizeof(serv_addr));       //serv_addr 멤버 변수 초기화
	serv_addr.sin_family=AF_INET;                   //멤버 변수들 정해주기
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);   //sin_addr.s_addr에 ip 주소
	serv_addr.sin_port=htons(atoi(argv[2]));        //sin_port에 port number를 정해준다
		
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1) // connect함수 호출을 통해 서버 프로그램에 연결을 요청
		error_handling("connect() error!");
	
	str_len=read(sock, message, sizeof(message)-1); //str_len변수에 sock에 들어있는 메시지
	if(str_len==-1)
		error_handling("read() error!");
	
	printf("Message from server: %s \n", message);  
	close(sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
