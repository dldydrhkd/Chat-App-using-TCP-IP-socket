#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>      // bind() 함수에 주소정보를 전달하기 위한 주소 정보 구조체가 들어있다
#include <sys/socket.h>     //socket 관련 함수가 들어있다

void error_handling(char *message); //error를 출력할 함수

int main(int argc, char *argv[])    //메인에서 인자를 받는다
{
	int serv_sock;  //socket()의 반환값을 받기 위한 변수
	int clnt_sock;  //accept()의 반환값을 받기 위한 변수

	struct sockaddr_in serv_addr;   //server 소켓주소를 표현하는 구조체
	struct sockaddr_in clnt_addr;   //client 소켓주소를 표현하는 구조체
	socklen_t clnt_addr_size;       //socklen_t라는 type의 client_addr의 길이를 저장할 변수

	char message[]="Hello World!";
	
	if(argc!=2){
		printf("Usage : %s <port>\n", argv[0]); //인자가 두개가 아니라면 예외처리를 해준다
		exit(1);
	}
	
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);  //socket()에 인자를 주어 반환값을 serv_socket에 저장
	if(serv_sock == -1)     //반환값이 -1이면 오류 발생 socket이 제대로 만들어지지 않음
		error_handling("socket() error");
	
	memset(&serv_addr, 0, sizeof(serv_addr));   //serv_addr에 들어있는 멤버들을 0으로 초기화
	serv_addr.sin_family=AF_INET;           //*serv_addr의 멤버들에 값을 넣어준다
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(atoi(argv[1]));    //bind()에 넣어주기 전 단계 입력한 port번호를 sin_port번호로 준다
	
	if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))==-1 )
		error_handling("bind() error");     //bind()에 초기화했던 serv_addr에 대한 정보를 전달한다
	
	if(listen(serv_sock, 5)==-1)        //listen()로 client를 기다린다
		error_handling("listen() error");
	
	clnt_addr_size=sizeof(clnt_addr);   //clnt_addr의 크기를 clnt_addr_size변수에 저장해준다
	clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr,&clnt_addr_size); //accept()를 이용해 clnt_sock에 연결할 소켓과 클라이언트의 정보를 인자로 넘겨 반환한다
	if(clnt_sock==-1)
		error_handling("accept() error");  
	
	write(clnt_sock, message, sizeof(message)); //write()를 호출하여 client에 데이터 전송
	close(clnt_sock);	//사용했던 socket을 닫는다
	close(serv_sock);   //사용했던 socket을 닫는다
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
