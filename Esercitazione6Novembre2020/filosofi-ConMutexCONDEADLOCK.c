/* OBIETTIVO: generare i 5 thread che rappresentano i filosofi: PROBLEMA DEI FILOSOFI RISOLTO CON UNA SOLUZIONE CHE USA UN ARRAY DI MUTEX E UNA SOLUZIONE SIMMETRICA che chiaramente può portare al DEADLOCK! 
 * La fase in cui il filosofo mangia e' stata simulata con una sleep mentre quella in cui pensa e' stata simulata con un descheduling. 
 * Ogni thread torna al main il proprio numero d'ordine. */
#define _GNU_SOURCE             /* Per non avere warning utilizzando la pthread_yield - See feature_test_macros(7) */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define NTIMES 10

typedef enum {false, true} Boolean;

/* variabili globali */
pthread_mutex_t S_BASTONCINO[5]={ PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER }; /* array di mutex inizializzato staticamente ognuno per definizione con valore iniziale uguale a 1 */

void *eseguiFilosofo(void *id)
{
   int *pi = (int *)id;
   int *ptr;
   int i;

   ptr = (int *) malloc( sizeof(int));
   if (ptr == NULL)
   {
        printf("Problemi con l'allocazione dell'array ptr\n");
        exit(-1);
   }
   
   /* codice filosofo */
   printf("FILOSOFO con indice %d\n", *pi);

   for (i = 0; i < NTIMES; i++) /* while (true)  i filosofi dovrebbero essere cicli senza fine */
   {
	pthread_mutex_lock(&(S_BASTONCINO[*pi]));
	pthread_mutex_lock(&(S_BASTONCINO[(*pi+1)%5]));
   	printf("FILOSOFO con indice %d e identificatore %lu ha ottenuto di poter mangiare (i=%d)\n", *pi, pthread_self(),i); 
   	sleep(5); /* simuliamo l'azione di mangiare */
	pthread_mutex_unlock(&(S_BASTONCINO[*pi]));
	pthread_mutex_unlock(&(S_BASTONCINO[(*pi+1)%5]));
   	printf("FILOSOFO con indice %d e identificatore %lu ora pensa (i=%d)\n", *pi, pthread_self(),i);
    	pthread_yield(); /* sleep(5); simuliamo l'azione di pensare */
   }

   /* pthread torna al padre il valore intero dell'indice */
   *ptr = *pi;
   pthread_exit((void *) ptr);
}

int main ()
{
   pthread_t *thread;
   int *taskids;
   int i;
   int *p;
   int NUM_THREADS = 5;

   thread=(pthread_t *) malloc(NUM_THREADS * sizeof(pthread_t));
   if (thread == NULL)
   {
        printf("Problemi con l'allocazione dell'array thread\n");
        exit(3);
   }
   taskids = (int *) malloc(NUM_THREADS * sizeof(int));
   if (taskids == NULL)
   {
        printf("Problemi con l'allocazione dell'array taskids\n");
        exit(4);
   }

   /* creazione dei thread filosofi */
   for (i=0; i < NUM_THREADS; i++)
   {
        taskids[i] = i;
   	printf("Sto per creare il thread %d-esimo\n", taskids[i]);
	if (pthread_create(&thread[i], NULL, eseguiFilosofo, (void *) (&taskids[i])) != 0)
                printf("SONO IL MAIN E CI SONO STATI PROBLEMI DELLA CREAZIONE DEL thread %d-esimo\n", taskids[i]);
	printf("SONO IL MAIN e ho creato il Pthread %i-esimo con id=%lu\n", i, thread[i]);
   }

   for (i=0; i < NUM_THREADS; i++)
   {
        int ris;
        pthread_join(thread[i], (void**) & p);
        ris= *p;
        printf("Pthread %d-esimo restituisce %d\n", i, ris);
   }

   exit(0);
}

