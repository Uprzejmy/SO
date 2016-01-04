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
int semFull;
int semEmpty;

static void createSemaphor(int count);
static void deleteSemaphor();
static void setSemaphor(int nr);
static void semafor_p(int nr);
static void semafor_v(int nr);
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

static void deleteSemafor()  
{
  int sem;
  sem=semctl(semaphore,0,IPC_RMID);
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
  struct sembuf bufor_sem;
  bufor_sem.sem_num=nr;
  bufor_sem.sem_op=-1;
  bufor_sem.sem_flg=SEM_UNDO;
  if (semop(semaphore,&bufor_sem,1)==-1) 
  {
    if(errno==EINTR)
    {
      printf("Producent - Wznowienie procesu\n");
      semafor_p(nr);
    }
    else
    {
      printf("Producent - Nie moglem zablokowac sekcji krytycznej nr %d\n",nr);
      exit(EXIT_FAILURE);
    }
    
  }
  else
  {
    printf("Producent - Sekcja krytyczna nr %d zablokowana.\n",nr);
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
      printf("Producent - Wznowienie procesu\n");
      semafor_v(nr);
    }
      
    printf("Producent - Nie moglem zablokowac sekcji krytycznej nr %d\n",nr);
    exit(EXIT_FAILURE);
  }
  else
  {
    printf("Producent - Sekcja krytyczna nr %d odblokowana.\n\n",nr);
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
  FILE* fip;
  srand(time(NULL));

  fip=fopen("input.txt", "r");
  if(!fip)
  {
    printf("Blad otwarcia pliku \"input.txt\"\n");
    exit(1);
  }

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

  //przesylanie danych - produkcja,konsumpcja
  while(1)
  {
    semafor_p(0); 
    fscanf(fip, "%c", address);
    if(feof(fip))
    {
      //koniec towaru, zaczynam zwijac stragan
      break;
    }
    printf("Jestem w trakcie produkcji ...\n");
    wait = ((double)rand()) / RAND_MAX * 4;
    printf("Produkcja potrwa %lf sekund\n",wait);
    sleep(wait);
    printf("Wyprodukowano: %c\n",*address);
    semafor_v(1); //otwieram dostep 1 - konsumentowi
  }

  //wstawiam NULL na polke
  *address = 0;
  printf("Koncze produkcje!\n");
  semafor_v(1); //otwieram dostep 1 - konsumentowi

  //czekam az konsument skonczy zapisywac do swojego notesu, zeby zlozyc stragan
  semafor_p(0);
  //chyba nie musi byc ale dla jasnosci i pewnosci
  semafor_v(0);

  fclose(fip);
  detachMemory();
  deleteSemafor();
  exit(EXIT_SUCCESS);
}
