#ifndef _TAXICAB_H
#define _TAXICAB_H

#include <stdio.h>
#include <stdlib.h>

#define SO_HEIGHT 10
#define SO_WIDTH 20
#define INDEX(x,y) (y*SO_WIDTH)+x
#define MAP (SO_WIDTH*SO_HEIGHT)
#define H_MAP MAP/9
#define SHM_KEY 0x210520
#define SEM_KEY 0x113522
#define SEM_KEY2 0x102725
#define SEM_KEY3 0x102326
#define MSGQ_KEY 0x170125

/*Valori richiesi definiti in questo file per permettere l'utilizzo
 sia nel file taxicab-module.c che in test-taxicab-module.c*/

int SO_SOURCES, SO_HOLES, SO_TAXI, SO_TOP_CELLS, SO_CAP_MIN, SO_CAP_MAX;
int SO_TIMENSEC_MIN, SO_TIMENSEC_MAX, SO_TIMEOUT, SO_DURATION, SO_NORM;

int ender;

typedef struct cell{
	int cap;
	int type;
	int timensec;
	int taxi_in;
	int on;
}cell;

typedef struct quest{
	int origin;			/*calcolate con la INDEX(j,i)*/
	int dest;
}quest;

typedef struct taxi{
	int busy;
	int origin;
	int dest;
	int now;	/*posizione corrente*/
}taxi;

typedef struct stat{
	int m_pid;
	int	score;	/*Richieste prese in carico totali*/
	int cells;	/*Celle attraversate*/
	int tempo;	/*Tempo della corsa presa in carico*/
}stat;

struct msgbuf {
	long mtype;
	quest req;
};

int inizializzazione_valori();	/*Usato per inizializzare i valori delle variabili globali del master*/

cell* mappa();	/*Il master da il via alla creazione della mappa*/

cell* make_city(cell* my_matrix,int source);	/*Esegue le funzioni necessarie alla creazione della mappa*/

void valori(int source);	/*Inizializza solo i valori necessari alla creazione della mappa*/

int controllo_valori();	/*Controlla che con i valori si possa creare la mappa*/

cell* shm_matrix();	/*Mediante execve genera la mappa in memoria condivisa*/

int random_type();	/*Inizializza il tipo della cella*/

int random_cap();	/*Inizializza la capacita' della cella*/

int random_timensec();	/*Inizializza il tempo di attraversamento della cella*/

void scambia(cell* my_matrix);	/*Modifica mappa per permettere di generare una mappa accettabile piu' frequentemente*/

int check_holes(cell* my_matrix);	/*Controlla che gli holes non creino barriere*/

void stampa_matrice(cell* my_arr);	/*Stampa la matrice, template da stampare ogni secondo*/

void send_quest(cell* shd,int time,int msg_id,int my_mtype);	/*Genera richieste ogni 2 secondi, con SIGINT ne genera una a comando*/

int take_source(int sem_source,int x,int y,cell* shd);

void set_sem(int sem2id,cell* shd);

void set_sem_source(int sem_source,cell* shd);

int posizionamento(int sem2id,int x, int y,cell* shd);

void goto_source(cell*shd,int now);

/*Avvia la generazione di processi sources, taxi e del countdouwn della simulazione*/
void simulation(cell* shd,pid_t *all_origin,pid_t *all_taxi,int sources,int taxi);	

void my_handler(int signum);	/*Handler delle sources, con SIGINT generano una richiesta*/

void p_handler(int signum);	/*Handler del processo master, se riceve SIGALRM invia SIGTERM a figli sources e taxi*/

void stampa_finale();	/*Stampa alla fine della simulazione*/

/*
  - viaggi(successo, inevasi e abortiti),
  - sorgenti,
  - top_cells,
  - taxi con piu' celle attraversate,
  - taxi con tempo viaggio piu' lungo,
  - taxi con piu' richieste raccolte
*/

/*void make_source(cell* shd,pid_t *all_origin,int sources);*/

#endif
