/* OBIETTIVO: generare un numero non noto di threads che eseguono il codice corrispondente al primo esempio di uso dei semafori: GESTIONE DI UN INSIEME DI RISORSE EQUIVALENTI. 
 * Il numero di risorse equivalenti (per semplicita') e' definito dalla costante m: chiaramente, e' possibile variare tale numero a piacimento; si segnala che il caso piu' interessante e' comunque quello in cui il numero di thread sia significativamente maggiore del numero di risorse: ad esempio, 10 thread per 5 risorse. 
 * L'utilizzo della risorsa e' stato simulato con una sleep. 
 * Ogni thread torna al main il numero d'ordine della risorsa che e' stata assegnata e quindi utilizzata. */
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define m 5

typedef enum {false, true} Boolean;

/* variabili globali */
pthread_mutex_t MUTEX = PTHREAD_MUTEX_INITIALIZER;	/* semaforo binario per la mutua esclusione nell'accesso all'array LIBERO */
sem_t RISORSE; 						/* semaforo contatore il cui valore iniziale dovra' essere uguale a m (cioe' quante sono le risorse equivalenti) */
Boolean LIBERO[m];

int RICHIESTA()
{
	int i;
	sem_wait(&RISORSE);
   	pthread_mutex_lock(&MUTEX);
   	/* inizio sezione critica */
	for (i=0; !LIBERO[i]; i++);
	/* trovato un indice i per cui LIBERO[i] e' true */
	LIBERO[i] = false;
   	/* fine sezione critica */
   	pthread_mutex_unlock(&MUTEX);
	return i;
}

void RILASCIO(int x)
{
        pthread_mutex_lock(&MUTEX);
   	/* inizio sezione critica */
	LIBERO[x] = true;
   	/* fine sezione critica */
   	pthread_mutex_unlock(&MUTEX);
	sem_post(&RISORSE);
}

void *esegui(void *id)
{
   int *pi = (int *)id;
   int *ptr;
   int x;

   ptr = (int *) malloc( sizeof(int));
   if (ptr == NULL)
   {
        printf("Problemi con l'allocazione di ptr\n");
        exit(-1);
   }

   x = RICHIESTA();
   printf("Thread%d e identificatore %lu ha ottenuto l'uso della risorsa di indice %d\n", *pi, pthread_self(), x);
   sleep(5); /* simuliamo l'uso della risorsa */
   RILASCIO(x);

   /* pthread torna al padre il valore intero di x */
   *ptr = x;
   pthread_exit((void *) ptr);
}

int main (int argc, char **argv)
{
   pthread_t *thread;
   int *taskids;
   int i;
   int *p;
   int NUM_THREADS;

   /* Controllo sul numero di parametri */
   if (argc != 2 ) /* Deve essere passato esattamente un parametro */
   {
   	printf("Errore nel numero dei parametri %d\n", argc-1);
        exit(1);
   }

   /* Calcoliamo il numero passato che sara' il numero di Pthread da creare */
   NUM_THREADS = atoi(argv[1]);
   if (NUM_THREADS <= 0) 
   {
   	printf("Errore: Il primo parametro non e' un numero strettamente maggiore di 0 ma e' %d\n", NUM_THREADS);
        exit(2);
   }

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

   /* prima di creare i thread, andiamo ad inizializzare il semaforo RISORSE al valore m */
   if (sem_init(&RISORSE, 0, m) != 0)
   {
        printf("Problemi con l'inizializzazione del semaforo RISORSE\n");
        exit(5);
   }

   /* prima di creare i thread, andiamo ad inizializzare gli elementi dell'array LIBERO */
   for (i=0; i < m; i++)
  	LIBERO[i] = true;

   for (i=0; i < NUM_THREADS; i++)
   {
        taskids[i] = i;
   	printf("Sto per creare il thread %d-esimo\n", taskids[i]);
        if (pthread_create(&thread[i], NULL, esegui, (void *) (&taskids[i])) != 0)
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

