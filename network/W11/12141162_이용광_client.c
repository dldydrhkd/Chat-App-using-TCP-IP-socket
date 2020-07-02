#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
	
#define BUF_SIZE 1024                //buffer size
#define NAME_SIZE 20                //name_size
	
void * send_msg(void * arg);           //send_msg method
void * recv_msg(void * arg);           //recv_msg method
void error_handling(char * msg);        //error handling method
	
char name[NAME_SIZE]="[DEFAULT]";       //default name
char msg[BUF_SIZE];                     //declare msg buffer
	
int main(int argc, char *argv[])        //main
{
    //setup variables
	int sock;
	struct sockaddr_in serv_addr;           //server addres variable
	pthread_t snd_thread, rcv_thread;       //threads for send_msg and rcv_msg
	void * thread_return;                   //thread return variable
    
	if(argc!=4) {                           //get 4 parameters from user
		printf("Usage : %s <IP> <port> <name>\n", argv[0]);
		exit(1);
	 }
	
	sprintf(name, "%s", argv[3]);           //save name
    
	sock=socket(PF_INET, SOCK_STREAM, 0);       // create client socket
	
	memset(&serv_addr, 0, sizeof(serv_addr));   //initialize socket address
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_addr.sin_port=htons(atoi(argv[2]));
	  
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)     //request connection to server
		error_handling("connect() error");
    write(sock, name, NAME_SIZE);                   //send server user's name
	pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);      //create thread for send_msg
	pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock);      //create thread for recv_msg
	pthread_join(snd_thread, &thread_return);               //wait until snd_tread work done
	pthread_join(rcv_thread, &thread_return);               //wait until recv_thread work done
	close(sock);                //close client socket
	return 0;
}
	
void * send_msg(void * arg)   // send thread main
{
	int sock=*((int*)arg);          //get arguments
	while(1) 
	{
		fgets(msg, BUF_SIZE, stdin);                //get message from user
		if(!strcmp(msg,"q\n")||!strcmp(msg,"Q\n"))  //if input is q then close socket
		{
			close(sock);
			exit(0);
		}
		write(sock, msg, strlen(msg));          //send message to server
	}
	return NULL;                        //end thread
}
	
void* recv_msg(void * arg)   // read thread main
{
	int sock=*((int*)arg);              //get arguments
	char name_msg[NAME_SIZE+BUF_SIZE];      //declare name+msg buffer
    int str_len;                    //declare name_msg length variable
	while(1)
	{
		str_len=read(sock, name_msg, NAME_SIZE+BUF_SIZE-1);     //read msg that server has sent
		if(str_len==-1)                 //if scoket closed
			return (void*)-1;
		name_msg[str_len]=0;
		fputs(name_msg, stdout);           //print out message
	}
	return NULL;            //end thread
}
	
void error_handling(char *msg)          //error handling
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
