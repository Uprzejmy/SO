#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

char fifoFilename[] = "transfer";
char inputFilename[128];
int numberOfChars = 0;

void sigintHandler(int signal);

void initialize()
{
  signal(SIGINT, sigintHandler);
  sprintf(inputFilename,"output/%ld.output",(long int)getpid());
}

void cleanup()
{

}

void sigintHandler(int signal)
{
  printf("\nKonsument przerwal dzialanie przez Ctrl+C \n");
  printf("Ilosc skonsumowanych znakow: %d\n",numberOfChars);
  exit(0);
}

int getFromFifo(char* c)
{
  FILE* fip;

  fip = fopen(fifoFilename,"r");
  if(!fip)
  {
    printf("Błąd otwarcia pliku fifo!\n");
    exit(1);
  }

  *c = getc(fip);

  fclose(fip);

  if(*c == EOF)
  {
    sleep(1);//wait for input
    return getFromFifo(c);
  }

  numberOfChars++;
  return 0;
}

void getData()
{
  FILE* fop;
  char c;

  while(!getFromFifo(&c))
  {
    fop = fopen(inputFilename,"a");
    if(!fop)
    {
      printf("Błąd otwarcia pliku wynikowego!\n");
      exit(1);
    }

    printf("%c\n",c);
    putc(c,fop);

    fclose(fop);
  }

}

int main()
{
  initialize();
  
  getData();

  cleanup();

  return 0;
}