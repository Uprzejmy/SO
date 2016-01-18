#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

int pipeDescriptors[2];

void createProducers(int n);
void createConsumers(int n);
void consumer();
void producer();
void generateFile();

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
      producer();
      /*
      execEr = execl("./producent.out","producent.out",NULL);
      if(execEr == -1)
      {
        printf("Blad wywolania execl\n");
        exit(2);
      }
      */
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
      consumer();

      /*
      execEr = execl("./konsument.out","konsument.out",NULL);
      if(execEr == -1)
      {
        printf("Blad wywolania execl\n");
        exit(2);
      }
      */
      exit(0);
    }
  }
}

void consumer()
{
  char outputFilename[32];
  char c;
  FILE* fop;

  close(pipeDescriptors[1]);//zamykam czytanie

  sprintf(outputFilename,"output/%ld.output",(long int)getpid());

  fop = fopen(outputFilename,"w");
  if(!fop)
  {
    printf("Błąd otwarcia pliku wynikowego\n");
    exit(1);
  }

  while(read(pipeDescriptors[0],&c,sizeof(char)))
    putc(c,fop);
  
  close(pipeDescriptors[0]);

  return;
}

void producer()
{
  char inputFilename[32];
  char c;
  FILE* fip;

  close(pipeDescriptors[0]);//zamykam czytanie

  generateFile(inputFilename);

  fip = fopen(inputFilename,"r");
  if(!fip)
  {
    printf("Błąd otwarcia pliku z danymi!\n");
    exit(1);
  }

  while((c = getc(fip))!=EOF)
    write(pipeDescriptors[1],&c,sizeof(char));
  
  close(pipeDescriptors[1]);

  return;
}

void generateFile(char* inputFilename)
{
  FILE* fop;
  int i,numberOfChars;
  char c;

  srand((int)getpid());

  numberOfChars = rand() % 5000 + 1;//bede generowal pliki od 1 do 50 znakow
  sprintf(inputFilename,"input/%ld.input",(long int)getpid());

  fop = fopen(inputFilename,"w");
  if(!fop)
  {
    printf("Błąd otwarcia pliku!\n");
    exit(1);
  }

  for(i=0;i<numberOfChars;i++)
  {
    //znaki ascii drukowalne od 32-126
    c = rand() % (126-32) + 32;
    fprintf(fop,"%c",c);
  }

  fclose(fop);
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

  pipe(pipeDescriptors);

  createProducers(numberOfProducers);
  createConsumers(numberOfConsumers);

  close(pipeDescriptors[0]);
  close(pipeDescriptors[1]);
  

  for(i=0; i<numberOfProducers+numberOfConsumers; i++)
  {
    cpid = wait(&status);
    if(cpid == -1)
    {
      printf("Blad potomka kod powrotu potomka to: %d\n", status/256);
    }
    else
    {
      //printf("Potomek zakonczyl sie poprawnie, id: %d\n", cpid);
    }
    
  } 
  

  return 0;
}
