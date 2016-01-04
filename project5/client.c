#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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

void initializeGlobals()
{
  key = ftok("server.out",10);
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

receiveMessage(struct Message* message)
{
  msgrcv( queueId, message, messageSize, getpid(), 0);
}

sendMessage(struct Message* message)
{
  message->receiver = 1;
  message->sender = getpid();
  sprintf(message->content, "czesc, tu nowy komunikat od procesu %ld",(long int) getpid());

  msgsnd( queueId, message, messageSize, 0);
}

int main()
{
  struct Message message;

  initializeGlobals();

  joinMessageQueue();
 
  printf("przed wyslaniem wiadomosci\n");
  sendMessage(&message);
  printf("przed odebraniem wiadomosci\n");
  receiveMessage(&message);
  printf("przed wyswietleniem wiadomosci\n");
  displayMessage(&message);

  return 0;
}