#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUF_SIZE 1024           //buffer size
#define MAX_CLNT 256            //max client
#define NAME_SIZE 20            //name size

typedef struct _user{           //user structure
    int usd;            //user discripter number
    char unick[NAME_SIZE];          //user name
}User;

void * handle_clnt(void * arg);             //handle_client method
void send_msg(User us, char * msg, int len);            //send_msg method
void error_handling(char * msg);                    //error_handling method
void whisper_msg(User us, char * msg, int len);         // whisper_msg method

int clnt_cnt=0;                     //current number of client
User clnt_socks[MAX_CLNT];          //array of client's information
pthread_mutex_t mutx;               //mutex to synchronization

int main(int argc, char *argv[])
{
    //setup variables
	int serv_sock, clnt_sock;               //serv_sock, clnt_sock
	struct sockaddr_in serv_adr, clnt_adr;          //server address, client address
	unsigned int clnt_adr_sz;               //client address size
    User user;                              //user
	pthread_t t_id;                         //thread
    
	if(argc!=2) {                       //get 2parameters from user
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
  
	pthread_mutex_init(&mutx, NULL);                //initialize mutex
    
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);      //create server socket
    
	memset(&serv_adr, 0, sizeof(serv_adr));         //initialize server address
	serv_adr.sin_family=AF_INET; 
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1)     //assign server'address information to socket
		error_handling("bind() error");
	if(listen(serv_sock, 5)==-1)                //listen
		error_handling("listen() error");
    
	
	while(1)
	{
		clnt_adr_sz=sizeof(clnt_adr);
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr,&clnt_adr_sz);      //accept connection request
        
        char name[NAME_SIZE]="";                    //declare name buffer
        read(clnt_sock, name, sizeof(name));        //read name that client has sent
        printf("User @%s has entered the chat\n",name);     //print clinet has been connected
        user.usd=clnt_sock;                             //save client socket number
        strcpy(user.unick,name);                        //save name
        
		pthread_mutex_lock(&mutx);                  //mutex lock
		clnt_socks[clnt_cnt++]=user;                //increase number of client
		pthread_mutex_unlock(&mutx);                //mutex unlock
	
		pthread_create(&t_id, NULL, handle_clnt, &user);         //create thread for user
		pthread_detach(t_id);                                   //release thread resource
	}
	close(serv_sock);                   //close server socket
	return 0;
}
	
void* handle_clnt(void * arg)
{
	User us=*(User*)arg;                    //User us
	int str_len=0, i;
	char msg[BUF_SIZE];                     //declare msg buffer
	
    while((str_len=read(us.usd, msg, sizeof(msg)))!=0){     //if client send message
        if(!strncmp(msg,"@all",4)){                 //if @all
            send_msg(us, msg, sizeof(msg));             //send_msg to all
        }
        else if(!strncmp(msg,"@",1)){               //if @someone
            whisper_msg(us, msg, sizeof(msg));      //whisper_msg
        }
        else{                                       //else
            char m[BUF_SIZE]="use @ to send message\n";
            write(us.usd, m, strlen(m));            //send exception to client
            continue;
        }
    }

	pthread_mutex_lock(&mutx);              //mutex lock
	for(i=0; i<clnt_cnt; i++)   // remove disconnected client
	{
        if (us.usd == clnt_socks[i].usd)
		{
			while (i < clnt_cnt - 1)
			{
				clnt_socks[i] = clnt_socks[i + 1];
				i++;
			}
			break;
		}
	}
	clnt_cnt--;                 //decrease number of client
	pthread_mutex_unlock(&mutx);            //mutex unlock
    close(us.usd);                      //close client socket
    printf("User @%s has left the chat\n",us.unick);            //print user has left
	return NULL;
}
    
void whisper_msg(User us, char *msg, int len){          //whisper method
    char message[NAME_SIZE+BUF_SIZE]="";            //message buffer
    char temp[NAME_SIZE+BUF_SIZE];                  //temp buffer
    char temp_name[NAME_SIZE]="";                   //temp_name buffer who to send msg
    
    sprintf(message, "[%s]", us.unick);             //save name in message buffer
    
    strcpy(temp, msg);          //save msg that client sent in temp
    strtok(msg, " ");           //get first token of msg
    
    
    strncpy(temp_name, msg+1, strlen(msg));  //detach '@' and save name in temp_name buffer
    strncpy(msg, temp+strlen(temp_name)+1, strlen(temp));       //get just the message from temp and save in msg buffer
    
    strcat(message, msg);       //attach message(name that which client sent) and msg(just msg)
    
    pthread_mutex_lock(&mutx);          //mutex lock
    for(int i=0; i<clnt_cnt; i++){
        if(strcmp(clnt_socks[i].unick,temp_name)==0){       //if find same name who to send
            write(clnt_socks[i].usd, message, strlen(message));     //send message to destination
            break;
        }
        if(i==clnt_cnt-1){                              //if no one found
            char m[BUF_SIZE]="Target user not found!\n";
            write(us.usd, m, strlen(m));                //send back not found msg to sender
            break;
        }
    }
    pthread_mutex_unlock(&mutx);            //mutex unlock
    
}

void send_msg(User us, char * msg, int len)   // send to all
{
    char temp[BUF_SIZE+BUF_SIZE]="";                //temp buffer
    char message[NAME_SIZE+BUF_SIZE];               //message buffer
    sprintf(message, "[%s]", us.unick);             //save name that who is sending in message buffer
    strncpy(temp, msg+4, sizeof(temp)-1);           //save just message in temp
    strcat(message,temp);                           //attach name and message
    
	pthread_mutex_lock(&mutx);              //mutex lock
	for(int i=0; i<clnt_cnt; i++)
        write(clnt_socks[i].usd, message, strlen(message));     //send to all
	pthread_mutex_unlock(&mutx);            //mutex unlock
}
    
void error_handling(char * msg)         //error handling
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
