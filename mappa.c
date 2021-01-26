#define _GNU_SOURCE
#include "taxicab.h"
#include <stdlib.h>
#include <unistd.h> 
#include <sys/ipc.h>
#include <sys/shm.h>

/*Utilizzo di macro per inizializzare le dimensioni della citta'*/
#define CHILD_NAME "./random-set"
int m_id;
int source;
int HOLES,CAP_MIN, CAP_MAX;
int TIMENSEC_MIN, TIMENSEC_MAX, NORM;

void valori(int source)
{
	/*Creazione degli stream*/
	FILE* taxicab_init;

	/*Creazione array per inserimento valori da stdin*/
	char choose[4];

	taxicab_init=fopen("./init.txt","r");

	/*Lettura da file ed inizializzazione valori della citta'*/

	fscanf(taxicab_init,"%d",&HOLES);
	fscanf(taxicab_init,"%d",&CAP_MIN);
	fscanf(taxicab_init,"%d",&CAP_MAX);
	fscanf(taxicab_init,"%d",&TIMENSEC_MIN);
	fscanf(taxicab_init,"%d",&TIMENSEC_MAX);
	
	NORM=MAP-(source+HOLES);

	fclose(taxicab_init);
}

cell* shm_matrix()
{
	int i=0,j=0,fd;
	int status;
	int so_sources,so_holes;
	int normali;

	char* buff;
	char* my_buff;
	char* buffer;
	int pipefd[2];
	int my_pipe[2];
	int piper[2];

	char i_str[3*sizeof(i)+1];
	char j_str[3*sizeof(j)+1];

	char cap_max_str[3*sizeof(CAP_MAX)+1];
	char cap_min_str[3*sizeof(CAP_MIN)+1];
	char timensec_min_str[3*sizeof(TIMENSEC_MIN)+1];
	char timensec_max_str[3*sizeof(TIMENSEC_MAX)+1];
	char source_str[3*sizeof(source)+1];
	char holes_str[3*sizeof(HOLES)+1];
	char normali_str[3*sizeof(NORM)+1];
	char fd_str[3*sizeof(fd)+1];

	char * args[12]={CHILD_NAME};

	cell * my_arr; 
	
	so_sources=source;
	so_holes=HOLES;
	normali=NORM;

	pipe(pipefd);
	pipe(my_pipe);
	pipe(piper);

	sprintf(cap_max_str,"%d",CAP_MAX);
	sprintf(cap_min_str,"%d",CAP_MIN);
	sprintf(timensec_min_str,"%d",TIMENSEC_MIN);
	sprintf(timensec_max_str,"%d",TIMENSEC_MAX);

	args[3]=cap_max_str;
	args[4]=cap_min_str;
	args[5]=timensec_min_str;
	args[6]=timensec_max_str;
	args[11]=NULL;
	
	m_id=shmget(SHM_KEY,sizeof(*my_arr),IPC_CREAT|0600);
	my_arr=shmat(m_id,NULL,0);

	buff=malloc(100);
	my_buff=malloc(100);
	buffer=malloc(100);

	for(i=0;i<SO_HEIGHT;i++){
		for(j=0;j<SO_WIDTH;j++){
			my_arr[INDEX(j,i)].type=1;
		}
	}

	for(i=0;i<SO_HEIGHT;i++){
		for(j=0;j<SO_WIDTH;j++){
			sprintf(i_str,"%d",i);
			sprintf(j_str,"%d",j);
			sprintf(source_str,"%d",so_sources);
			sprintf(holes_str,"%d",so_holes);
			sprintf(normali_str,"%d",normali);
			args[1]=i_str;
		   	args[2]=j_str;	
			args[7]=source_str;
			args[8]=holes_str;
			args[9]=normali_str;
			switch(fork()){
					case -1:
						exit(EXIT_FAILURE);
					case 0:

						dup2(pipefd[1],1);
						close(pipefd[1]);

						fd=dup(0);
						dup2(piper[1],fd);
						close(piper[1]);
						sprintf(fd_str,"%d",fd);
						args[10]=fd_str;
							

						dup2(my_pipe[1],0);
						close(my_pipe[1]);

						execve(CHILD_NAME,args,NULL);
					break;
					default:

						read(pipefd[0],buff,100);
						so_sources=atoi(buff);
						
						read(my_pipe[0],my_buff,100);
						so_holes=atoi(my_buff);
						
						read(piper[0],buffer,100);
						normali=atoi(buffer);
						
			}
		}
	}
	while(wait(&status)!= -1){
	
	}
	free(buff);
	free(my_buff);
	scambia(my_arr);
	return my_arr;
}

