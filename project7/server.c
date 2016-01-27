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
#include <pthread.h>

#define connectionsLimit 5
#define messageSize 10

int serverSocketId;
int messageLength;
struct sockaddr_in serverAddress;
int port = 1234;
char sizeFormat[10];


/*
struct sockaddr_in {
    short int          sin_family;  // Address family, AF_INET
    unsigned short int sin_port;    // Port number
    struct in_addr     sin_addr;    // Internet address
    unsigned char      sin_zero[8]; // Same size as struct sockaddr
}*/

void handleError(char name[])
{
    printf("%s\n",name);
    exit(1);
}

void handleMessage(char name[])
{
    printf("%s\n",name);
}

void sigintHandler(int signal)
{
  printf("\nServer stopped using Ctrl+C \n");
  close(serverSocketId);
  exit(0);
}

void initialize()
{
    signal(SIGINT, sigintHandler);
    
    if( (serverSocketId = socket(AF_INET , SOCK_STREAM , 0)) == -1) // AF_INET - ipv4 connection, SOCK_STREAM - two ends pipe-type connection
    {
        handleError("Error creating socket");
    }

    handleMessage("Socket created Successfully");

    //konfiguracja naglowkow tcp
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET; //protokól ipv4
    serverAddress.sin_addr.s_addr = INADDR_ANY; //INADDR_ANY (0.0.0.0) means any address for binding;
    serverAddress.sin_port = htons(port); //port (domyslnie - 45000)

    if (bind(serverSocketId, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) == -1) //rzutowanie jest konieczne, zeby socket byl uniwersalny
    {
        handleError("Error binding socket with an address");
    }

    handleMessage("Socket binded Successfully");

    if (listen(serverSocketId, connectionsLimit) == -1)
    {
        handleError("Error turning listener on");
    }

    handleMessage("Socket listener turned on Successfully");

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

void changeMessage(char message[messageSize])
{
  int i;

  for(i=0;i<messageSize;i++)
  {
    if(message[i] == '\0')
      break;

    message[i] = toupper(message[i]);
  }
}

void* handleClient(void* clientSocketIdParameter)
{
    char message[messageSize];
    int clientSocketId;
    int transmittedBytes;

    clientSocketId = *((int*) clientSocketIdParameter);

    while(1)
    {
        transmittedBytes = recv(clientSocketId,&message,messageSize,0);
        if(transmittedBytes == -1)
        {
            printf("Error receiving message, shutting client down\n");
            break;
        }
        if(transmittedBytes == 0)
        {
            printf("Empty message, client probably down\n");
            break;
        }

        printMessage(message);
        changeMessage(message);
        transmittedBytes = send(clientSocketId,&message,messageSize,0);
        if(transmittedBytes == -1)
        {
            printf("Error sending message, shutting client down\n");
            break;
        }
        if(transmittedBytes == 0)
        {
            printf("Empty message, client probably down\n");
            break;
        }

    }

    close(clientSocketId);
    pthread_exit(0);
}

int main()
{
    int errP;

    pthread_attr_t threadAttributes;
    pthread_t threadId;

    int sizeOfClient;
    struct sockaddr_in clientAddress;
    int clientSocketId;

    initialize();

    sizeOfClient = sizeof(clientAddress);

    pthread_attr_init(&threadAttributes);//tu moga byc bledy
    pthread_attr_setdetachstate(&threadAttributes, PTHREAD_CREATE_DETACHED); // tu tez


    while(1)
    {

        printf("Waiting for a client...\n");

        clientSocketId=accept(serverSocketId,(struct sockaddr*) &clientAddress, &sizeOfClient);

        errP = pthread_create(&threadId, &threadAttributes, &handleClient, &clientSocketId);
        if (errP != 0)
          printf("\nCan't create thread :[%s]", strerror(errP));
        else
          printf("\nClient connected\n");
    }

    
    

    return 0;
}
