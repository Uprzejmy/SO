#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <errno.h>

int memoryKey = 10;
int memorySize = 256;
int memory;
int detachment1;
int detachment2;
char* address;
int semaphore;

static void semafor_p(int nr);
static void semafor_v(int nr);
static void createSemafor(int count);

void upd();
void upa();
void fillInMemory();
void takeFromMemory();

static void createSemafor(int count)
{
  semaphore=semget(10,count,0777|IPC_CREAT);
  if (semaphore==-1) 
  {
    printf("Nie moglem utworzyc nowego semafora.\n");
    exit(EXIT_FAILURE);
  }
  else
  {
    printf("Semafor zostal utworzony : %d\n",semaphore);
  }
}

static void semafor_p(int nr)
{
  int zmien_sem;
  struct sembuf bufor_sem;
  bufor_sem.sem_num=nr;
  bufor_sem.sem_op=-1;
  bufor_sem.sem_flg=SEM_UNDO;
  zmien_sem=semop(semaphore,&bufor_sem,1);
  if (zmien_sem==-1) 
  {
    if(errno==EINTR)
    {
      printf("Pid: %d Wznowienie procesu\n",getpid());
      semafor_p(nr);
    }
    else
    {
      printf("Pid: %d   Nie moglem zablokowac sekcji krytycznej.\n",getpid());
      exit(EXIT_FAILURE);
    }
    
  }
  else
  {
    printf("Pid: %d   Sekcja krytyczna zablokowana.\n",getpid());
  }
}

static void semafor_v(int nr)
{
  int zmien_sem;
  struct sembuf bufor_sem;
  bufor_sem.sem_num=nr;
  bufor_sem.sem_op=1;
  bufor_sem.sem_flg=SEM_UNDO;
  if (semop(semaphore,&bufor_sem,1)==-1) 
  {
    if(errno==EINTR)
    {
      printf("Pid: %d Wznowienie procesu\n",getpid());
      semafor_v(nr);
    }
      
    printf("Pid: %d   Nie moglem zablokowac sekcji krytycznej.\n",getpid());
    exit(EXIT_FAILURE);
  }
  else
  {
    printf("Pid: %d   Sekcja krytyczna odblokowana.\n\n",getpid());
  }
}

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
  int i;

  //uzyskanie dostepu do zbioru semaforow (praca wykluczajaca konsument-producent)
  createSemafor(2);

  //uzyskanie dostepu do pamieci wspoldzielonej
  upd();
  upa();

  for(i=0;i<3;i++)
  {
    semafor_p(1); 
    printf("Jestem w trakcie konsumpcji ...\n");
    sleep(1);
    printf("Skonsumowalem!\n");
    takeFromMemory();
    semafor_v(0); //otwieram dostep 0 - producentowi
  }
  

  return 0;
}