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

   printf("Thread%d partito: Hello World! Ho come identificatore %lu\n", *pi, pthread_self());
   /* pthread vuole tornare al padre un valore intero, ad es 1000+id */
   *ptr = 1000+*pi;
   pthread_exit((void *) ptr);
}

int main ()
{
   pthread_t *thread;
   int *taskids;
   int i;
   int *p;

   thread=(pthread_t *) malloc(NUM_THREADS * sizeof(pthread_t));
   if (thread == NULL)
   {
        printf("Problemi con l'allocazione dell'array thread\n");
        exit(1);
   }
   taskids = (int *) malloc(NUM_THREADS * sizeof(int));
   if (taskids == NULL)
   {
        printf("Problemi con l'allocazione dell'array taskids\n");
        exit(2);
    }

   for (i=0; i < NUM_THREADS; i++)
   {
        taskids[i] = i;
   	printf("Sto per creare il thread %d-esimo\n", taskids[i]);
        if (pthread_create(&thread[i], NULL, PrintHello, (void *) (&taskids[i])) !=
 0)
                printf("SONO IL MAIN E CI SONO STATI PROBLEMI DELLA CREAZIONE DEL thread %d-esimo\n", taskids[i]);
	printf("SONO IL MAIN e ho creato il Pthread %i-esimo con id=%lu\n", i, thread[i]);
    }

   for (i=0; i < NUM_THREADS; i++)
   {
	int ris;
   	pthread_join(thread[i], (void**) & p);
	ris= *p;
	printf("Pthread %d-esimo restituisce %d\n", i, ris);
	//free(p);
   }
 
   exit(0);
}

