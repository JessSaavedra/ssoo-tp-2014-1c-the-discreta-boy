/*
 * dispatcher.c
 *
 *  Created on: 06/06/2014
 *      Author: utnso
 */

#include "kernel.h"

void despacharProcesos(void){

	t_cliente* CPU;
	tipoProceso* proceso;
	char* streamPCB;
	int aux,codigoVerificacionDevuelto,codigoVerificacionOriginal;
	while(1){														log_debug(logDebug,"Antes de desencolar un proceso para despachar hay %d en Ready",list_size(colaProcesosListos->cola));
		CPU = retirar(colaCPUsListos);								log_debug(logDebug,"Saque el cpu %d de la cola de CPUsListos",CPU->socket);
		proceso = (tipoProceso*) retirar(colaProcesosListos); 		log_debug(logDebug,"Saque el proceso %d de la cola, quedan %d",proceso->PCB.identificador,list_size(colaProcesosListos->cola));
		codigoVerificacionOriginal=rand()%100;
		aux=enviar(CPU->socket,&codigoVerificacionOriginal,sizeof(codigoVerificacionOriginal));			log_debug(logDebug,"Al enviar codigo verificacion enviar devolvio: %d",aux);
		if(aux<=0){																						log_debug(logDebug,"Se elimina al CPU muerto antes de despachar el proceso");
					deshacerRetirar(colaProcesosListos,proceso);
					free(CPU);//Saca al CPU muerto del sistema.
					continue;
				}
		aux=recv(CPU->socket,&codigoVerificacionDevuelto,sizeof(codigoVerificacionDevuelto),MSG_WAITALL);	log_debug(logDebug,"Antes de despachar proceso recv devuelve %d,el CPU esta %s",aux,(codigoVerificacionDevuelto==codigoVerificacionOriginal)?"vivo":"muerto");
		if((aux<=0)||(codigoVerificacionDevuelto!=codigoVerificacionOriginal)){								log_debug(logDebug,"Se elimina al CPU muerto antes de despachar el proceso");
			deshacerRetirar(colaProcesosListos,proceso);
			free(CPU);//Saca al CPU muerto del sistema.
			continue;
		}
		streamPCB = serializarPCB(proceso->PCB);
		char* cpu=string_itoa(CPU->socket);
		pthread_mutex_lock(&lockProcesosEnEjecucion);
		dictionary_put(procesosEnEjecucion,cpu,proceso);
		pthread_mutex_unlock(&lockProcesosEnEjecucion);
		aux=enviar(CPU->socket,streamPCB,11*TAMANIO_INT);			log_debug(logDebug,"Al despachar proceso enviar devolvio: %d",aux);
		if(aux<=0){													log_debug(logDebug,"Hubo un error de socket al intentar despachar el proceso al CPU");
			CPU->estaVivo=false;
			manejarExcepcion(CPUMURIO,CPU);
			free(cpu);
			free(streamPCB);
			continue;
		}
		encolar(colaCPUsEnUso, CPU);
		free(cpu);													log_debug(logDebug,"El proceso fue despachado");
		free(streamPCB);
	}
	return;
}

char* serializarPCB(tipoPCB PCB){

	char* streamPCB = malloc(11*sizeof(int));
	char* siguiente = streamPCB;

	grabarEnStream(&siguiente,PCB.identificador);
	grabarEnStream(&siguiente,PCB.segmentoDeCodigo);
	grabarEnStream(&siguiente,PCB.segmentoDeStack);
	grabarEnStream(&siguiente,PCB.cursorDelStack);
	grabarEnStream(&siguiente,PCB.indiceDeCodigo);
	grabarEnStream(&siguiente,PCB.indiceEtiquetas);
	grabarEnStream(&siguiente,PCB.programCounter);
	grabarEnStream(&siguiente,PCB.tamanioContextoActual);
	grabarEnStream(&siguiente,PCB.tamanioIndiceEtiquetas);
	grabarEnStream(&siguiente,quantum);
	grabarEnStream(&siguiente,retardo);

	return streamPCB;
}

void grabarEnStream(char** siguiente, int valor){

	memcpy(*siguiente,&valor,sizeof(valor));
	*siguiente+=sizeof(valor);
	return;
}










