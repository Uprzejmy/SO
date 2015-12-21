#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
  int i,id,pid;
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
      printf("PID %d\n",getpid());
      printf("PPID %d\n",getppid());
      printf("UID %d\n",getuid());
      printf("GID %d\n",getgid());
      printf("PGID %d\n\n",getpgid(getpid()));
      fork();   
    }
    fork();
  }

sprintf(cmd, "pstree -p %d",pid);

  //if(getpid() == pid)
  {
    
    system(cmd);
  }

  //sleep(1);

  exit(0);
}
