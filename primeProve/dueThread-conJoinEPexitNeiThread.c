#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS 2

void *PrintHello(void *id)
{
   int *pi = (void *)id;
   int *ptr;

   ptr = (int *) malloc( sizeof(int));
   if (ptr == NULL)
   {
        printf("Problemi con l'allocazione di ptr\n");
        exit(3);
   }

   printf("Thread%d partito: Hello World!\n", *pi);
   /* pthread vuole tornare al padre un valore intero, ad es 1000+id */
   *ptr = 1000+*pi;
   pthread_exit((void *) ptr);
}

int main ()
{
   pthread_t *thread;
   int *taskids[NUM_THREADS];
   int i;
   int *p;

   thread=(pthread_t *) malloc(NUM_THREADS * sizeof(pthread_t));
   if (thread == NULL)
   {
        printf("Problemi con l'allocazione dell'array thread\n");
        exit(1);
   }

   for (i=0; i < NUM_THREADS; i++)
   {
        //printf("Sto per creare il thread %d-esimo\n", i);
        taskids[i] = (int *) malloc(sizeof(int));
        if (taskids[i] == NULL)
        {
                printf("Problemi con l'elemento %i-esimo di taskids\n", i);
                exit(2);
        }

        *taskids[i] = i;
   	printf("Sto per creare il thread %d-esimo\n", *taskids[i]);
        if (pthread_create(&thread[i], NULL, PrintHello, (void *) taskids[i]) !=
 0)
                printf("SONO IL MAIN E CI SONO STATI PROBLEMI DELLA CREAZIONE DEL thread %d-esimo\n", *taskids[i]);
    }

   for (i=0; i < NUM_THREADS; i++)
   {
	int ris;
   	pthread_join(thread[i], (void**) & p);
	ris= *p;
	printf("Pthread %d-esimo restituisce %d\n", i, ris);
	free(p);
   }
 
   exit(0);
}

