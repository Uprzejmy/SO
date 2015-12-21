#include <stdlib.h>
#include <stdio.h>
int main() 
{
  printf("PID %d\n", getpid());
  printf("UID %d\n",getuid());
  printf("GID %d\n",getgid());
  printf("PPID %d\n\n", getppid());
}
