#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define LENGTH 2048
#define MAX_CLIENTS 100
#define BUFFER_SZ 2048

struct message_type{
  int chatP_G;
  char message[BUFFER_SZ];
  char senderName[BUFFER_SZ];
  char ReceiverName[BUFFER_SZ];
};

// Global variables
volatile sig_atomic_t flag = 0;
int sockfd = 0;
char name[32];

void str_overwrite_stdout() {
  printf("%s", "> ");
  fflush(stdout);
}

void catch_ctrl_c_and_exit(int sig) {
    flag = 1;
}

void str_trim_lf (char* arr, int length) {
  int i;
  for (i = 0; i < length; i++) { // trim \n
    if (arr[i] == '\n') {
      arr[i] = '\0';
      break;
    }
  }
}

void send_msg_handler(){
    while(1){
    int chat_type;
    printf("[1]Group_chat / [0]Private_chat \n");
    scanf("%d", &chat_type);

// Private chat
 if(chat_type == 0){
      char nameR[BUFFER_SZ];
      printf("Enter the name you want to send: \n");
      scanf("%s",&nameR);
      printf("☑️ \n");

      char message[LENGTH] = {};
	    char buffer[LENGTH] = {};

      struct message_type mt;

      printf("Enter Message to %s: \n", nameR);
      scanf("%s", &message);
      str_trim_lf(message, LENGTH);

      // Send the Message
      sprintf(buffer, "%s: %s\n", name, message);
      // Printing the buffer
      printf("%s",buffer);

      mt.chatP_G = 1;
      strcpy(mt.ReceiverName, nameR);
      strcpy(mt.senderName, name);
      strcpy(mt.message, buffer);

      int n = send(sockfd, &mt, sizeof(mt), 0);
    }

//Group Chat
 else if(chat_type == 1){

   char message[LENGTH] = {};
   char buffer[LENGTH] = {};

   struct message_type mt;

   // Taking Message input and removing /0
   printf("Message to the group: \n");
   // fgets(message, LENGTH, stdin);
   scanf("%s", &message);
   str_trim_lf(message, LENGTH);

   // It the input exit?
   // Send the Message
   sprintf(buffer, "%s: %s\n", name, message);
   printf("%s",buffer);
   mt.chatP_G = 0;
   char temp[BUFFER_SZ] = "all";
   strcpy(mt.ReceiverName, temp);
   strcpy(mt.senderName, name);
   strcpy(mt.message, buffer);
   int n = send(sockfd, &mt, sizeof(mt), 0);
  }
 }
}
void recv_msg_handler() {
	char message[LENGTH] = {};
  while (1) {
		int receive = recv(sockfd, message, LENGTH, 0);
    if (receive > 0) {
      printf("%s", message);
      str_overwrite_stdout();
    } else if (receive == 0) {
			break;
    } else {
			// -1
		}
		memset(message, 0, sizeof(message));
  }
}

int main(int argc, char **argv){
  if(argc !=2){
    printf("[!] Error in arguments");
  }

  char *ip = "127.0.0.1";
  int port = atoi(argv[1]);

  // Enter Client name
  printf("Enter your name: ");
  // Taking in the name of length 32
  fgets(name, 32, stdin);
  str_trim_lf(name, strlen(name));
  printf("[+] Name joined \n");

  // Socket Connection
  struct sockaddr_in server_addr;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(ip);
  server_addr.sin_port = htons(port);

  // Connect to Server
  int err = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (err == -1) {
		printf("[!] ERROR while connecting via socket\n");
		return EXIT_FAILURE;
	}
  printf("[+] Connected the socket \n");

  //[1] Socket connected so Send name
  send(sockfd, name, 32, 0);

//Threading to avoid the traffic in between clients & server

//Sending thread
  pthread_t send_msg_thread;
  if(pthread_create(&send_msg_thread, NULL, (void *) send_msg_handler, NULL) != 0){
		printf("ERROR: pthread\n");
    return EXIT_FAILURE;
	}

  // Receiving Thread
  pthread_t recv_msg_thread;
  if(pthread_create(&recv_msg_thread, NULL, (void *) recv_msg_handler, NULL) != 0){
		printf("ERROR: pthread\n");
		return EXIT_FAILURE;
	}

  	while (1){
		if(flag){
			printf("%s Disconnected", name);
			break;
    }
	}

	close(sockfd);
  return EXIT_SUCCESS;
}
