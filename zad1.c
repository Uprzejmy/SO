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

#define n 10 //ilosc elementow tablicy
#define threads 3 //ilosc watkow (ilosc wierszy tablicy)

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

  printf("Suma wiersza to: %d\n", array[threadId][n]);

  return NULL;
}

int main()
{

  int i,j;
  int sum = 0;
  int* id;
  int err;
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
    err = pthread_create(&(threadIds[i]), NULL, &sumRow, id);
    if (err != 0)
      printf("\ncan't create thread :[%s]", strerror(err));
    else
      printf("\n Thread created successfully\n");

    i++;
  }

  //lel chyba dziala dobrze
  for(i=0;i<threads;i++)
  {
    err = pthread_join(threadIds[i], NULL);
    if (err != 0)
      printf("\ncan't join thread :[%s]", strerror(err));
    else
      printf("\n Thread joined successfully\n");
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
