/*
 * stubRecuperarPCB.c
 *
 *  Created on: 14/06/2014
 *      Author: utnso
 */

#include "kernel.h"

#define IPLOCAL "127.0.0.1"

void stubDespachar(tipoProceso* proceso){

	struct addrinfo criteriosConfiguracion;
	struct addrinfo* datosSocketServidor;
	int socketPCP;

	memset(&criteriosConfiguracion,0,sizeof(criteriosConfiguracion)); //Inicializo  en cero

	criteriosConfiguracion.ai_family=AF_UNSPEC;          //Cargo datos del socket
	criteriosConfiguracion.ai_socktype=SOCK_STREAM;

	getaddrinfo(IPLOCAL,"7011", &criteriosConfiguracion,&datosSocketServidor);

	socketPCP = socket(datosSocketServidor->ai_family,datosSocketServidor->ai_socktype,datosSocketServidor->ai_protocol );

	connect(socketPCP, datosSocketServidor->ai_addr,datosSocketServidor->ai_addrlen);
	freeaddrinfo(datosSocketServidor);
	//-------------------------------------------------------------------------------------------------//

	recv(socketPCP,&(proceso->PCB.identificador),sizeof(int),MSG_WAITALL);log_debug(logDebug,"recibi id %d",proceso->PCB.identificador);
	recv(socketPCP,&(proceso->PCB.segmentoDeCodigo),sizeof(int),MSG_WAITALL);log_debug(logDebug,"recibi segmento codigo %d",proceso->PCB.segmentoDeCodigo);
	recv(socketPCP,&(proceso->PCB.segmentoDeStack),sizeof(int),MSG_WAITALL);log_debug(logDebug,"recibi segmento stack %d",proceso->PCB.segmentoDeStack);
	recv(socketPCP,&(proceso->PCB.cursorDelStack),sizeof(int),MSG_WAITALL);log_debug(logDebug,"recibi cursor stack %d",proceso->PCB.cursorDelStack);
	recv(socketPCP,&(proceso->PCB.indiceDeCodigo),sizeof(int),MSG_WAITALL);log_debug(logDebug,"recibi indice codigo %d",proceso->PCB.indiceDeCodigo);
	recv(socketPCP,&(proceso->PCB.indiceEtiquetas),sizeof(int),MSG_WAITALL);log_debug(logDebug,"recibi indice etiquetas %d",proceso->PCB.indiceEtiquetas);
	recv(socketPCP,&(proceso->PCB.programCounter),sizeof(int),MSG_WAITALL);log_debug(logDebug,"recibi pc %d",proceso->PCB.programCounter);
	recv(socketPCP,&(proceso->PCB.tamanioContextoActual),sizeof(int),MSG_WAITALL);log_debug(logDebug,"recibi tamanio contexto %d",proceso->PCB.tamanioContextoActual);
	recv(socketPCP,&(proceso->PCB.tamanioIndiceEtiquetas),sizeof(int),MSG_WAITALL);log_debug(logDebug,"recibi tam indice etiquetas %d",proceso->PCB.tamanioIndiceEtiquetas);

	log_debug(logDebug,"stubDespachar finalizo");

	close(socketPCP);
}
