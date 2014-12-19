/*
 * main.c
 *
 *  Created on: 31/05/2014
 *      Author: utnso
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <commons/collections/queue.h>
#include "kernel.h"

t_colaSincronizada *colaCPUsListos,*colaCPUsEnUso,*colaProcesosListos,*colaProcesosFinalizados,*colaProcesosNuevos;
int quantum;
int socketUMV;
int tamanioStack;
int retardo;
char *puertoPP,*puertoCPU,*puertoUMV,*IPCPU,*IPUMV;
t_dictionary *variablesGlobales,*procesosEnEjecucion,*semaforos,*dispositivosIO;
t_log* logDebug;
sem_t* gradoDeMultiprogramacion;
sem_t* procesosNuevos;
int segmentosCreadosFlag;
pthread_mutex_t lockProcesosEnEjecucion;



int runTests(void);

int main(int argc, char** argv){

	logDebug = log_create("logDebugKernel.txt","Kernel",0,LOG_LEVEL_DEBUG);

	variablesGlobales = dictionary_create();
	procesosEnEjecucion = dictionary_create();
	dispositivosIO = dictionary_create();
	semaforos = dictionary_create();
	pthread_mutex_init(&lockProcesosEnEjecucion,NULL);
	gradoDeMultiprogramacion= malloc(sizeof(sem_t));
	procesosNuevos=malloc(sizeof(sem_t));


	sem_init(procesosNuevos,0,0);

	AdministrarInicio

	colaCPUsEnUso = newColaSincronizada();
	colaCPUsListos = newColaSincronizada();
	colaProcesosFinalizados = newColaSincronizada();
	colaProcesosListos = newColaSincronizada();
	colaProcesosNuevos = newColaSincronizada();

	//------Hilos IO----------------//

	dictionary_iterator(dispositivosIO,(void(*)(char*,void*))crearHilosIO);

	//------Hilos PLP----------------//

	pthread_t hiloOrganizadorDeProgramas;
	pthread_t hiloDeProgramasListos;
	pthread_t hiloDeProgramasEliminados;

	pthread_create(&hiloOrganizadorDeProgramas,NULL,(void* (*)(void*))organizadorDeProgramas,NULL);
	pthread_create(&hiloDeProgramasListos,NULL,(void* (*)(void*))planificadorDeProgramasListos,NULL);
	pthread_create(&hiloDeProgramasEliminados,NULL,(void* (*)(void*))planificadorDeProgramasEliminados,NULL);


	//------Hilos PCP----------------//

	pthread_t hiloDispatcher, hiloQueAtiendeCPUs,hiloRecepcionistaCPUs;
	int* socketEscuchaCPUs= malloc(sizeof(int));
	*socketEscuchaCPUs=crearSocketEscuchaSobre(puertoCPU,8); //TODO ver que onda, cantidad de cpus

	pthread_create(&hiloDispatcher,NULL,(void*(*)(void*))despacharProcesos,NULL);
	pthread_create(&hiloQueAtiendeCPUs,NULL,(void*(*)(void*))escucharPedidosCPU,NULL);
	pthread_create(&hiloRecepcionistaCPUs,NULL,(void*(*)(void*))recibirConexionesCPUs,socketEscuchaCPUs);

	pthread_join(hiloOrganizadorDeProgramas,NULL);
	pthread_join(hiloDeProgramasListos,NULL);
	pthread_join(hiloDeProgramasEliminados,NULL);
	pthread_join(hiloDispatcher,NULL);
	pthread_join(hiloQueAtiendeCPUs,NULL);
	pthread_join(hiloRecepcionistaCPUs,NULL);

	return 0;
}
