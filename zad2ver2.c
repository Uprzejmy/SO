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
#define threads 2 //ilosc watkow (ilosc wierszy tablicy)

int array[threads][n+1]; //na koncu wiersza bedzie suma policzona przez watek

void* sumRow(void* tid)
{
  int i;
  int sum = 0;
  int threadId = *((int*) tid);
  int* result;

  free(tid);

  for(i=0;i<n;i++)
  {
    array[threadId][n] += array[threadId][i];
  }

  printf("suma wiersza %d: %d\n",threadId,array[threadId][n]);

  result = malloc(sizeof(int));
  *result = 15;

  pthread_exit(result); 

  // dla pewnosci
  return NULL;
}

int main()
{

  int i,j;
  int sum = 0;
  int* id;
  int errP;
  int* statusT;
  int ids[threads];
  pthread_t threadIds[threads];

  srand(time(NULL));

  for(i=0;i<threads;i++)
  {
    for(j=0;j<n;j++)
    {
      array[i][j] = rand()%10;
    }
  }

  for(i=0;i<threads;i++)
  {
    id = malloc(sizeof(id));
    *id = i;
    errP = pthread_create(&(threadIds[i]), NULL, sumRow, id);
    if (errP != 0)
      printf("\nCan't create thread :[%s]", strerror(errP));
    else
      printf("\nThread created successfully\n");
  }

  for(i=0;i<threads;i++)
  {
    errP = pthread_join(threadIds[i], (void**) &statusT);
    if (errP != 0)
    {
      printf("\nCan't join thread :[%s]", strerror(errP));
      printf("\nCan't join thread :[%d]", *statusT);
    }
    else
      printf("\nThread joined successfully\n");
    
    printf("status :[%d]", *statusT);
    free(statusT);
  }



  printf("\n");


  
  for(i=0;i<threads;i++)
  {
    errP = pthread_detach(threadIds[i]);
    if (errP != 0)
    {
      printf("\nCan't detach thread :[%s]\n", strerror(errP));
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
