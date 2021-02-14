#include<stdio.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<errno.h>
#include<sys/wait.h>
#include<pthread.h>
#include<signal.h>

struct sockaddr_in serv; // socket variable
int fd; // file descripter
int conn; // connection descripter
char message[100] = "";
void* send_func(void* n){
  int sd = *(int*)n;
  char msg[1024] = {0};
  fgets(msg,1024,stdin);
  send(sd,msg, strlen(msg),0);
  return 0;
}

void* recv_func(void* n){
  int rsd = *(int*)n;
  char rply[1024];
  read(rsd,rply,sizeof(rply));
  printf("Client: %s\n",rply );

  return 0;
}
void signal_handler(int n){
  char ans;

  if (n == SIGINT) {
        printf("exit Y/N\n");
        scanf("%c", &ans);
        if (ans == 'y' || ans == 'Y') {
            printf("The client exited gracefully\n");

            exit(0);
        }
    }
}
//main function:
int main(int argc, char *argv[]){
  //signal handler
  // struct sigaction *act;
  // act = (struct sigaction*) malloc(sizeof(struct sigaction));
  // act->sa_handler = signal_handler;
  // act->sa_flags = SA_RESTART;
  // sigaction(SIGINT,act,NULL);

  int fd, new_socket, valread;
	struct sockaddr_in serv;
	int opt = 1;
	int addrlen = sizeof(serv);
	char buffer[1024] = {0};
  pthread_t thread;
  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

  serv.sin_family = AF_INET;
  int port_no = atoi(argv[2]);
  printf("%d\n",port_no );
  serv.sin_port = htons(port_no); // port at which server will listen for connections
  serv.sin_addr.s_addr = INADDR_ANY;
  //fd = socket(AF_INET, SOCK_STREAM, 0);

  if (bind(fd, (struct sockaddr *)&serv,
								sizeof(serv))<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
  //bind(fd, (struct sockaddr *) &serv, sizeof(serv));
  if (listen(fd, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
  //listen(fd,5); //max 5 clients.
  //handle connections
  // if ((conn = accept(fd, (struct sockaddr *)&serv,
	// 				(socklen_t*)&addrlen))<0)
	// {
	// 	perror("accept");
	// 	exit(EXIT_FAILURE);
	// }

  //int dpid = fork();
  //printf("%d\n",dpid );

    signal(SIGINT,signal_handler);
    while((conn =accept(fd, (struct sockaddr *)&serv,
					(socklen_t*)&addrlen))>-1){

            printf("entered loop\n");
    // int pid=fork();
      while(1){

        pthread_create(&thread,NULL,send_func,(void*) &conn);
        pthread_create(&thread,NULL,recv_func,(void*) &conn);
      //exit(0);
      }
    }

  return 0;
}