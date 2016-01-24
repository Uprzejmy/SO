#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

char fifoFilename[] = "transfer";
char inputFilename[128];
int numberOfChars = 0;

void generateFile()
{
  FILE* fop;
  int i,numberOfChars;
  char c;

  numberOfChars = rand() % 50 + 1;//bede generowal pliki od 1 do 50 znakow
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

void initialize()
{
  srand((unsigned int) getpid());

  generateFile();
}

void cleanup()
{

}

void putIntoFifo()
{
  int fifo;
  int n;
  FILE* fip;
  char c;
  int i;

  fip = fopen(inputFilename,"r");
  if(!fip)
  {
    printf("Błąd otwarcia pliku z danymi!\n");
    exit(1);
  }

  printf("Otwarlem plik z danymi\n");

  while(1)
  {
    fifo = open(fifoFilename,O_WRONLY);
    if(fifo == -1)
    {
      if(errno == ENXIO)
      {
        printf("ENXIO Czekam az ktos po drugiej stronie otworzy fifo do czytania\n");
        continue;
      }

      if(errno == EPIPE)
      {
        printf("EPIPE Czekam az ktos po drugiej stronie otworzy fifo do czytania\n");
        continue;
      }

      printf("Błąd otwarcia pliku fifo!\n");
      exit(1);
    }

    break;
  }

  //fscanf(fip,"%c",c);
  while(1)
  {
    fscanf(fip,"%c",&c);
    if(feof(fip))
      break;

    n = write(fifo,&c,sizeof(char));
    if(n == -1)
    {
      if(errno == EPIPE)
      {
        //printf("Czekam az ktos po drugiej stronie otworzy fifo do czytania\n");
        //continue;
        printf("error\n");
        exit(1);
      }

      printf("Blad pisania do pliku fifo!\n");
      exit(1);
    }

    if(n == 0)
    {
      printf("Nie udalo sie nic wpisac do fifo\n");
    }
  }

  printf("skonczylem produkowac %d",getpid());

  close(fifo);
  fclose(fip);
}

void sendData()
{
  /*
  FILE* fip;
  char c;

  fip = fopen(inputFilename,"r");
  if(!fip)
  {
    printf("Błąd otwarcia pliku z danymi!\n");
    exit(1);
  }

  while((c = getc(fip))!=EOF)
  {
    //printf("Wysylam znak do fifo\n");
    putIntoFifo(c);
  }

  printf("\nKoniec wysylania znakow do fifo\n");
  printf("Ilosc wyprodukowanych znakow: %d\n",numberOfChars);

  fclose(fip);
  */
  putIntoFifo();
}

int main()
{
  initialize();
  
  sendData();

  cleanup();

  return 0;
}