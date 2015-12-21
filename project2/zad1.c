/******
 *
 * nie zapomnij przy kompilacji dolinkowac biblioteki pthread
 * gcc zad1.c -o zad1.out -pthread
 *
 */

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define n 100 //ilosc elementow tablicy
#define threads 10 //ilosc watkow (ilosc wierszy tablicy)

int array[threads][n+1]; //na koncu wiersza bedzie suma policzona przez watek

void* sumRow(void* tid)
{
  int i;
  int sum = 0;
  int threadId = *((int*) tid);
  free(tid);

  for(i=0;i<n;i++)
  {
    array[threadId][n] += array[threadId][i];
  }

  //printf("Suma wiersza to: %d\n", array[threadId][n]);

  pthread_exit(0); 

  return NULL;
}

int main()
{

  int i,j;
  int sum = 0;
  int* id;
  int errP;
  int** statusT;
  int ids[threads];
  pthread_t threadIds[threads];

  srand(time(NULL));

  for(i=0;i<threads;i++)
  {
    for(j=0;j<n;j++)
    {
      array[i][j] = rand()%100;
    }
  }

  i=0;

  while(i < threads)
  {
    id = malloc(sizeof(id));
    *id = i;
    errP = pthread_create(&(threadIds[i]), NULL, &sumRow, id);
    if (errP != 0)
      printf("\nCan't create thread :[%s]", strerror(errP));
    else
      printf("\nThread created successfully\n");

    i++;
  }

  //lel chyba dziala dobrze
  for(i=0;i<threads;i++)
  {
    errP = pthread_join(threadIds[i], (void**) statusT);
    if (errP != 0)
    {
      printf("\nCan't join thread :[%s]", strerror(errP));
      printf("\nCan't join thread :[%d]", **statusT);
    }
    else
      printf("\nThread joined successfully\n");
  }

  printf("");

  for(i=0;i<threads;i++)
  {
    errP = pthread_detach(threadIds[i]);
    if (errP != 0)
    {
      printf("\nCan't detach thread :[%s]", strerror(errP));
    }
    else
      printf("\nThread detached successfully\n");
  }


  //jak dziala join to nie trzeba spac
  //sleep(5);

  for(i=0;i<threads;i++)
  {
    sum += array[i][n];
  }

  printf("Suma wszystkich wierszy to: %d\n",sum);



  return 0;


}
