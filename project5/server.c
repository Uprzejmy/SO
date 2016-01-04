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
void createMessageQueue();
void deleteMessageQueue();
void displayMessage(struct Message* message);
void receiveMessage(struct Message* message);
void sendMessage(struct Message* message);

void initializeGlobals()
{
  key = ftok("server.out",10);
  messageSize = MESSAGE_SIZE*sizeof(char)-sizeof(long int);
}

void createMessageQueue()
{
  queueId = msgget(key, 0600|IPC_CREAT|IPC_EXCL);
  if(queueId == -1)
  {
    printf("Blad tworzenia kolejki, moze juz istniec kolejka o kluczu: %d\n",key);
    exit(1);
  }
  
  printf("Utworzona kolejka o id: %d\n",queueId);
}

void deleteMessageQueue()
{

  if(msgctl(queueId, IPC_RMID, NULL) == -1)
  // awakening all waiting reader and writer processes (with an error return and errno set to EIDRM
  {
    printf("Blad usuwania kolejki o id: %d\n",queueId);
    exit(1);
  }

  printf("Usunieta kolejka o id: %d\n",queueId);
}

void displayMessage(struct Message* message)
{
  printf("Nadawca: %ld\n",message->sender);
  printf("Odbiorca: %ld\n",message->receiver);
  printf("Tresc: %s\n\n",message->content);
}

void receiveMessage(struct Message* message)
{
  int receiveError;
  receiveError = msgrcv( queueId, message, messageSize, 1, 0);
  if(receiveError == -1)
  {
    printf("Blad podczas odbierania komunikatu\n");
  }

}

void sendMessage(struct Message* message)
{
  message->receiver = message->sender;
  message->sender = 1;
  sprintf(message->content, "czesc, tu odpowiedz od serwera");

  msgsnd( queueId, message, messageSize, 0);
}


int main()
{
  char c;
  struct Message message;

  initializeGlobals();

  createMessageQueue();

  
  printf("przed odebraniem wiadomosci\n");
  receiveMessage(&message);
  printf("przed wyswietleniem wiadomosci\n");
  displayMessage(&message);
  printf("przed wyslaniem wiadomosci\n");
  sendMessage(&message);


  printf("wcisnij dowolny przycisk aby zakonczyc..\n");
  scanf(" %c",&c);
  deleteMessageQueue();

  return 0;
}