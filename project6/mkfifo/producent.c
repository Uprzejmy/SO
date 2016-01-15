#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

void generateFile()
{
  FILE* fop;
  char filename[128];
  int i,numberOfChars;
  char c;

  numberOfChars = rand() % 50 + 1;//bede generowal pliki od 1 do 50 znakow
  sprintf(filename,"input/%ld",(long int)getpid());

  fop = fopen(filename,"w");
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
}

initialize()
{
  srand(time(NULL));

  generateFile();
}

int main()
{
  initialize();
  
  

  cleanup();

  return 0;
}