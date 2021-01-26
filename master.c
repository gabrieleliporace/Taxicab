#define _GNU_SOURCE
#include "taxicab.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>	
#include <time.h>	

#define CHILD_NAME "./mappa"
int expired=0,s=0;
int successi=0,inevasi=0,abortiti=0;	/*Tipologie di viaggi per la stampa finale*/

int  inizializzazione_valori()
{
	/*Creazione degli stream*/
	FILE* taxicab_init;

	/*Creazione array per inserimento valori da stdin*/
	char choose[4];

	taxicab_init=fopen("./init.txt","r");

	/*Lettura da file ed inizializzazione valori della citta'*/

	fscanf(taxicab_init,"%d",&SO_HOLES);
	fscanf(taxicab_init,"%d",&SO_CAP_MIN);
	fscanf(taxicab_init,"%d",&SO_CAP_MAX);
	fscanf(taxicab_init,"%d",&SO_TIMENSEC_MIN);
	fscanf(taxicab_init,"%d",&SO_TIMENSEC_MAX);
	fscanf(taxicab_init,"%d",&SO_TOP_CELLS);
	fscanf(taxicab_init,"%d",&SO_DURATION);
	fscanf(taxicab_init,"%d",&SO_TIMEOUT);
	
	/*Utilizzo di switch per impostare il numero di sorgenti in modo 
	  predefinito o usando un valore scritto su file di input*/

	printf("Use default value<1> or a custom one<2> for sources number?\n");
	fgets(choose, sizeof(choose),stdin);
	switch(atoi(choose)){
		case 1:
			SO_SOURCES=MAP-SO_HOLES;
			break;
		case 2:
			fscanf(taxicab_init,"%d",&SO_SOURCES);
			break;
		default:
			printf("Option non available\n");
			exit(-1);
	}

	/*Utilizzo di switch per impostare il numero di taxi in modo predefinito o 
	  usando un valore scritto su file di input*/

	printf("Use default value<1> or a custom one<2> for taxi number?\n");
	fgets(choose, sizeof(choose), stdin);
	switch(atoi(choose)){
		case 1:
			SO_TAXI=SO_SOURCES/2;
			break;
		case 2:
			fscanf(taxicab_init,"%d",&SO_TAXI);
			break;
		default:
			printf("Option non available\n");
			exit(-1);
	}
	
	fclose(taxicab_init);
	return SO_SOURCES;
}

cell* mappa()	/*Mediante execve genera la mappa della citta'*/
{
	cell* shd;
	int status;
	int m_id;
	char source_str[3*sizeof(SO_SOURCES)+1];
	char * args[2]={CHILD_NAME};
	sprintf(source_str,"%d",SO_SOURCES);
	args[0]=source_str;
	args[1]=NULL;

	if(!fork()){
		execve(CHILD_NAME,args,NULL);
	}
	m_id=shmget(SHM_KEY,sizeof(*shd),IPC_CREAT|0600);
	shd=shmat(m_id,NULL,0);
	while(wait(&status)!=-1){
	}
	return shd;
}

