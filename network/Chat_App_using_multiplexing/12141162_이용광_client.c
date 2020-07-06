#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 100
#define NAME_SIZE 30
void error_handling(char *message);
void Input_routine(int sock, char *message);
void Output_routine(int sock, char *message);

char name[NAME_SIZE];

int main(int argc, char *argv[])
{
    //setup variables
	int sock;
    pid_t pid;
	char message[BUF_SIZE];
    struct sockaddr_in serv_adr;

	if(argc!=4) {       //if the format does not match
		printf("Usage : %s <IP> <port> <name>\n", argv[0]);     //print error message
		exit(1);
	}
    
    sprintf(name, "[%s]", argv[3]);
	
	sock=socket(PF_INET, SOCK_STREAM, 0);   //create client socket
	if(sock==-1)
		error_handling("socket() error");
	
	memset(&serv_adr, 0, sizeof(serv_adr));     //initialize socket
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_adr.sin_port=htons(atoi(argv[2]));
	
	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)        //request connection to server
		error_handling("connect() error!");
    
    pid=fork();
    if(pid==0){         //output handling process
        Output_routine(sock, message);
    }
    else{               //input handling process
        Input_routine(sock, message);
    }
    close(sock);
    return 0;
}

void Input_routine(int sock, char *message) {       //read message from server
    char total_message[NAME_SIZE + BUF_SIZE];
    while(1) {
        int str_len=read(sock, total_message, NAME_SIZE + BUF_SIZE);        //read message from server
        
        if(str_len==0)      //if closed return
            return;
 
        total_message[str_len]=0;
        fputs(total_message, stdout);   //printout data
    }
}
 

void Output_routine(int sock, char *message) {          //transmit message to server
    char total_message[NAME_SIZE + BUF_SIZE];
    while(1) {
        fgets(message, BUF_SIZE, stdin);        //input message
        
        if(!strcmp(message,"q\n") || !strcmp(message,"Q\n")) {      //if input message is q or Q then half close
            shutdown(sock, SHUT_WR);
            return;
        }
        
        sprintf(total_message, "%s %s", name, message);     //print name and message
        
        write(sock, total_message, strlen(total_message));      //transmit input data to server
    }
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
