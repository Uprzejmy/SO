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

key_t key;
int queueId;

void initializeGlobals();
void createMessageQueue();
void deleteMessageQueue();

void initializeGlobals()
{
  key = ftok("server.out",10);
}

void createMessageQueue()
{
  queueId = msgget(key, 0600|IPC_CREAT|IPC_EXCL);
  if(queueId == -1)
  {
    printf("Blad tworzenia kolejki, moze istniec kolejka o kluczu: %d\n",key);
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


int main()
{
  initializeGlobals();

  createMessageQueue();

  sleep(10);

  deleteMessageQueue();

  return 0;
}