int check_holes(cell * shd)
{
	int i=1,j=1;
	int result=1;

	for(i=1;i<((SO_HEIGHT)-1);i++){
		for(j=1;j<((SO_WIDTH)-1);j++){
			if(result!=0){
				switch(shd[INDEX(j, i)].type){	/*Controllo degli holes in base al tipo della mia cella*/
					case 0:
						if(shd[INDEX((j-1),(i-1))].type==0){
							result=0;
						}else if(shd[INDEX(j-1,i)].type==0){
							result=0;
						}else if(shd[INDEX((j-1),(i+1))].type==0){
							result=0;
						}else if(shd[INDEX(j,(i-1))].type==0){
							result=0;
						}else if(shd[INDEX(j,(i+1))].type==0){
							result=0;
						}else if(shd[INDEX((j+1),(i-1))].type==0){
							result=0;
						}else if(shd[INDEX((j+1),i)].type==0){
							result=0;
						}else if(shd[INDEX((j+1),(i+1))].type==0){
							result=0;
						}
						break;
					default:
						if((shd[INDEX((j-1),(i-1))].type==shd[INDEX((j-1),(i))].type)&&(shd[INDEX((j-1),(i-1))].type==0)){
							result=0;
						}else if((shd[INDEX((j-1),(i-1))].type==shd[INDEX((j),(i-1))].type)&&(shd[INDEX((j-1),(i-1))].type==0)){
							result=0;
						}else if((shd[INDEX((j),(i-1))].type==shd[INDEX((j+1),(i-1))].type)&&(shd[INDEX((j),(i-1))].type==0)){
							result=0;
						}else if((shd[INDEX((j+1),(i-1))].type==shd[INDEX((j+1),(i))].type)&&(shd[INDEX((j+1),(i-1))].type==0)){
							result=0;
						}else if((shd[INDEX((j+1),(i))].type==shd[INDEX((j+1),(i+1))].type)&&(shd[INDEX((j+1),(i))].type==0)){
							result=0;
						}else if((shd[INDEX((j+1),(i+1))].type==shd[INDEX((j),(i+1))].type)&&(shd[INDEX((j+1),(i+1))].type==0)){
							result=0;
						}else if((shd[INDEX((j),(i+1))].type==shd[INDEX((j-1),(i+1))].type)&&(shd[INDEX((j),(i+1))].type==0)){
							result=0;
						}else if((shd[INDEX((j-1),(i+1))].type==shd[INDEX((j-1),(i))].type)&&(shd[INDEX((j-1),(i+1))].type==0)){
							result=0;
						}
						break;
				}
			}
		
		}
	}
	
	return result;
}

void stampa_matrice(cell* shd)	/*Stampa della matrice*/
{
	int i,j;
	for(i=0;i<SO_HEIGHT;i++){
		printf("\n");
		for(j=0;j<SO_WIDTH;j++){
			if(shd[INDEX(j,i)].type==0){
				printf("  %d/X  ",shd[INDEX(j,i)].taxi_in);
			}else if(shd[INDEX(j,i)].type==1){
				printf("  %d/%d  ",shd[INDEX(j,i)].taxi_in,shd[INDEX(j,i)].cap);
			}else if(shd[INDEX(j,i)].type==2){
				printf(" [%d/%d] ",shd[INDEX(j,i)].taxi_in,shd[INDEX(j,i)].cap);
			}else if(shd[INDEX(j,i)].type==3){
				printf(" !%d/%d] ",shd[INDEX(j,i)].taxi_in,shd[INDEX(j,i)].cap);	/*Sorgente e TOP_CELL*/
			}else{
				printf(" !%d/%d  ",shd[INDEX(j,i)].taxi_in,shd[INDEX(j,i)].cap);	/*Normale e TOP_CELL*/
			}
		}
		printf("\n");
	}
	printf("\n");
}

void my_handler(int signum)	/*Handler per la generazione di richieste*/
{
	switch(signum){
		case SIGALRM:
			expired=1;
			break;
		case SIGINT:
			s=1;
			break;
	}
}

void p_handler(int signum){	/*Handler per la durata della simulazione*/
	switch(signum){
		case SIGALRM:
			ender=5;
			break;
	}
}

