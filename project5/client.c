
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <errno.h>

#define MESSAGE_SIZE 128

key_t key;
int queueId;
int messageSize;

struct Message
{
  long int receiver;
  long int sender;
  char content[MESSAGE_SIZE];
};

void initializeGlobals();
void joinMessageQueue();
void displayMessage(struct Message* message);

void* senderThread();
void* receiverThread();

void initializeGlobals()
{
  key = ftok("keyFile",10);
  messageSize = MESSAGE_SIZE*sizeof(char)-sizeof(long int);
}

void joinMessageQueue()
{
  queueId = msgget(key, 0600|IPC_CREAT);
  if(queueId == -1)
  {
    printf("Blad dolaczenia do kolejki o kluczu: %d\n",key);
    exit(1);
  }
  
  printf("Dolaczono do kolejki o id: %d\n",queueId);
}

void displayMessage(struct Message* message)
{
  printf("Nadawca: %ld\n",message->sender);
  printf("Odbiorca: %ld\n",message->receiver);
  printf("Tresc: %s\n\n",message->content);
}

void receiveMessage(struct Message* message)
{
  //jak server przerwie dzialanie to caly czas czyta ta sama wiadomosc
  msgrcv( queueId, message, messageSize, (long int) getpid(), 0);
}

void sendMessage(int count)
{
  struct Message message;

  message.receiver = 1;
  message.sender = getpid();
  sprintf(message.content, "czesc, tu komunikat %d od procesu %ld", count, (long int) getpid());

  msgsnd( queueId, &message, messageSize, 0);
}

void* senderThread()
{
  int i;

  for(i=0;i<3;++i)
  {
    printf("Wysylam wiadomosc nr: %d ...\n",i);
    sendMessage(i);
    printf("Wyslano\n");
    sleep(1);
  }

  pthread_exit(0);
}

void* receiverThread()
{
  int i;
  struct Message message;

  //for(i=0;i<10;++i)
  while(1)
  {
    printf("przed odebraniem wiadomosci\n");
    receiveMessage(&message);
    printf("przed wyswietleniem wiadomosci\n");
    displayMessage(&message);
    sleep(2);
  }

  pthread_exit(0);
}

void createThread(pthread_t* threadId, void* threadFunction)
{
  int threadError;

  threadError = pthread_create(threadId, NULL, threadFunction, NULL);
  if (threadError != 0)
    printf("\nNie udalo sie stworzyc watku :[%s]\n", strerror(threadError));
  else
    printf("\nUdalo sie stworzyc watkek\n");
}

void waitThread(pthread_t threadId)
{
  int threadError;
  int** threadStatus;

  threadStatus = malloc(sizeof(*threadStatus));
  *threadStatus = malloc(sizeof(**threadStatus));

  threadError = pthread_join(threadId, (void**) threadStatus);
  if (threadError != 0)
  {
    printf("\nNie udalo sie dolaczyc watku :[%s]", strerror(threadError));
    printf("\nNie udalo sie dolaczyc watku :[%d]", ** (int**)threadStatus);
  }
  else
    printf("\nUdalo sie dolaczyc watek\n");

  free(*threadStatus);
  free(threadStatus);
}

int main()
{
  int i;
  int threadError;
  int** threadStatus;
  pthread_t senderThreadId;
  pthread_t receiverThreadId;

  initializeGlobals();

  joinMessageQueue();

  createThread(&senderThreadId, &senderThread);
  createThread(&receiverThreadId, &receiverThread);

  waitThread(senderThreadId);
  waitThread(receiverThreadId);

  return 0;
}