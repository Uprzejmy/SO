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
#include <time.h>

int memoryKey = 10;
int memorySize = sizeof(char);
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
void detachMemory();


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
  struct sembuf bufor_sem;
  bufor_sem.sem_num=nr;
  bufor_sem.sem_op=-1;
  bufor_sem.sem_flg=SEM_UNDO;
  if (semop(semaphore,&bufor_sem,1)==-1) 
  {
    if(errno==EINTR)
    {
      printf("Konsument - Wznowienie procesu\n");
      semafor_p(nr);
    }
    else
    {
      printf("Konsument - Nie moglem zablokowac sekcji krytycznej nr %d\n",nr);
      exit(EXIT_FAILURE);
    }
    
  }
  else
  {
    printf("Konsument - Sekcja krytyczna nr %d zablokowana.\n",nr);
  }
}

static void semafor_v(int nr)
{
  struct sembuf bufor_sem;
  bufor_sem.sem_num=nr;
  bufor_sem.sem_op=1;
  bufor_sem.sem_flg=SEM_UNDO;
  if (semop(semaphore,&bufor_sem,1)==-1) 
  {
    if(errno==EINTR)
    {
      printf("Konsument - Wznowienie procesu\n");
      semafor_v(nr);
    }
      
    printf("Konsument - Nie moglem zablokowac sekcji krytycznej nr %d\n",nr);
    exit(EXIT_FAILURE);
  }
  else
  {
    printf("Konsument - Sekcja krytyczna nr %d odblokowana.\n\n",nr);
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


int main()
{
  double wait;
  int i;
  FILE* fop;

  srand(time(NULL));

  fop=fopen("output.txt", "w");
  if(!fop)
  {
    printf("Blad otwarcia pliku \"output.txt\"\n");
    exit(1);
  }

  //uzyskanie dostepu do zbioru semaforow (praca wykluczajaca konsument-producent)
  createSemafor(2);

  //uzyskanie dostepu do pamieci wspoldzielonej
  upd();
  upa();

  while(1)
  {
    semafor_p(1); 
    if(!(*address))
    {
      //niech tylko zapisze do notesu i ide
      break;
    }
    printf("Jestem w trakcie konsumpcji ...\n");
    wait = ((double)rand()) / RAND_MAX * 4;
    printf("Konsumpcja potrwa %lf sekund\n",wait);
    sleep(wait);
    fprintf(fop,"%c",*address);
    printf("Skonsumowalem!\n");
    semafor_v(0); //otwieram dostep 0 - producentowi
  }

  printf("Konsument wychodzi..\n");
  //skonczylem zapisywac do notesu, producent moze skladac stragan
  detachMemory();
  semafor_v(0);
  
  return 0;
}