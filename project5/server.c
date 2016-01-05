#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <signal.h>
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

//general functions
void initialize();
void sigintHandler(int signal);

//Message queue interface
void createMessageQueue();
void deleteMessageQueue();

//Single message interface
void displayMessage(struct Message* message);
void receiveMessage(struct Message* message);
void sendMessage(struct Message* message);

void initialize()
{
  //przejmuje kontrole nad ctrl+c zeby posprzatac po sobie w razie przerwania z zewnatrz, potem grzecznie koncze proces.
  signal(SIGINT, sigintHandler);

  key = ftok("keyFile",10);
  messageSize = MESSAGE_SIZE*sizeof(char)-sizeof(long int);
}

void sigintHandler(int signal)
{
  printf("\nSerwer przerwal dzialanie poprzez Ctrl+C \n");
  deleteMessageQueue();
  exit(1);
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

  initialize();

  createMessageQueue();

  while(1)
  {
    printf("Czekam na wiadomosc...\n");
    receiveMessage(&message);
    printf("Odebralem wiadomosc\n\n");

    displayMessage(&message);

    printf("Wysylam odpowiedz...\n");
    sendMessage(&message);
    printf("Wyslalem odpowiedz\n\n\n");
  }


  //w tym miejscu nikt raczej nie powinien sie znalezc, ale na wszelki wypadek..
  printf("wcisnij dowolny przycisk aby zakonczyc..\n");
  scanf(" %c",&c);

  deleteMessageQueue();

  return 0;
}