int controllo_valori()
{
	int result;
	if((HOLES<=H_MAP) && (NORM>=0)){
			result=1;	
	}else{
		printf("\n");
		printf("Valori non accettabili\n");
		printf("SO_HOLES:%d\nSO_SOURCES:%d\nMAP:%d\nMAX HOLES:%d\n",
			   	HOLES, source, MAP, H_MAP);
		printf("\n");
		printf("Causa del blocco:\n%d-(%d+%d)<0 oppure %d>=%d.\n",
			   	MAP, HOLES, source, HOLES, H_MAP);
		result=0;
	}
	return result;
}

void scambia(cell* my_matrix)
{
	int i, j;
	int buffer_type, buffer_cap, buffer_time;
	
	for(i=0;i<SO_HEIGHT;i++){
		for(j=0;j<SO_WIDTH;j++){
			switch(my_matrix[INDEX(j, i)].type){
				case 0:
					if(j!=0 && i==0){	/*Modifica prima riga*/
						if(my_matrix[INDEX(j-1,i)].type==0){
							if(j<=(SO_WIDTH-1)/2){
								buffer_type=my_matrix[INDEX(j, i)].type;
								buffer_cap=my_matrix[INDEX(j, i)].cap;
								buffer_time=my_matrix[INDEX(j, i)].timensec;
								
								my_matrix[INDEX(j, i)].type = my_matrix[INDEX(i, j)].type;
								my_matrix[INDEX(j, i)].cap = my_matrix[INDEX(i, j)].cap;
								my_matrix[INDEX(j, i)].timensec = my_matrix[INDEX(i, j)].timensec;
								
								my_matrix[INDEX(i, j)].type=buffer_type;
								my_matrix[INDEX(i, j)].cap=buffer_cap;
								my_matrix[INDEX(i, j)].timensec=buffer_time;
							}else{	
								buffer_type=my_matrix[INDEX(j, i)].type;
								buffer_cap=my_matrix[INDEX(j, i)].cap;
								buffer_time=my_matrix[INDEX(j, i)].timensec;
								
								my_matrix[INDEX(j, i)].type = my_matrix[INDEX(j,(SO_HEIGHT-(i+1)))].type;
								my_matrix[INDEX(j, i)].cap = my_matrix[INDEX(j,(SO_HEIGHT-(i+1)))].cap;
								my_matrix[INDEX(j, i)].timensec = my_matrix[INDEX(j,(SO_HEIGHT-(i+1)))].timensec;
								
								my_matrix[INDEX(j,(SO_HEIGHT-(i+1)))].type=buffer_type;
								my_matrix[INDEX(j,(SO_HEIGHT-(i+1)))].cap=buffer_cap;
								my_matrix[INDEX(j,(SO_HEIGHT-(i+1)))].timensec=buffer_time;
							}
						}
					}else if(j==0 && i!=0){	/*Modifica prima colonna*/
						if(my_matrix[INDEX(j,i-1)].type==0){
							buffer_type=my_matrix[INDEX(j, i)].type;
							buffer_cap=my_matrix[INDEX(j, i)].cap;
							buffer_time=my_matrix[INDEX(j, i)].timensec;
						
							my_matrix[INDEX(j,i)].type=my_matrix[INDEX((SO_WIDTH-(i+1)),i)].type;
							my_matrix[INDEX(j,i)].cap=my_matrix[INDEX((SO_WIDTH-(i+1)),i)].cap;
							my_matrix[INDEX(j,i)].timensec=my_matrix[INDEX((SO_WIDTH-(i+1)),i)].timensec;

							my_matrix[INDEX((SO_WIDTH-(i+1)),i)].type=buffer_type;
							my_matrix[INDEX((SO_WIDTH-(i+1)),i)].cap=buffer_cap;
							my_matrix[INDEX((SO_WIDTH-(i+1)),i)].timensec=buffer_time;
						}
					}else if((j>=1 && i>=1) && (i<((SO_HEIGHT)-1) && j<((SO_WIDTH)-1))){
						if(my_matrix[INDEX((j-1),(i-1))].type==0){
							buffer_type=my_matrix[INDEX(j, i)].type;
							buffer_cap=my_matrix[INDEX(j, i)].cap;
							buffer_time=my_matrix[INDEX(j, i)].timensec;
						
							my_matrix[INDEX(j,i)].type=my_matrix[INDEX((SO_WIDTH-(j+1)),(SO_HEIGHT-(i+1)))].type;
							my_matrix[INDEX(j,i)].cap=my_matrix[INDEX((SO_WIDTH-(i+1)),(SO_HEIGHT-(i+1)))].cap;
							my_matrix[INDEX(j,i)].timensec=my_matrix[INDEX((SO_WIDTH-(i+1)),(SO_HEIGHT-(i+1)))].timensec;

							my_matrix[INDEX((SO_WIDTH-(i+1)),(SO_HEIGHT-(i+1)))].type=buffer_type;
							my_matrix[INDEX((SO_WIDTH-(i+1)),(SO_HEIGHT-(i+1)))].cap=buffer_cap;
							my_matrix[INDEX((SO_WIDTH-(i+1)),(SO_HEIGHT-(i+1)))].timensec=buffer_time;
						}else if(my_matrix[INDEX(j-1,i)].type==0){
							buffer_type=my_matrix[INDEX(j, i)].type;
							buffer_cap=my_matrix[INDEX(j, i)].cap;
							buffer_time=my_matrix[INDEX(j, i)].timensec;
						
							my_matrix[INDEX(j,i)].type=my_matrix[INDEX((SO_WIDTH-(j+1)),(SO_HEIGHT-(i+1)))].type;
							my_matrix[INDEX(j,i)].cap=my_matrix[INDEX((SO_WIDTH-(i+1)),(SO_HEIGHT-(i+1)))].cap;
							my_matrix[INDEX(j,i)].timensec=my_matrix[INDEX((SO_WIDTH-(i+1)),(SO_HEIGHT-(i+1)))].timensec;

							my_matrix[INDEX((SO_WIDTH-(i+1)),(SO_HEIGHT-(i+1)))].type=buffer_type;
							my_matrix[INDEX((SO_WIDTH-(i+1)),(SO_HEIGHT-(i+1)))].cap=buffer_cap;
							my_matrix[INDEX((SO_WIDTH-(i+1)),(SO_HEIGHT-(i+1)))].timensec=buffer_time;
						}else if(my_matrix[INDEX((j-1),(i+1))].type==0){
							buffer_type=my_matrix[INDEX(j, i)].type;
							buffer_cap=my_matrix[INDEX(j, i)].cap;
							buffer_time=my_matrix[INDEX(j, i)].timensec;
						
							my_matrix[INDEX(j,i)].type=my_matrix[INDEX((SO_WIDTH-(j+1)),(SO_HEIGHT-(i+1)))].type;
							my_matrix[INDEX(j,i)].cap=my_matrix[INDEX((SO_WIDTH-(i+1)),(SO_HEIGHT-(i+1)))].cap;
							my_matrix[INDEX(j,i)].timensec=my_matrix[INDEX((SO_WIDTH-(i+1)),(SO_HEIGHT-(i+1)))].timensec;

							my_matrix[INDEX((SO_WIDTH-(i+1)),(SO_HEIGHT-(i+1)))].type=buffer_type;
							my_matrix[INDEX((SO_WIDTH-(i+1)),(SO_HEIGHT-(i+1)))].cap=buffer_cap;
							my_matrix[INDEX((SO_WIDTH-(i+1)),(SO_HEIGHT-(i+1)))].timensec=buffer_time;
						}else if(my_matrix[INDEX(j,(i-1))].type==0){
							buffer_type=my_matrix[INDEX(j, i)].type;
							buffer_cap=my_matrix[INDEX(j, i)].cap;
							buffer_time=my_matrix[INDEX(j, i)].timensec;
						
							my_matrix[INDEX(j,i)].type=my_matrix[INDEX((SO_WIDTH-(j+1)),(SO_HEIGHT-(i+1)))].type;
							my_matrix[INDEX(j,i)].cap=my_matrix[INDEX((SO_WIDTH-(i+1)),(SO_HEIGHT-(i+1)))].cap;
							my_matrix[INDEX(j,i)].timensec=my_matrix[INDEX((SO_WIDTH-(i+1)),(SO_HEIGHT-(i+1)))].timensec;

							my_matrix[INDEX((SO_WIDTH-(i+1)),(SO_HEIGHT-(i+1)))].type=buffer_type;
							my_matrix[INDEX((SO_WIDTH-(i+1)),(SO_HEIGHT-(i+1)))].cap=buffer_cap;
							my_matrix[INDEX((SO_WIDTH-(i+1)),(SO_HEIGHT-(i+1)))].timensec=buffer_time;
						}else if(my_matrix[INDEX(j,(i+1))].type==0){
							buffer_type=my_matrix[INDEX(j, i)].type;
							buffer_cap=my_matrix[INDEX(j, i)].cap;
							buffer_time=my_matrix[INDEX(j, i)].timensec;
						
							my_matrix[INDEX(j,i)].type=my_matrix[INDEX((SO_WIDTH-(j+1)),(SO_HEIGHT-(i+1)))].type;
							my_matrix[INDEX(j,i)].cap=my_matrix[INDEX((SO_WIDTH-(i+1)),(SO_HEIGHT-(i+1)))].cap;
							my_matrix[INDEX(j,i)].timensec=my_matrix[INDEX((SO_WIDTH-(i+1)),(SO_HEIGHT-(i+1)))].timensec;

							my_matrix[INDEX((SO_WIDTH-(i+1)),(SO_HEIGHT-(i+1)))].type=buffer_type;
							my_matrix[INDEX((SO_WIDTH-(i+1)),(SO_HEIGHT-(i+1)))].cap=buffer_cap;
							my_matrix[INDEX((SO_WIDTH-(i+1)),(SO_HEIGHT-(i+1)))].timensec=buffer_time;
						}else if(my_matrix[INDEX((j+1),i)].type==0){
							buffer_type=my_matrix[INDEX(j, i)].type;
							buffer_cap=my_matrix[INDEX(j, i)].cap;
							buffer_time=my_matrix[INDEX(j, i)].timensec;
						
							my_matrix[INDEX(j,i)].type=my_matrix[INDEX((SO_WIDTH-(j+1)),(SO_HEIGHT-(i+1)))].type;
							my_matrix[INDEX(j,i)].cap=my_matrix[INDEX((SO_WIDTH-(i+1)),(SO_HEIGHT-(i+1)))].cap;
							my_matrix[INDEX(j,i)].timensec=my_matrix[INDEX((SO_WIDTH-(i+1)),(SO_HEIGHT-(i+1)))].timensec;

							my_matrix[INDEX((SO_WIDTH-(i+1)),(SO_HEIGHT-(i+1)))].type=buffer_type;
							my_matrix[INDEX((SO_WIDTH-(i+1)),(SO_HEIGHT-(i+1)))].cap=buffer_cap;
							my_matrix[INDEX((SO_WIDTH-(i+1)),(SO_HEIGHT-(i+1)))].timensec=buffer_time;
						}else if(my_matrix[INDEX((j+1),(i+1))].type==0){
							buffer_type=my_matrix[INDEX(j, i)].type;
							buffer_cap=my_matrix[INDEX(j, i)].cap;
							buffer_time=my_matrix[INDEX(j, i)].timensec;
						
							my_matrix[INDEX(j,i)].type=my_matrix[INDEX((SO_WIDTH-(j+1)),(SO_HEIGHT-(i+1)))].type;
							my_matrix[INDEX(j,i)].cap=my_matrix[INDEX((SO_WIDTH-(i+1)),(SO_HEIGHT-(i+1)))].cap;
							my_matrix[INDEX(j,i)].timensec=my_matrix[INDEX((SO_WIDTH-(i+1)),(SO_HEIGHT-(i+1)))].timensec;

							my_matrix[INDEX((SO_WIDTH-(i+1)),(SO_HEIGHT-(i+1)))].type=buffer_type;
							my_matrix[INDEX((SO_WIDTH-(i+1)),(SO_HEIGHT-(i+1)))].cap=buffer_cap;
							my_matrix[INDEX((SO_WIDTH-(i+1)),(SO_HEIGHT-(i+1)))].timensec=buffer_time;
						}
					}
					break;
				default:
					break;
			}
		}
	}
}

cell* make_city(cell* my_matrix,int source)
{	
	valori(source);
	if(controllo_valori()){
		my_matrix=shm_matrix();
	}
	return my_matrix;
}

int main(int argc, char * argv[])
{
	cell* my_matrix;
	source=atoi(argv[0]);
	my_matrix=make_city(my_matrix,source);
	shmctl(m_id,IPC_RMID,NULL);
	return 0;
}
