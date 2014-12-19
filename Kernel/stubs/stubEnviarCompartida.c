/*
 * stubRecuperarPCB.c
 *
 *  Created on: 14/06/2014
 *      Author: utnso
 */

#include "kernel.h"

#define IPLOCAL "127.0.0.1"

void stubEnviarCompartida(int* compartida){

	struct addrinfo criteriosConfiguracion;
	struct addrinfo* datosSocketServidor;
	int socketPCP;

	memset(&criteriosConfiguracion,0,sizeof(criteriosConfiguracion)); //Inicializo  en cero

	criteriosConfiguracion.ai_family=AF_UNSPEC;          //Cargo datos del socket
	criteriosConfiguracion.ai_socktype=SOCK_STREAM;

	getaddrinfo(IPLOCAL,"7012", &criteriosConfiguracion,&datosSocketServidor);

	socketPCP = socket(datosSocketServidor->ai_family,datosSocketServidor->ai_socktype,datosSocketServidor->ai_protocol );

	connect(socketPCP, datosSocketServidor->ai_addr,datosSocketServidor->ai_addrlen);
	freeaddrinfo(datosSocketServidor);
	//-------------------------------------------------------------------------------------------------//

	int tamanio=4;
	char* stream=malloc(TAMANIO_INT+tamanio);
	char* nombre = "jose";

	memcpy(stream,&tamanio, TAMANIO_INT);
	memcpy(stream+TAMANIO_INT,nombre,tamanio);				log_debug(logDebug,"stubEnviarCompartida a punto de enviar");

	send(socketPCP,stream,TAMANIO_INT+tamanio,0);			log_debug(logDebug,"stubEnviarCompartida esperando respuesta");

	recv(socketPCP,compartida,TAMANIO_INT,MSG_WAITALL);		log_debug(logDebug,"stubEnviarCompartida a punto de finalizar");

	close (socketPCP);

}
