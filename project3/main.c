#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int semafor;

static void createSemafor(void);
static void deleteSemafor(void);
static void setSemafor(void);

//  arg[0] - nazwa programu - main
//  arg[1] - sciezka do programu wywolywanego
//  arg[2] - nazwa wywolywanego programu
//  arg[3] - ilosc procesow
//  arg[4] - ilosc przejsc przez sekcje krytyczna
// ./main /home/ubuntu/Documents/Sysop/project3/program program 4 5 NULL
int main(int argc, char* argv[])
{
  int i,id,cpid,execEr,status,pid,processes,strtolErr;
  char semaforArg[20];
  char location[120];

  if(argc!=5)
  {
    fprintf(stdout,"nieprawidlowa ilosc argumentow programu main! Powinno byc 4, jest: %d\n",argc-1);
    fprintf(stdout,"1: %s\n",argv[1]);
    fprintf(stdout,"2: %s\n",argv[2]);
    fprintf(stdout,"3: %s\n",argv[3]);
    fprintf(stdout,"4: %s\n",argv[4]);
    exit(1);
  }

  processes = strtol(argv[3],NULL,0);
  if(!processes>0)
  {
    fprintf(stdout,"Podana ilosc procesow mniejsza od 0 lub blad konwersji strtol\n");
    exit(4);
  }

  if(!strtol(argv[3],NULL,0)>0)
  {
    fprintf(stdout,"Podana ilosc przejsc przez sekcje krytyczna mniejsza od 0 lub blad konwersji strtol\n");
    exit(5);
  }


  createSemafor();
  setSemafor();

  sprintf(semaforArg, "%d",semafor);

  for(i=0; i<processes; i++)
  {
    id = fork();
    if(id == -1)
    {
      printf("Nieprawidlowe wywolanie fork()\n");
      exit(2);
    }
    if(id == 0)
    {
      execEr = execl(argv[1],argv[2],semaforArg,argv[4],NULL);
      //execEr = execl("/home/ubuntu/Documents/Sysop/project3/program","program",arg1, arg2, NULL);
      if(execEr == -1)
      {
        printf("Blad wywolania execl\n");
        exit(3);
      }
    }
  }

  for(i=0; i<processes; i++)
  {
    cpid = wait(&status);
    if(cpid == -1)
    {
      printf("Blad potomka kod powrotu potomka to: %d\n", status/256);
    }
    else
    {
      printf("Potomek zakonczyl sie poprawnie, id: %d\n", cpid);
      printf("kod powrotu potomka to: %d\n", status/256);
    }
    
  } 

  deleteSemafor();

  return 0;
}

static void createSemafor(void)
{
  semafor=semget(10,1,0777|IPC_CREAT);
  if (semafor==-1) 
  {
    printf("Nie moglem utworzyc nowego semafora.\n");
    exit(EXIT_FAILURE);
  }
  else
  {
    printf("Semafor zostal utworzony : %d\n",semafor);
  }
}

static void setSemafor(void)
{
  int ustaw_sem;
  ustaw_sem=semctl(semafor,0,SETVAL,1);
  if (ustaw_sem==-1)
  {
    printf("Nie mozna ustawic semafora.\n");
    exit(EXIT_FAILURE);
  }
  else
  {
    printf("Semafor zostal ustawiony.\n");
  }
}

static void deleteSemafor(void)  
{
  int sem;
  sem=semctl(semafor,0,IPC_RMID);
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
