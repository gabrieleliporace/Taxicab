#define _GNU_SOURCE
#include "taxicab.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int cap_min,cap_max,timensec_min,timensec_max;
int sources,holes,normali;

int random_type()
{
	int result;
	srand(getpid());
	switch(sources){ 	/*controllo le sources*/
		case 0:
			switch(holes){	/*sources=0, controllo degli holes*/
				case 0:
					result=1;
					normali--;
					break;
				default:
					switch(normali){	/*sources=0, holes!=0, controllo normali*/
						case 0:
							result=0;
							holes--;
							break;
						default:
							switch(rand()%2){	/*randomico tra normali e holes*/
								case 0:
									result=0;
									holes--;
									break;
								default:
									result=1;
									normali--;
									break;
							}
							break;
					}
					break;
			}
			break;

		default:
		switch(holes){	/*sources!=0, controllo degli holes*/	
			case 0:
				switch(normali){	/*sources!=0, holes=0, controllo normali*/
					case 0:
						result=2;
						sources--;
						break;
					default:
						switch(rand()%2){	/*randomico tra normali e sources*/
							case 0:
							result=1;
							normali--;
							break;
						default:
							result=2;
							sources--;
							break;
					}
					break;
				}
				break;
			default:
				switch(normali){	/*sources!=0, holes!=0, controllo normali*/
					case 0:
						switch(rand()%2){	/*randomico tra holes e sources*/
							case 0:
								result=0;
								holes--;
								break;
							default:
								result=2;
								sources--;
								break;
						}
						break;
					default:
						switch(rand()%3){	/*randomico tra i tre valori*/
							case 0:
								result=0;
								holes--;
								break;
							case 1:
								result=1;
								normali--;
								break;
							default:
								result=2;
								sources--;
								break;
						}
						break;
				}
				break;
		}
	}
	return result;	
}

int random_cap()
{
	srand(getpid());
	return rand()%((cap_max+1)-cap_min)+cap_min;
}

int random_timensec()
{
	srand(getpid());
	return rand()%((timensec_max+1)-timensec_min)+timensec_min;
}

int main(int argc,char* argv[]){
	int i,j,m_id,fd;
	struct cell * my_arr;
	char buff[100];

	i=atoi(argv[1]);
	j=atoi(argv[2]);
	cap_max=atoi(argv[3]);
	cap_min=atoi(argv[4]);
	timensec_min=atoi(argv[5]);
	timensec_max=atoi(argv[6]);
	sources=atoi(argv[7]);
	holes=atoi(argv[8]);
	normali=atoi(argv[9]);
	fd=atoi(argv[10]);

	m_id=shmget(SHM_KEY,sizeof(*my_arr),IPC_CREAT|0600);
	my_arr=shmat(m_id,NULL,0);
	my_arr[INDEX(j,i)].type=random_type();
	my_arr[INDEX(j,i)].cap=random_cap();
	my_arr[INDEX(j,i)].timensec=random_timensec();

	dprintf(0,"%d\n",holes);	
	dprintf(fd,"%d\n",normali);
	dprintf(1,"%d\n",sources);

	return 0;
}
