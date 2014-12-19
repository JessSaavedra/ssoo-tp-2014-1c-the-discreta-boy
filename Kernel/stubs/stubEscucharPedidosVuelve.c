/*
 * stubRecuperarPCB.c
 *
 *  Created on: 14/06/2014
 *      Author: utnso
 */

#include "kernel.h"

#define IPLOCAL "127.0.0.1"

void stubEscucharPedidosVuelve(void){

	struct addrinfo criteriosConfiguracion;
	struct addrinfo* datosSocketServidor;
	int socketPCP;

	memset(&criteriosConfiguracion,0,sizeof(criteriosConfiguracion)); //Inicializo  en cero

	criteriosConfiguracion.ai_family=AF_UNSPEC;          //Cargo datos del socket
	criteriosConfiguracion.ai_socktype=SOCK_STREAM;

	getaddrinfo(IPLOCAL,"6015", &criteriosConfiguracion,&datosSocketServidor);

	socketPCP = socket(datosSocketServidor->ai_family,datosSocketServidor->ai_socktype,datosSocketServidor->ai_protocol );

	connect(socketPCP, datosSocketServidor->ai_addr,datosSocketServidor->ai_addrlen);
	freeaddrinfo(datosSocketServidor);
	//-------------------------------------------------------------------------------------------------//

	char* streamPCB = malloc(11*TAMANIO_INT);
	int cop =QUANTUMFINALIZADO;

	memcpy(streamPCB,&cop,TAMANIO_INT);

	recv(socketPCP,streamPCB+TAMANIO_INT,10*TAMANIO_INT,MSG_WAITALL);

	log_debug(logDebug,"stubEPVuelve a punto de enviar");

	send(socketPCP,streamPCB,11*TAMANIO_INT,0);

	log_debug(logDebug,"stubEPVuelve finalizo");

	close(socketPCP);
}
