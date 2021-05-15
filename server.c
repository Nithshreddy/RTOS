#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include "message.h"
#include "pulse_audio.h"

#define PORT 8888
struct Message Msg,sendMsg;
char members[100][20]; // members of group
//int sel;
//int msgtype;
int client_index[100]; // array to store names of clients
int n=0,member_count=0;
void *connection_handler(void *socket_desc)
{
  int i,j;
  int cl_id = *(int*)socket_desc; //client socket descriptor
  //char name[25],send_name[25];
  int sendcl_id;
  int read_size =0;
  while(read_size = (recv(cl_id,&Msg,sizeof(Msg),0)) > 0){
    if(Msg.voice_or_text == 1){
      for(i = 0; i < member_count;i++)
      {
        if(Msg.name == members[i]){
          goto here;}
        }
        if(i == member_count){
          strcpy(members[member_count++],Msg.name);
          //printf("%d\n",member_count);
	        client_index[n]=cl_id;
	        n++;
        }
      here :

        //char client_message[2000];
        //while((read_size =recv(cl_id , client_message ,sizeof(client_message), 0))>0){ //Receive a message from client
        //recv(cl_id ,&sel,sizeof(sel),0);
        //printf("%d\n",ntohl(sel));
      strcpy(sendMsg.name,Msg.name);

      sendMsg.voice_or_text = 1;
      strcpy(sendMsg.msg, Msg.msg);
      if(ntohl(Msg.msgtype) == 1){
        sendMsg.msgtype = htonl(1);
        //Send message to  all remaining clients in group
	      printf("%s\n",sendMsg.msg);
	      for(i=0;i<n;i++){
	         if(client_index[i] != cl_id){ // to send message all the remaining client
             //send(client_index[i], client_message , sizeof(client_message),0);
             //send(client_index[i],&msgtype,sizeof(msgtype),0);
             //send(client_index[i], name , sizeof(name),0);
             send(client_index[i],&sendMsg,sizeof(sendMsg),0);
	          }
	       }
      }
      else { // send message to a single person
        sendMsg.msgtype = htonl(0);
        //recv(cl_id ,send_name,sizeof(send_name),0);
        //printf("%s\n",send_name);
        for(i=0;i<member_count;i++){
          if(strcmp(members[i],Msg.recipient_id)==0){
            //printf("%s\n",members[i]);
	          sendcl_id = client_index[i]; // to send message to particlualr client
            //send(sendcl_id, client_message , sizeof(client_message),0);
            //send(client_index[i],&msgtype,sizeof(msgtype),0);
            //send(sendcl_id, name , sizeof(name),0);
            send(sendcl_id,&sendMsg,sizeof(sendMsg),0);
	        }
	      }
      }
    }

    //else{} voice message
   }
   if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);

    for(i = 0; i < n; i++) {
		if(client_index[i] == cl_id) {
			j = i;
			while(j < n-1) {
				client_index[j] = client_index[j+1];
				j++;
			}
		}
	}
	member_count--;
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
}


int main(int argc, char *argv[])
{
    int socket_desc ,client_sock, *new_sock,i;
    pthread_t sendt,recvt;
    int opt=1;
    struct sockaddr_in serv_addr,client;
    int addrlen = sizeof(struct sockaddr_in );

    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }

    puts("Socket created");
    if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt))){
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    bind(socket_desc, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    listen(socket_desc, 150);

    while((client_sock = accept(socket_desc, (struct sockaddr *)&client,
                       (socklen_t*)&addrlen)))
    {

        if( pthread_create( &recvt , NULL ,  connection_handler , (void*) &client_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
	    else  // added else
            puts("Handler assigned");

     }
return 0;
}
