#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include "message.h"
#include "pulse_audio.h"




struct Message Msgstruct,recvMsg;

void sigintHandler(int sig_num)
{
    printf("exit(Y/N)");
    if(getchar()=='Y') exit(1);
}

void *recvmg(void *sock)
{
	int rcv_sock = *((int *)sock);
	//char msg[2000],put_msg[2000];
  //int msgtype;
	int len;
	while((len = recv(rcv_sock,&recvMsg,sizeof(recvMsg),0))>0) {
        //strcpy(put_msg,msg);
        //recv(rcv_sock,&msgtype,sizeof(msgtype),0);
        //recv(rcv_sock,msg,sizeof(msg),0);
        if(ntohl(recvMsg.voice_or_text == 1)){
          if( ntohl(recvMsg.msgtype) == 1){
              printf("%s: %s\n",recvMsg.name,recvMsg.msg);}
          else {
              printf("direct message from %s: %s\n", recvMsg.name,recvMsg.msg);}
        }
        else {
          //voice Message
          pa_simple *s = NULL;
          int err;
          // uint8 buf[1024];
          /* Create a new playback stream */
          if (!(s = pa_simple_new(NULL,recvMsg.name, PA_STREAM_PLAYBACK, NULL, "playback", &ss, NULL, NULL, &err))) {
            fprintf(stderr, __FILE__ ": pa_simple_new() failed: %s\n", pa_strerror(err));
            goto finish;
          }
          for (;;) {
            if (len == 0) /* EOF */
                break;
            fprintf(stderr, __FILE__": read() failed: %s\n", strerror(errno));
            goto finish;
            // playing the received data
            fprintf(stderr, __FILE__": read() failed: %s\n", strerror(errno));
            if (pa_simple_write(s, recvMsg.msg, sizeof(recvMsg.msg), &err) < 0) {
              fprintf(stderr, __FILE__": pa_simple_write() failed: %s\n", pa_strerror(err));
              goto finish;}
          }
          /* Make sure that every single sample was played */
          if (pa_simple_drain(s, &err) < 0) {
            fprintf(stderr, __FILE__": pa_simple_drain() failed: %s\n", pa_strerror(err));
            goto finish;}

         finish:

            if (s)
              pa_simple_free(s);

        }
   }
}
//*****************Main******************
int main(int argc, char const *argv[])
{
	int sock = 0;
	pthread_t recvt; // pthread for receiving message

	//char msg[2000] ={0}
  //Msgstruct.msg[BUFSIZE] ;

  struct sockaddr_in serv_addr;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Socket creation error \n");
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[1]));
	// Convert IPv4 and IPv6 addresses from text to binary form
	if(inet_pton(AF_INET, argv[2], &serv_addr.sin_addr)<=0)
	{
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("\nConnection Failed \n");
		return -1;
	}
    else
	    printf("connected");

    //char name[25],send_name[25];
    //int sel;
    strcpy(Msgstruct.name,argv[3]);
    //send(sock ,Msgstruct.name, sizeof(name),0);

	signal(SIGINT, sigintHandler);
	pthread_create(&recvt,NULL,recvmg,&sock);


	while(1){
    printf("Enter the message to be sent: ");
    fgets(Msgstruct.msg, sizeof(Msgstruct.msg), stdin);

    printf("Enter '0' for sending voice message and '1' text message");
    scanf("%d%*c",&Msgstruct.voice_or_text);
    if(Msgstruct.voice_or_text == 1){

	    //send(sock ,Msgstruct.msg, sizeof(Msgstruct.msg),0);
      printf("Enter '1' for group chat and '0' for direct message: ");
      scanf("%d%*c",&Msgstruct.msgtype);
      Msgstruct.msgtype = htonl(Msgstruct.msgtype);
      //printf("%d",ntohl(sel));
      //send(sock ,&Msgstruct.msgtype, sizeof(Msgstruct.msgtype),0);

      if(ntohl(Msgstruct.msgtype) == 0)
      {
          printf("Enter recipient name");
          scanf("%[^\n]%*c", Msgstruct.recipient_id);
          //send(sock ,Msgstruct.recipient_id, sizeof(Msgstruct.recipient_id),0);
      }
      // else{
      //   Msgstruct.recipient_id = -1;   // for grp message default value is -1
      // }
      send(sock,&Msgstruct,sizeof(Msgstruct),0); // sending entire structure
    }
    else{
      // voice message code here.
      printf("send Voice message\n");
      pa_simple *s = NULL;
      int error = 0;
      /* Create the recording stream */
      if (!(s = pa_simple_new(NULL, Msgstruct.name, PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &error))) {
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
        goto finish;
      }
      printf("pa_simple_new done\n");

      if (pa_simple_read(s, Msgstruct.msg, sizeof(Msgstruct.msg), &error) < 0) {
        fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
        printf("hulala\n");
        goto finish;
      printf("pa_simple_read done\n");
      }
      finish:
        if (s)
          pa_simple_free(s);
    }

  }

	pthread_join(recvt,NULL);
	return 0;
}
