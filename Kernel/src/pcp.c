/*
 * pcp.c
 *
 *  Created on: 02/06/2014
 *      Author: utnso
 */

#include "kernel.h"

void pcp(void){

	pthread_t hiloDispatcher, hiloQueAtiendeCPUs,hiloRecepcionistaCPUs;
	int* socketEscuchaCPUs= malloc(sizeof(int));
	*socketEscuchaCPUs=crearSocketEscuchaSobre(puertoCPU,8); //TODO ver que onda, cantidad de cpus

	pthread_create(&hiloDispatcher,NULL,(void*(*)(void*))despacharProcesos,NULL);
	pthread_create(&hiloQueAtiendeCPUs,NULL,(void*(*)(void*))escucharPedidosCPU,NULL);
	pthread_create(&hiloRecepcionistaCPUs,NULL,(void*(*)(void*))recibirConexionesCPUs,socketEscuchaCPUs);

	pthread_join(hiloDispatcher,NULL);
	pthread_join(hiloQueAtiendeCPUs,NULL);
	pthread_join(hiloRecepcionistaCPUs,NULL);

	return;
}

void recibirConexionesCPUs(int* escuchador){

	int socketCPU,socketEscucha=*escuchador;

	while(1){
		socketCPU = accept(socketEscucha,NULL,NULL);	log_debug(logDebug,"Se ha conectado un nuevo CPU y se le ha dado el socket %d",socketCPU);
		encolar(colaCPUsListos,newCliente(socketCPU));	log_debug(logDebug,"Se ha encolado al CPU nuevo");
	}
	return;
}

tipoPCB* newPCB(int identificador,int segmentoDeCodigo ,int segmentoDeStack,int cursorDelStack,int indiceDeCodigo,\
	int indiceEtiquetas,int programCounter,int tamanioContextoActual,int tamanioIndiceEtiquetas){

	tipoPCB* PCB=malloc(sizeof(tipoPCB));

	PCB->identificador = identificador;
	PCB->segmentoDeCodigo=segmentoDeCodigo;
	PCB->segmentoDeStack=segmentoDeStack;
	PCB->cursorDelStack=cursorDelStack;
	PCB->indiceDeCodigo=indiceDeCodigo;
	PCB->indiceEtiquetas=indiceEtiquetas;
	PCB->programCounter=programCounter;
	PCB->tamanioContextoActual=tamanioContextoActual;
	PCB->tamanioIndiceEtiquetas=tamanioIndiceEtiquetas;

	return PCB;
}
