#include<stdio.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<unistd.h>
#include<string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>
struct sockaddr_in serv; // socket variable
int fd; // file descripter
int conn; // connection descripter
char message[100] = "";

//main function:
int main(int argc, char *argv[]){
  int fd, new_socket, valread;
	struct sockaddr_in serv;
	int opt = 1;
	int addrlen = sizeof(serv);
	char buffer[1024] = {0};
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
  while((conn =accept(fd, (struct sockaddr *)&serv,
					(socklen_t*)&addrlen))>-1){

     printf("entered loop\n");
    // int pid=fork();
    printf("%d\n",valread );
      while (1){
        //printf("%d\n",valread );

        //strcpy("",buffer);// = "";
        if(fork()==0){
          //printf("Enter a message: ");
          fgets(message, 100, stdin);
          send(conn , message , strlen(message) , 0 );
      	  //printf("message sent\n");
        }
        else{
          valread=read(conn,buffer,1024);
          printf("Client: %s\n", buffer);
        }

      }
      //exit(0);
  }
  return 0;
}