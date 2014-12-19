/*
 * stubRecuperarPCB.c
 *
 *  Created on: 14/06/2014
 *      Author: utnso
 */

#include "kernel.h"

#define IPLOCAL "127.0.0.1"

void stubImprimirTextoCPU(char* valor){

	struct addrinfo criteriosConfiguracion;
	struct addrinfo* datosSocketServidor;
	int socketPCP;

	memset(&criteriosConfiguracion,0,sizeof(criteriosConfiguracion)); //Inicializo  en cero

	criteriosConfiguracion.ai_family=AF_UNSPEC;          //Cargo datos del socket
	criteriosConfiguracion.ai_socktype=SOCK_STREAM;

	getaddrinfo(IPLOCAL,"7020", &criteriosConfiguracion,&datosSocketServidor);

	socketPCP = socket(datosSocketServidor->ai_family,datosSocketServidor->ai_socktype,datosSocketServidor->ai_protocol );

	connect(socketPCP, datosSocketServidor->ai_addr,datosSocketServidor->ai_addrlen);log_debug(logDebug,"stubImprimirTextoCPU se conecto");
	freeaddrinfo(datosSocketServidor);
	//-------------------------------------------------------------------------------------------------//
	log_debug(logDebug,"stubImprimirTextoCPU a punto de crear stream");

	char* stream = malloc(TAMANIO_INT+5);
	int tamanio =5;

	log_debug(logDebug,"stubImprimirTextoCPU a punto de hacer memcpys");

	memcpy(stream,&tamanio,TAMANIO_INT);
	memcpy(stream+TAMANIO_INT,valor,tamanio);

	log_debug(logDebug,"stubImprimirTextoCPU a punto de enviar valor ");

	send(socketPCP,stream,TAMANIO_INT+5,0);

	log_debug(logDebug,"stubImprimirTextoCPU a punto de finalizar");

	close (socketPCP);

}
