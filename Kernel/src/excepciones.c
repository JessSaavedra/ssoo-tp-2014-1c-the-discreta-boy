/*
 * excepciones.c
 *
 *  Created on: 19/06/2014
 *      Author: utnso
 */

#include "kernel.h"

void manejarExcepcion(int codigoOperacion,t_cliente* CPU){
	int error=0;
	char* cpu=string_itoa(CPU->socket);
	pthread_mutex_lock(&lockProcesosEnEjecucion);
	tipoProceso* proceso = dictionary_remove(procesosEnEjecucion,cpu);
	pthread_mutex_unlock(&lockProcesosEnEjecucion);
	free(cpu);

	switch(codigoOperacion){
		case CPUMURIO:																							log_debug(logDebug,"Se maneja excepcion CPUMURIO para el cpu %d",CPU->socket);
			notificar(proceso,"ERROR: El CPU a cargo del proceso cerro de manera inesperada y tuvo que detenerse la ejecucion del programa");
			error=CPUMURIO;
			break;

		case SEGMENTATIONFAULT:																					log_debug(logDebug,"Se maneja excepcion SEGFAULT para el cpu %d",CPU->socket);
			notificar(proceso,"ERROR: Violacion de acceso a un segmento de memoria");
			break;

		case STACKOVERFLOW:																						log_debug(logDebug,"Se maneja excepcion STACKOVERFLOOO (BellesiStyle) para el cpu %d",CPU->socket);
			notificar(proceso,"ERROR: The long awaited ESTA COVERFLOW");
			break;

		case LEER_COMPARTIDAINEXISTENTE:																		log_debug(logDebug,"Se maneja excepcion LEERCOMPARTIDAINEXISTENTE para el cpu %d",CPU->socket);
			notificar(proceso,"ERROR: Se intento acceder a una variable compartida inexistente");
			break;

		case ESCRIBIR_COMPARTIDAINEXISTENTE:																	log_debug(logDebug,"Se maneja excepcion ESCRIBIRCOMPARTIDAINEXISTENTE para el cpu %d",CPU->socket);
			notificar(proceso,"ERROR: Se intento acceder a una variable compartida inexistente");
			error=recibirPendientes(ESCRIBIR_COMPARTIDAINEXISTENTE,CPU);
			break;

		case SEMAFOROINEXISTENTE:																				log_debug(logDebug,"Se maneja excepcion SEMAFOROINEXISTENTE para el cpu %d",CPU->socket);
			notificar(proceso,"ERROR: Se intento acceder a un semaforo inexistente");
			break;

		case IOINEXISTENTE:																						log_debug(logDebug,"Se maneja excepcion IOINEXISTENTE para el cpu %d",CPU->socket);
			notificar(proceso,"ERROR: Se intento utilizar un dispositivo de Entrada/Salida inexistente");
			error=recibirPendientes(IOINEXISTENTE,CPU);
			break;


		default: //En caso de recibir un codigo raro se asume que el CPU murio y se mantiene al sistema en un estado consistente
			notificar(proceso,"ERROR: El CPU a cargo del proceso cerro de manera inesperada y tuvo que detenerse la ejecucion del programa");
			error=CPUMURIO;
			break;
		}

	encolar(colaProcesosFinalizados,proceso);																	log_debug(logDebug,"Proceso encolado en Finalizados");
	if(error<0){
		log_debug(logDebug,"Se liberará el CPU %d",CPU->socket);
		free(CPU);
		log_debug(logDebug,"CPU Liberado");
	}
	else {
		log_debug(logDebug,"Se reencolara el CPU %d",CPU->socket);
		encolar(colaCPUsListos,CPU);
		log_debug(logDebug,"CPU reencolado");
	}
	return;
}

void notificar(tipoProceso* proceso, char* mensaje){
	int tamanioMensaje=strlen(mensaje)+1;
	int codigoAEnviar=1;

	int tamanioPaquete=tamanioMensaje+TAMANIO_INT+TAMANIO_INT;
	char* paquete=malloc(tamanioPaquete);

	memcpy(paquete,&codigoAEnviar,TAMANIO_INT);
	memcpy(paquete+TAMANIO_INT,&tamanioMensaje,TAMANIO_INT);
	memcpy(paquete+TAMANIO_INT+TAMANIO_INT,mensaje,tamanioMensaje);

	enviar(proceso->socketPP,paquete,tamanioPaquete);

}

int recibirPendientes(int codigoExcepcion,t_cliente* CPU){
	int error=0;
	int bufferAux=0;
	switch(codigoExcepcion){
		case ESCRIBIR_COMPARTIDAINEXISTENTE:
			recibirDeCliente(CPU,&bufferAux,TAMANIO_INT);//Recibir valor de la compartida
			break;
		case IOINEXISTENTE:
															  //Recibir PCB
			recibirDeCliente(CPU,&bufferAux,TAMANIO_INT);//	int identificador;
			recibirDeCliente(CPU,&bufferAux,TAMANIO_INT);//	int segmentoDeCodigo;
			recibirDeCliente(CPU,&bufferAux,TAMANIO_INT);//	int segmentoDeStack;
			recibirDeCliente(CPU,&bufferAux,TAMANIO_INT);//	int cursorDelStack;
			recibirDeCliente(CPU,&bufferAux,TAMANIO_INT);//	int indiceDeCodigo;
			recibirDeCliente(CPU,&bufferAux,TAMANIO_INT);//	int indiceEtiquetas;
			recibirDeCliente(CPU,&bufferAux,TAMANIO_INT);//	int programCounter;
			recibirDeCliente(CPU,&bufferAux,TAMANIO_INT);//	int tamanioContextoActual;
			recibirDeCliente(CPU,&bufferAux,TAMANIO_INT);//	int tamanioIndiceEtiquetas;

			recibirDeCliente(CPU,&bufferAux,TAMANIO_INT);//Recibir Tiempo de IO
			break;
		default:
			break;
	}

	return error;

}