void send_quest(cell* shd,int time,int msg_id,int my_mtype)
{
	int my_pid;
	int x,y;
	struct sigaction sa;
	struct msgbuf mbuf;
	struct timespec now;
	bzero(&sa, sizeof(sa));
	sa.sa_handler=my_handler;
	sigaction(SIGALRM,&sa,NULL);
	sigaction(SIGINT,&sa,NULL);

	my_pid=getpid();
	printf("SOURCE PID:%d, mtype:%d\n",my_pid,my_mtype);
	while(1){
		alarm(time);
		while(!expired){
			if(s){
				expired=1;	
				s=0;
				printf("Genero richiesta causa SIGINT\nPID:%d\n",my_pid);

			}	
			wait();
		}
		clock_gettime(CLOCK_REALTIME,&now);
		srand(now.tv_nsec);
		x=rand()%SO_WIDTH;
		clock_gettime(CLOCK_REALTIME,&now);
		srand(now.tv_nsec);
		y=rand()%SO_HEIGHT;

		mbuf.mtype=my_mtype+1;
		mbuf.req.origin=my_mtype;
	
		if(shd[INDEX(x,y)].type==0 && INDEX(x,y)!=0){	/*Definisco la destinazione della richiesta*/
			mbuf.req.dest=(INDEX(x-1,y));
		}else if(shd[INDEX(x,y)].type==0 && INDEX(x,y)==0){
			mbuf.req.dest=(INDEX(x+1,y));
		}else{
			mbuf.req.dest=INDEX(x,y);
		}
		
		msgsnd(msg_id,&mbuf,sizeof(mbuf.req),0);
		expired=0;
	}
	exit(EXIT_SUCCESS);

}

int take_source(int sem_source,int x,int y,cell* shd)
{
	struct sembuf sops;
	static int offset=0;
	
	sops.sem_num=INDEX(x,y);
	sops.sem_op=-1;
	sops.sem_flg=IPC_NOWAIT;
	if(semop(sem_source,&sops,1)==-1){
		take_source(sem_source,x+1,y,shd);
		offset+=1;
	}else{
		return INDEX(x+offset,y);
	}
}

void set_sem(int sem2id,cell* shd)
{
	int i,j;

	for(i=0;i<SO_HEIGHT;i++){
		for(j=0;j<SO_WIDTH;j++){
			semctl(sem2id,INDEX(j,i),SETVAL,shd[INDEX(j,i)].cap);
		}
	}
}

void set_sem_source(int sem_source,cell* shd)
{
	int i,j;
	for(i=0;i<SO_HEIGHT;i++){
		for(j=0;j<SO_WIDTH;j++){
			if(shd[INDEX(j,i)].type==2){
				semctl(sem_source,INDEX(j,i),SETVAL,1);
			}else{
				semctl(sem_source,INDEX(j,i),SETVAL,0);
			}
		}
	}
}

taxi posizionamento(int sem2id,int x,int y,cell* shd)
{
	struct sembuf sops;
	struct taxi yellow_car;
	struct timespec now;

	sops.sem_num=INDEX(x,y);
	sops.sem_op=-1;
	sops.sem_flg=IPC_NOWAIT;
	if(semop(sem2id,&sops,1)==-1){
		clock_gettime(CLOCK_REALTIME,&now);
		srand(now.tv_nsec);
		x=rand()%SO_WIDTH;
		clock_gettime(CLOCK_REALTIME,&now);
		srand(now.tv_nsec);
		y=rand()%SO_HEIGHT;
		posizionamento(sem2id,x,y,shd);
	}else{
		if(shd[INDEX(x,y)].type==0 && INDEX(x,y)!=0){
			posizionamento(sem2id,x-1,y,shd);
		}else if(shd[INDEX(x,y)].type==0 && INDEX(x,y)==0){
			posizionamento(sem2id,x+1,y,shd);
		}else{
			yellow_car.now=INDEX(x,y);
			shd[INDEX(x,y)].taxi_in+=1;	/*Posiziono il taxi nella cella e ne aggiorno i valori*/
			shd[INDEX(x,y)].on+=1;
		}
	}
	return yellow_car;
}

