/* OBIETTIVO: generare un numero non noto di threads che eseguono il codice corrispondente al secondo esempio di uso dei semafori: LETTORI E SCRITTORI senza starvation. 
 * Il numero di lettori e il numero di scrittori, per semplicita', e' uguale: prima vengono creati un quarto di thread classificati come lettori, poi un quarto come scrittori e poi, di nuovo, un quarto di lettori, e quindi l'ultimo quarto con scrittori. 
 * L'utilizzo della risorsa in lettura o in scrittura e' stato simulato con una sleep. 
 * Ogni thread torna al main il proprio numero d'ordine. */
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef enum {false, true} Boolean;

/* variabili globali */
pthread_mutex_t MUTEX = PTHREAD_MUTEX_INITIALIZER; 	/* semaforo binario per la mutua esclusione nell'accesso alle variabili introdotte */
sem_t S_LETTORI; 					/* valore iniziale uguale a 0 ==> semaforo privato per i thread lettori */
sem_t S_SCRITTORI; 					/* valore iniziale uguale a 0 ==> semaforo privato per i thread scrittori */
int lettori_attivi = 0;					/* numero lettori attivi */
Boolean scrittori_attivi = false;			/* true se uno scrittore e' attivo */
int lettori_bloccati = 0;				/* numero di lettoti bloccati */
int scrittori_bloccati = 0;				/* numero di scrittori bloccati */

void Inizio_lettura()
{
   	pthread_mutex_lock(&MUTEX);
   	/* inizio sezione critica */
	if (!scrittori_attivi && scrittori_bloccati == 0)
	{
		sem_post(&S_LETTORI);
		lettori_attivi++;
	}
	else lettori_bloccati++;
   	/* fine sezione critica */
   	pthread_mutex_unlock(&MUTEX);
	sem_wait(&S_LETTORI);
}

void Fine_lettura()
{
        pthread_mutex_lock(&MUTEX);
   	/* inizio sezione critica */
        lettori_attivi--;
	if (lettori_attivi == 0 && scrittori_bloccati > 0)
   	{
                scrittori_attivi = true;
                scrittori_bloccati--;
		sem_post(&S_SCRITTORI);
        }
        /* fine sezione critica */
        pthread_mutex_unlock(&MUTEX);
}

void Inizio_scrittura()
{
   	pthread_mutex_lock(&MUTEX);
   	/* inizio sezione critica */
	if (lettori_attivi == 0 && !scrittori_attivi)
	{
		sem_post(&S_SCRITTORI);
		scrittori_attivi = true;
	}
	else scrittori_bloccati++;
   	/* fine sezione critica */
   	pthread_mutex_unlock(&MUTEX);
	sem_wait(&S_SCRITTORI);
}

void Fine_scrittura()
{
        pthread_mutex_lock(&MUTEX);
   	/* inizio sezione critica */
	scrittori_attivi = false;
	if (lettori_bloccati > 0)
	do
   	{
		lettori_attivi++;
		lettori_bloccati--;
		sem_post(&S_LETTORI);
        }
	while (lettori_bloccati != 0);
	else 
		if (scrittori_bloccati > 0)
 		{
                	scrittori_attivi = true;
			scrittori_bloccati--;
                	sem_post(&S_SCRITTORI);
        }
        /* fine sezione critica */
        pthread_mutex_unlock(&MUTEX);
}

void *eseguiLettura(void *id)
{
   int *pi = (int *)id;
   int *ptr;

   ptr = (int *) malloc( sizeof(int));
   if (ptr == NULL)
   {
        printf("Problemi con l'allocazione di ptr\n");
        exit(-1);
   }

   Inizio_lettura();
   printf("Thread%d e identificatore %lu ha ottenuto l'uso della risorsa in LETTURA\n", *pi, pthread_self());
   sleep(5); /* simuliamo l'uso della risorsa in lettura */
   Fine_lettura();

   /* pthread torna al padre il valore intero dell'indice */
   *ptr = *pi;
   pthread_exit((void *) ptr);
}

