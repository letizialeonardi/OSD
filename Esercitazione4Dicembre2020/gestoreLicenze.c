/* OBIETTIVO: generare un numero non noto di threads che eseguono il codice corrispondente al problema della gestione delle licenze di un certo software. 
 * Il numero di licenze (per semplicita') e' definito dalla costante MAX_LICENZE: chiaramente, e' possibile variare tale numero a piacimento; 
 * si segnala che il caso piu' interessante e' comunque quello in cui il numero di thread sia significativamente maggiore del numero di licenze: ad esempio, 10 thread per 5 risorse. 
 * L'utilizzo delle licenze da parte di un thread (che rappresenta un singolo utente) e' stato simulato con una sleep. 
 * Ogni thread utente torna al main il numero random generato */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define  MAX_LICENZE 5

/* variabili globali */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;      /* semaforo di mutua esclusione per l'accesso a tutte le variabili condivise (simula il semaforo di mutua esclus
ione associato ad una istanza di tipo monitor) */
pthread_cond_t *disponibili;

int licenze_correnti = MAX_LICENZE; 			/* variabile che tiene traccia di quante licenze sono disponibili */
int NUM_THREADS;					/* deve diventare una variabile globale dato che serve nella fase di rilascio */

void RICHIESTA_LICENZE(int i, int count)
{
	/* N.B. L'indice del thread utente serve per sapere su quale variabile condizione ci si deve sospendere */
	pthread_mutex_lock(&mutex);     /* simulazione di inizio procedura entry del monitor */
        while (licenze_correnti < count) /* si deve usare un while perche' quando si rilasciano le licenze non si puo' sapere le singole richieste fatte da altri utenti */
                pthread_cond_wait(&disponibili[i], &mutex);
        licenze_correnti -= count;	/* se abbiamo avuto successo nel trovare il numero di licenze_correnti sufficienti, allora decrementiamo il numero */
        pthread_mutex_unlock(&mutex);   /* simulazione di termine procedura entry del monitor */
}

void RILASCIA_LICENZE(int i, int count)
{
	/* N.B. L'indice del thread utente serve per sapere su quale variabile condizione non si deve fare la signal (in realta' non servirebbe, ma risulta piu' chiaro il codice */
	int j;
        pthread_mutex_lock(&mutex);     /* simulazione di inizio procedura entry del monitor */
        licenze_correnti += count;	/* dobbiamo incrementare il numero di licenze disponibili */
   	printf("Thread%d e identificatore %lu STA RILASCIANDO %d licenze\n", i, pthread_self(), count);
        for (j=0; j < NUM_THREADS; j++)
		if (j != i)
                	pthread_cond_signal(&disponibili[j]);
        pthread_mutex_unlock(&mutex);   /* simulazione di termine procedura entry del monitor */
}

int mia_random(int n)
{
int casuale;
casuale = rand() % n;
casuale++;              /* si incrementa dato che la rand produce un numero random fra 0 e n-1, mentre a noi serve un numero fra 1 e n */
return casuale;
}

void *eseguiUtente(void *id)
{
   int *pi = (int *)id;
   int *ptr;
   int r;	/* per generare un numero random di licenze da richiedere (compreso fra 1 e MAX_LICENZE */

   ptr = (int *) malloc( sizeof(int));
   if (ptr == NULL)
   {
        printf("Problemi con l'allocazione di ptr\n");
        exit(-1);
   }

   printf("UTENTE-[Thread%d e identificatore %lu] STO ARRIVANDO\n", *pi, pthread_self());

   r=mia_random(MAX_LICENZE);
   RICHIESTA_LICENZE(*pi, r);
   printf("Thread%d e identificatore %lu ha ottenuto l'uso di %d licenze\n", *pi, pthread_self(), r);
   sleep(5); /* simuliamo l'uso delle licenze */
   RILASCIA_LICENZE(*pi, r);

   /* pthread torna al padre il numero random calcolato */
   *ptr = r;
   pthread_exit((void *) ptr);
}

int main (int argc, char **argv)
{
   pthread_t *thread;
   int *taskids;
   int i;
   int *p;

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

   disponibili=(pthread_cond_t *) malloc(NUM_THREADS * sizeof(pthread_cond_t));
   if (disponibili == NULL)
   {
        printf("Problemi con l'allocazione dell'array disponibili\n");
        exit(5);
   }

   /* prima di creare i thread, andiamo ad inizializzare l'array di variabili condition al valore di default */
   for (i=0; i < NUM_THREADS; i++)
  	pthread_cond_init(&disponibili[i], NULL);	/* attenzione che la macro PTHREAD_COND_INITIALIZER puo' essere usata solo in fase di inizializzazione di una variabile condition! */

   srand(time(NULL)); /* inizializziamo il seme per la generazione random di numeri  */

   for (i=0; i < NUM_THREADS; i++)
   {
        taskids[i] = i;
   	printf("Sto per creare il thread %d-esimo\n", taskids[i]);
        if (pthread_create(&thread[i], NULL, eseguiUtente, (void *) (&taskids[i])) != 0)
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