void simulation(cell* shd,pid_t *all_origin,pid_t *all_taxi,int sources,int taxi)
{
	int j,i,k;
	int x,y;
	int time=2;
	int s=0,t=0;
	int sem_id,sem2id,sem_source;
	int msg_id;
	struct sigaction sg;
	struct sembuf sops;
	struct taxi yellow_car;
	struct timespec now;
	struct msgbuf mbuf;

	bzero(&sg, sizeof(sg));
	sg.sa_handler=p_handler;
	sigaction(SIGALRM,&sg,NULL);

	sem_id=semget(SEM_KEY,3,IPC_CREAT|0600);
	semctl(sem_id,0,SETVAL,0);
	semctl(sem_id,1,SETVAL,0);
	semctl(sem_id,2,SETVAL,0);

	sem2id=semget(SEM_KEY2,MAP,IPC_CREAT|0600);
	set_sem(sem2id,shd);

	sops.sem_flg=0;

	sem_source=semget(SEM_KEY3,MAP,IPC_CREAT|0600);
	set_sem_source(sem_source,shd);

	msg_id=msgget(MSGQ_KEY,IPC_CREAT|0600);

	printf("I PROCESSI SOURCES SONO:\n");
	for(i=0;i<sources;i++){
		switch(all_origin[i]=fork()){
			case 0:
				x=0;
				y=0;
				free(all_origin);

				k=take_source(sem_source,x,y,shd);

				sops.sem_num=0;	/*Semaforo per aspettare la creazione di tutte le sources*/
				sops.sem_op=1;
				semop(sem_id,&sops,1);

				sops.sem_num=2;	/*Blocco per sincronizzare con l'avvio del timer*/
				sops.sem_op=1;
				semop(sem_id,&sops,1);
				
				send_quest(shd,time,msg_id,k);
				exit(EXIT_SUCCESS);
		}
	}

	for(t=0;t<taxi;t++){
		switch(all_taxi[t]=fork()){
			case 0:
				free(all_taxi);

				clock_gettime(CLOCK_REALTIME,&now);
				srand(now.tv_nsec);
				x=rand()%SO_WIDTH;
				clock_gettime(CLOCK_REALTIME,&now);
				srand(now.tv_nsec);
				y=rand()%SO_HEIGHT;
				yellow_car=posizionamento(sem2id,x,y,shd);

				if(shd[yellow_car.now].type==2){
					msgrcv(msg_id,&mbuf,sizeof(mbuf.req),yellow_car.now+1,0);
				}
				printf("PID:%d, NOW:%d, ORIG:%d, DEST:%d\n",getpid(),yellow_car.now,mbuf.req.origin,mbuf.req.dest);

				sops.sem_num=1;	/*Semaforo per attendere la creazione di tutti i taxi*/
				sops.sem_op=1;
				semop(sem_id,&sops,1);

				sops.sem_num=2;	/*Blocco per sincronizzare con l'avvio del timer*/
				sops.sem_op=1;
				semop(sem_id,&sops,1);

				exit(EXIT_SUCCESS);
		}
	}

	ender=0;

	sops.sem_num=0;	/*Lascio partire le sources*/
	sops.sem_op=-sources;
	semop(sem_id,&sops,1);

	sops.sem_num=1;	/*Lascio partire i taxi*/
	sops.sem_op=-taxi;
	semop(sem_id,&sops,1);

	sops.sem_num=2;	/*Avvio la simulazione e il timer*/
	sops.sem_op=taxi+sources;
	semop(sem_id,&sops,1);

	now.tv_sec=1;
	now.tv_nsec=000000000;
	alarm(SO_DURATION);
	while(!ender){
		nanosleep(&now,NULL);
		stampa_matrice(shd);
	}

	semctl(sem_id,0,IPC_RMID);	/*Rimuovo il semaforo*/
	semctl(sem2id,0,IPC_RMID);	/*Rimuovo il semaforo*/
	semctl(sem_source,0,IPC_RMID);	/*Rimuovo il semaforo*/
	msgctl(msg_id,IPC_RMID,NULL);

	s=0;
	for(s=0;s<sources;s++){
		kill(all_origin[s],SIGTERM);
	}

	t=0;
	for(t=0;t<taxi;t++){
		kill(all_taxi[t],SIGTERM);
	}
}