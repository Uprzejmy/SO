#include <stdio.h>  
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>

#define messageSize 10

struct sockaddr_in serverAddress;
int serverSocketId;
int port = 1234;
char host[] = "127.0.0.1";
char sizeFormat[10];

void handleError(char name[])
{
    printf("%s\n",name);
    exit(1);
}

void handleMessage(char name[])
{
    printf("%s\n",name);
}

void initialize()
{
  sprintf(sizeFormat,"%%%ds",messageSize);

  if( (serverSocketId = socket(AF_INET , SOCK_STREAM , 0)) == -1) // AF_INET - ipv4 connection, SOCK_STREAM - two ends pipe-type connection
  {
      handleError("Error naming server socket");
  }

  handleMessage("Server socket named successfully");

  //konfiguracja naglowkow tcp
  memset(&serverAddress, 0, sizeof(serverAddress));
  serverAddress.sin_family = AF_INET; //protokól ipv4
  serverAddress.sin_addr.s_addr = inet_addr(host); //INADDR_ANY (0.0.0.0) means any address for binding;
  serverAddress.sin_port = htons(port); //port (domyslnie - 45000)

  if (connect(serverSocketId,(struct sockaddr*) &serverAddress,sizeof(serverAddress))==-1)
  {
    handleError("Error connecting server socket");
  }

  handleMessage("Server socket connected successfully");
}

void printMessage(char message[messageSize])
{
  int i;

  for(i=0;i<messageSize;i++)
  {
    if(message[i] == '\0')
      break;

    printf("%c",message[i]);
  }

  printf("\n");
}

int main()
{
  char messageIn[messageSize];
  char messageOut[messageSize];
  int transmittedBytes;

  initialize();

  printf("Podaj tekst do przeslania serwerowi\n");

  while(scanf(sizeFormat,&messageIn))
  {
    transmittedBytes = send(serverSocketId,&messageIn,messageSize,0);
    if(transmittedBytes == -1)
    {
      printf("Error sending message - skip to next send\n");
      continue;
    }
    if(transmittedBytes == 0)
    {
      printf("Error connecting to server, qutting\n");
      close(serverSocketId);
      break;
    }

    transmittedBytes = recv(serverSocketId,&messageOut,messageSize,0);
    if(transmittedBytes == -1)
    {
      printf("Error receiving message - skip to next send\n");
      continue;
    }
    if(transmittedBytes == 0)
    {
      printf("Error connecting to server, qutting\n");
      close(serverSocketId);
      break;
    }

    printMessage(messageOut);
  }

  close(serverSocketId);
  

  return 0;
}
