#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

char fifoFilename[] = "transfer";
char inputFilename[128];

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

void makeFifo()
{
  if(mkfifo(fifoFilename, 0600)==-1)
  {
    if(errno == EEXIST)
    {
      printf("Fifo juz istnieje, nie musze tworzyc nowego\n");
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

void initialize()
{
  srand(time(NULL));

  generateFile();

  makeFifo();
}

void cleanup()
{

}

void putIntoFifo(char c)
{
  FILE* fop;

  fop = fopen(fifoFilename,"w");
  if(!fop)
  {
    printf("Błąd otwarcia pliku fifo!\n");
    exit(1);
  }

  putc(c,fop);

  fclose(fop);
}

void sendData()
{
  FILE* fip;
  char c;

  fip = fopen(inputFilename,"r");
  if(!fip)
  {
    printf("Błąd otwarcia pliku fifo!\n");
    exit(1);
  }

  while((c = getc(fip))!=EOF)
  {
    putIntoFifo(c);
  }

  fclose(fip);
}

int main()
{
  initialize();
  
  sendData();

  cleanup();

  return 0;
}