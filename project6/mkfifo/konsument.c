#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

char fifoFilename[] = "transfer";
char inputFilename[128];

void initialize()
{
  sprintf(inputFilename,"output/%ld.output",(long int)getpid());
}

void cleanup()
{

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

  return 0;
}

void getData()
{
  FILE* fop;
  char c;

  fop = fopen(inputFilename,"w");
  if(!fop)
  {
    printf("Błąd otwarcia pliku wynikowego!\n");
    exit(1);
  }

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