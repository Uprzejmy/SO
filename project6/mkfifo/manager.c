#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

void createProducers(int n);
void createConsumers(int n);
void createFifo();

void createProducers(int n)
{
  pid_t id;
  int i,execEr;

  for(i=0; i<n; i++)
  {
    id = fork();
    if(id == -1)
    {
      printf("Nieprawidlowe wywolanie fork()\n");
      exit(1);
    }
    if(id == 0)//jezeli proces jest procesem potomnym
    {
      execEr = execl("./producent.out","producent.out",NULL);
      if(execEr == -1)
      {
        printf("Blad wywolania execl\n");
        exit(2);
      }
      
      exit(0);
    }
  }
}

void createConsumers(int n)
{
  pid_t id;
  int i,execEr;

  for(i=0; i<n; i++)
  {
    id = fork();
    if(id == -1)
    {
      printf("Nieprawidlowe wywolanie fork()\n");
      exit(1);
    }
    if(id == 0)//jezeli proces jest procesem potomnym
    {
      execEr = execl("./konsument.out","konsument.out",NULL);
      if(execEr == -1)
      {
        printf("Blad wywolania execl\n");
        exit(2);
      }

      exit(0);
    }
  }
}

void createFifo()
{
  if(mkfifo("transfer", 0600 | 0)==-1)
  {
    if(errno == EEXIST)
    {
      unlink("transfer");
      createFifo();
      return;
    }
    else
    {
      //dorzucic errno
      printf("Blad tworzenia fifo\n");
    }
    
  }
  else
  {
    printf("Stworzono fifo\n");
  }
}

int main(int argc, char* argv[])
{
  int i,cpid,status,numberOfProducers,numberOfConsumers;
  char cmd[120];

  if(argc!=3)
  {
    printf("nieprawidlowa ilosc argumentow programu! Powinno byc 2.\nPierwszy - ilosc producentow, Drugi - ilosc konsumentow\n");
    exit(3);
  }

  numberOfProducers = atoi(argv[1]);
  if(numberOfProducers <1)
  {
    printf("Zbyt malo producentow, powinien byc przynajmniej jeden\n");
    exit(4);
  }
  numberOfConsumers = atoi(argv[2]);
  if(numberOfConsumers <1)
  {
    printf("Zbyt malo konsumentow, powinien byc przynajmniej jeden\n");
    exit(4);
  }

  if(numberOfProducers > 200 || numberOfConsumers > 200)
  {
    printf("Proszę podac mniejsza ilosc procesow");
    exit(5);
  }

  //czesc wlasciwa programu

  createFifo();

  createProducers(numberOfProducers);
  createConsumers(numberOfConsumers);

  for(i=0; i<numberOfProducers+numberOfConsumers; i++)
  {
    cpid = wait(&status);
    if(cpid == -1)
    {
      printf("Blad, potomka kod powrotu potomka to: %d\n", status/256);
    }
    else
    {
      //printf("Potomek zakonczyl sie poprawnie, id: %d\n", cpid);
    }

    if(i==numberOfProducers)
    {
      unlink("transfer");
    }
    
  } 
  

  return 0;
}
