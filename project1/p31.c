#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
  int i,id,cpid,execEr,status,pid;
  char cmd[120];

  pid=getpid();

  for(i=0; i<3; i++)
  {
    id = fork();
    if(id == -1)
    {
      printf("Nieprawidlowe wywolanie fork()\n");
      exit(1);
    }
    if(id == 0)
    {
      execEr = execl("/usr/bin/cal","cal","07", "1410",NULL);
      if(execEr == -1)
      {
        printf("Blad wywolania execl\n");
        exit(2);
      }
    }
  }

  sprintf(cmd, "pstree -p %d",pid);
  system(cmd);

  for(i=0; i<3; i++)
  {
    cpid = wait(&status);
    if(cpid == -1)
    {
      printf("Blad potomka kod powrotu potomka to: %d\n", status/256);
    }
    else
    {
      printf("Potomek zakonczyl sie poprawnie, id: %d\n", cpid);
      printf("Blad potomka kod powrotu potomka to: %d\n", status/256);
    }
    
  } 
  

  exit(0);
}
