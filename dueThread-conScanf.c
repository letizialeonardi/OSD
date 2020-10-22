#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS 2

void *PrintHello(void *id)
{
   int *pi = (void *)id;
   printf("Thread%d partito: Hello World!\n", *pi);
   return NULL;
}

int main ()
{
   pthread_t *thread;
   int *taskids[NUM_THREADS];
   int i;
   
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
   /* inseriamo una scanf in modo che il processo creatore resti in attesa */
   scanf("%d", &i);
   printf("Processo creatore ha letto %d\n", i);
   exit(0);
}

