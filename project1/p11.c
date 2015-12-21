#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
  printf("PID %d\n",getpid());
  printf("PPID %d\n",getppid());
  printf("UID %d\n",getuid());
  printf("GID %d\n\n",getgid());
  exit(0);
}
