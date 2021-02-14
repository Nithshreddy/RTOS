#include<stdio.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include <unistd.h>

struct sockaddr_in serv; // socket variable
int fd; // file descripter
int conn; // connection descripter
char message[100] = "";

//main function.
int main(int argc, char const *argv[]) {
  char buffer[1024] = {0};
  int valread;
  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Socket creation error \n");
		return -1;
	}
  //fd = socket(AF_INET, SOCK_STREAM, 0);
  serv.sin_family = AF_INET;
  int port_no = atoi(argv[3]);
  printf("%d\n",port_no );
  serv.sin_port = htons(port_no);
  printf("%s\n",argv[2] );
  if(inet_pton(AF_INET, argv[2], &serv.sin_addr)<=0)
	{
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}
  //inet_pton(AF_INET, argv[2], &serv.sin_addr); //binding client to local host
  //connect(fd, (struct sockaddr *)&serv, sizeof(serv));
  if (connect(fd, (struct sockaddr *)&serv, sizeof(serv)) < 0)
	{
		printf("\nConnection Failed \n");
		return -1;
	}
  while(1) {

    if(fork()==0){
      //printf("Enter a message: ");
      fgets(message, 100, stdin);
      send(fd, message, strlen(message),0);
    }
    else{
      valread = read(fd,buffer,1024);
      printf("Server: %s\n",buffer);
    }
  }

  return 0;
}