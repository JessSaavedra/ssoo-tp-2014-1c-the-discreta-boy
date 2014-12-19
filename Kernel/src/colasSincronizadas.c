/*
 * colasCPU.c
 *
 *  Created on: 06/06/2014
 *      Author: utnso
 */

#include "kernel.h"

t_colaSincronizada* newColaSincronizada(void){

	t_colaSincronizada* cola = malloc(sizeof(t_colaSincronizada));

	cola->cola=list_create();
	cola->mutex=malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(cola->mutex,NULL);
	cola->semaforo=malloc(sizeof(sem_t));
	sem_init(cola->semaforo,0,0);
	return cola;
}

//TODO destructor de colas



void encolar(t_colaSincronizada* cola,void* elemento){
	pthread_mutex_lock(cola->mutex);
	list_add_in_index(cola->cola,list_size(cola->cola),elemento);
	pthread_mutex_unlock(cola->mutex);
	sem_post(cola->semaforo);
}

void deshacerRetirar(t_colaSincronizada* cola,void* elemento){
	pthread_mutex_lock(cola->mutex);
	list_add_in_index(cola->cola,0,elemento);
	pthread_mutex_unlock(cola->mutex);
	sem_post(cola->semaforo);
}

void encolar_order_by (t_colaSincronizada* cola,void* data, int(*isBigger)(void*,void*)){

		pthread_mutex_lock(cola->mutex);
		list_add_order_by(cola->cola,data,isBigger);
		pthread_mutex_unlock(cola->mutex);
		sem_post(cola->semaforo);
}

void* retirar(t_colaSincronizada* cola){

	void* elemento;

	sem_wait(cola->semaforo);
	pthread_mutex_lock(cola->mutex);
	elemento= list_remove(cola->cola,0);
	pthread_mutex_unlock(cola->mutex);
	return elemento;
}

int tamanioCola(t_colaSincronizada* cola){
	int tamanio;
	pthread_mutex_lock(cola->mutex);
	tamanio=list_size(cola->cola);
	pthread_mutex_unlock(cola->mutex);
	return tamanio;

}

