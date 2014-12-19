/*
 * stubRecuperarPCB.c
 *
 *  Created on: 14/06/2014
 *      Author: utnso
 */

#include "kernel.h"

#define IPLOCAL "127.0.0.1"

void stubSelectNoVuelve(void){

	struct addrinfo criteriosConfiguracion;
	struct addrinfo* datosSocketServidor;
	int socketPCP;

	memset(&criteriosConfiguracion,0,sizeof(criteriosConfiguracion)); //Inicializo  en cero

	criteriosConfiguracion.ai_family=AF_UNSPEC;          //Cargo datos del socket
	criteriosConfiguracion.ai_socktype=SOCK_STREAM;

	getaddrinfo(IPLOCAL,"7009", &criteriosConfiguracion,&datosSocketServidor);

	socketPCP = socket(datosSocketServidor->ai_family,datosSocketServidor->ai_socktype,datosSocketServidor->ai_protocol );

	connect(socketPCP, datosSocketServidor->ai_addr,datosSocketServidor->ai_addrlen);
	freeaddrinfo(datosSocketServidor);
	//-------------------------------------------------------------------------------------------------//
	int finalizar;

	log_debug(logDebug,"stubSelectNoVuelve a punto de recibir");

	int a=recv(socketPCP,&finalizar,TAMANIO_INT,0);

	log_debug(logDebug,"%d %d",a,errno);

	log_debug(logDebug,"stubSelectNoVuelve finalizo");

	close(socketPCP);
}
