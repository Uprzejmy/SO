#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <string.h>

int memoryKey = 10;
int memorySize = 256;
int memory;
int detachment1;
int detachment2;
char* address;
int semaphor;

static void createSemaphor(void);
static void deleteSemaphor(void);
static void setSemaphor(void);
void upd();
void upa();
void detachMemory();
void fillInMemory();
void takeFromMemory();


void upd()
{
  memory=shmget(memoryKey,memorySize,0777|IPC_CREAT);
  if (memory==-1) 
    {
      printf("Problemy z utworzeniem pamieci dzielonej.\n");
      exit(EXIT_FAILURE);
    }
  else printf("Pamiec dzielona zostala utworzona : %d\n",memory);
}

void upa()
{
  address=shmat(memory,0,0);
  if (*address==-1) 
    {
      printf("Problem z przydzieleniem adresu.\n");
      exit(EXIT_FAILURE);
    }    
  else printf("Przestrzen adresowa zostala przyznana : %s\n",address);
}

void detachMemory()
{
  detachment1=shmctl(memory,IPC_RMID,0);
  detachment2=shmdt(address);
  if (detachment1==-1 || detachment2==-1)
    {
      printf("Problemy z odlaczeniem pamieci dzielonej.\n");
      exit(EXIT_FAILURE);
    }
  else printf("Pamiec dzielona zostala odlaczona.\n");
}

void fillInMemory()
{
  printf("Wpisz cos do pamieci :");
  scanf("%s",address);
}

void takeFromMemory()
{
  printf("Biore z pamieci : %s\n",address);
}


int main()
{
  upd();
  upa();
  fillInMemory();
  takeFromMemory();
  detachMemory();
  exit(EXIT_SUCCESS);
}
