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
int semFull;
int semEmpty;

static void createSemaphor(int count);
static void deleteSemaphor(int nr);
static void setSemaphor(int nr);
static void semafor_p(int nr);
static void semafor_v(int nr);
void upd();
void upa();
void detachMemory();
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

static void setSemafor(int nr)
{
  int setSem;
  setSem=semctl(semaphore,nr,SETVAL,1);
  if (setSem==-1)
  {
    printf("Nie mozna ustawic semafora.\n");
    exit(EXIT_FAILURE);
  }
  else
  {
    printf("Semafor zostal ustawiony.\n");
  }
}

static void deleteSemafor(int nr)  
{
  int sem;
  sem=semctl(semaphore,nr,IPC_RMID);
  if (sem==-1)
  {
    printf("Nie mozna usunac semafora.\n");
    exit(EXIT_FAILURE);
  }
  else
  {
    printf("Semafor zostal usuniety : %d\n",sem);
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
      printf("Pid: %d   Nie moglem odblokowac sekcji krytycznej.\n",getpid());
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

  int i;

  //ustawiam zbior semaforow (praca wykluczajaca konsument-producent)
  createSemafor(2);

  for(i=0;i<2;i++)
  {
    setSemafor(i);
  }
  semafor_p(1); //zamykam dostep 1 - konsumentowi
  
  //tworzenie pamieci wspoldzielonej
  upd();
  upa();

  for(i=0;i<3;i++)
  {
    semafor_p(0); 
    printf("Jestem w trakcie produkcji ...\n");
    fillInMemory();
    sleep(1);
    printf("Wyprodukowano!\n");
    semafor_v(1); //otwieram dostep 1 - konsumentowi
  }

  
  detachMemory();
  deleteSemafor(0);
  deleteSemafor(1);
  exit(EXIT_SUCCESS);
}
