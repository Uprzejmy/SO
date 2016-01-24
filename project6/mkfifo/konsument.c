#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>

char fifoFilename[] = "transfer";
char inputFilename[128];
char* input;
int numberOfChars = 0;
int maxChars = 10;
int maxOutputSize;
FILE* fop;

void sigintHandler(int signal);

void initialize()
{
  signal(SIGINT, sigintHandler);
  sprintf(inputFilename,"output/%ld.output",(long int)getpid());
  maxOutputSize = maxChars*sizeof(char);
  input = malloc(maxOutputSize);
}

void cleanup()
{
  if(fop!=NULL)
  {
    fclose(fop);
  }
}

void sigintHandler(int signal)
{
  printf("\nKonsument przerwal dzialanie przez Ctrl+C \n");
  printf("Ilosc skonsumowanych znakow: %d\n",numberOfChars);
  cleanup();
  exit(0);
}

void getData()
{
  FILE* fop;
  char c;
  int charsRead;
  int i,fifo;

  printf("Czekam az ktos po drugiej stronie otworzy fifo do pisania\n");
  fifo = open(fifoFilename,O_RDONLY);
  if(fifo == -1)
  {

    printf("Błąd otwarcia pliku fifo!\n");
    exit(1);
  }

  while(1)
  {
    if ((charsRead = read(fifo,&c,sizeof(char))) == -1)
    {
      printf("error reading fifo\n");
      exit(1);
    }

    if(charsRead == 0)
    {
      close(fifo);
      printf("koniec fifo");
      exit(0);
    }

    fop = fopen(inputFilename,"a");
    if(!fop)
    {
      printf("Błąd otwarcia pliku wynikowego!\n");
      exit(1);
    }

    numberOfChars += charsRead;

    for(i=0;i<charsRead;i++)
    {
      putc(c,fop);
    }

    fclose(fop);
  }

  printf("Przeczytano %d znakow (koniec)\n",charsRead);

}

int main()
{
  initialize();
  
  getData();

  cleanup();

  return 0;
}
