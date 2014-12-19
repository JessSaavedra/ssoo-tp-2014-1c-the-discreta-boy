/*
 * funcionesIO.c
 *
 *  Created on: 26/06/2014
 *      Author: utnso
 */

#include <stdio.h>
#include "kernel.h"
#include <pthread.h>


void crearHilosIO(char* key,tipoDispositivo* dispositivo){
	dispositivo->colaDeIO=newColaSincronizada();
	pthread_create(&dispositivo->hiloIO,NULL,(void*(*)(void*))ejecutarIO,dispositivo);	log_debug(logDebug,"Se creo un hilo de IO para el dispositivo:%s",key);
}


void ejecutarIO(tipoDispositivo* dispositivo){
	while (1){
		tipoProcesoEnIO* procesoEnIO = retirar(dispositivo->colaDeIO);	log_debug(logDebug,"Se ejecutara la operacion de IO para el proceso %d",(procesoEnIO->proceso)->PCB.identificador);
		milisleep(dispositivo->valorDeRetardo*procesoEnIO->cantidadUnidadesDeIO);
		encolar(colaProcesosListos,procesoEnIO->proceso);
	}
}