void *eseguiScrittura(void *id)
{
   int *pi = (int *)id;
   int *ptr;

   ptr = (int *) malloc( sizeof(int));
   if (ptr == NULL)
   {
        printf("Problemi con l'allocazione di ptr\n");
        exit(-1);
   }

   Inizio_scrittura();
   printf("Thread%d e identificatore %lu ha ottenuto l'uso della risorsa in SCRITTURA\n", *pi, pthread_self());
   sleep(5); /* simuliamo l'uso della risorsa in scrittura */
   Fine_scrittura();

   /* pthread torna al padre il valore intero dell'indice */
   *ptr = *pi;
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

   /* Calcoliamo il numero passato che sara' il numero di Pthread da creare che deve essere divisibile per 4! */
   NUM_THREADS = atoi(argv[1]);
   if (NUM_THREADS <= 0 || NUM_THREADS % 4 != 0) 
   {
   	printf("Errore: Il primo parametro non e' un numero strettamente maggiore di 0 e non e' divisibile per 4, infatti e' %d\n", NUM_THREADS);
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

   /* prima di creare i thread, andiamo ad inizializzare il semaforo S_LETTORI al valore 0 */
   if (sem_init(&S_LETTORI, 0, 0) != 0)
   {
        printf("Problemi con l'inizializzazione del semaforo S_LETTORI\n");
        exit(5);
   }

   /* prima di creare i thread, andiamo ad inizializzare il semaforo S_SCRITTORI al valore 0 */
   if (sem_init(&S_SCRITTORI, 0, 0) != 0)
   {
        printf("Problemi con l'inizializzazione del semaforo S_SCRITTORI\n");
        exit(6);
   }

   /* per semplicità decidiamo di creare un numero uguale di lettori e di scrittori: prima un quarto di lettori, poi un quarto di scrittori e poi di nuovo */
   for (i=0; i < NUM_THREADS/4; i++)
   {
        taskids[i] = i;
   	printf("Sto per creare il thread LETTORE %d-esimo\n", taskids[i]);
        if (pthread_create(&thread[i], NULL, eseguiLettura, (void *) (&taskids[i])) != 0)
                printf("SONO IL MAIN E CI SONO STATI PROBLEMI DELLA CREAZIONE DEL thread %d-esimo\n", taskids[i]);
	printf("SONO IL MAIN e ho creato il Pthread %i-esimo con id=%lu\n", i, thread[i]);
   }

   for (; i < NUM_THREADS/2; i++)
   {
        taskids[i] = i;
   	printf("Sto per creare il thread SCRITTORE %d-esimo\n", taskids[i]);
        if (pthread_create(&thread[i], NULL, eseguiScrittura, (void *) (&taskids[i])) != 0)
                printf("SONO IL MAIN E CI SONO STATI PROBLEMI DELLA CREAZIONE DEL thread %d-esimo\n", taskids[i]);
	printf("SONO IL MAIN e ho creato il Pthread %i-esimo con id=%lu\n", i, thread[i]);
   }

   for (; i < NUM_THREADS*3/4; i++)
   {
        taskids[i] = i;
   	printf("Sto per creare il thread LETTORE %d-esimo (seconda passata)\n", taskids[i]);
        if (pthread_create(&thread[i], NULL, eseguiLettura, (void *) (&taskids[i])) != 0)
                printf("SONO IL MAIN E CI SONO STATI PROBLEMI DELLA CREAZIONE DEL thread %d-esimo\n", taskids[i]);
	printf("SONO IL MAIN e ho creato il Pthread %i-esimo con id=%lu\n", i, thread[i]);
   }

   for (; i < NUM_THREADS; i++)
   {
        taskids[i] = i;
   	printf("Sto per creare il thread SCRITTORE %d-esimo (seconda passata)\n", taskids[i]);
        if (pthread_create(&thread[i], NULL, eseguiScrittura, (void *) (&taskids[i])) != 0)
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

