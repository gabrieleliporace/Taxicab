#define _GNU_SOURCE
#include "taxicab.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <signal.h>

int main()
{
	cell* shd;
	int sources;
	int status;
	int i=0,so_sources=3,so_taxi=0;
	pid_t *all_origin;
	pid_t *all_taxi;

    printf("\nPID_MASTER:%d\n\n",getpid());
	sources=inizializzazione_valori();
	so_taxi=3;
	so_sources=SO_SOURCES;
	shd=mappa();
	if(!check_holes(shd)){
		printf("\nNon sono riuscito a creare una citta' accettabile, riprovare.\n");
		exit(0);
	}
	stampa_matrice(shd);
	all_origin=malloc(so_sources*sizeof(*all_origin));
	all_taxi=malloc(so_taxi*sizeof(*all_taxi));
	simulation(shd,all_origin,all_taxi,so_sources,so_taxi);

	while(wait(&status)!=-1){
	}
	free(all_origin);
	free(all_taxi);
	return 0;
}

