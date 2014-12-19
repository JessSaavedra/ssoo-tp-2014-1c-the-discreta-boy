/*
 * stubRecuperarPCB.c
 *
 *  Created on: 14/06/2014
 *      Author: utnso
 */

#include "kernel.h"

#define IPLOCAL "127.0.0.1"

void stubImprimirTextoPP(char* rta){

	struct addrinfo criteriosConfiguracion;
	struct addrinfo* datosSocketServidor;
	int socketPCP;

	memset(&criteriosConfiguracion,0,sizeof(criteriosConfiguracion)); //Inicializo  en cero

	criteriosConfiguracion.ai_family=AF_UNSPEC;          //Cargo datos del socket
	criteriosConfiguracion.ai_socktype=SOCK_STREAM;

	getaddrinfo(IPLOCAL,"7020", &criteriosConfiguracion,&datosSocketServidor);

	socketPCP = socket(datosSocketServidor->ai_family,datosSocketServidor->ai_socktype,datosSocketServidor->ai_protocol );

	connect(socketPCP, datosSocketServidor->ai_addr,datosSocketServidor->ai_addrlen);log_debug(logDebug,"stubImprimirTextoPP se conecto");
	freeaddrinfo(datosSocketServidor);
	//-------------------------------------------------------------------------------------------------//

int  tamanio;
													log_debug(logDebug,"stubImprimirTextoPP a punto de recibir");
	recv(socketPCP,rta,TAMANIO_INT,0);					log_debug(logDebug,"stubImprimirTextoPP recibio cop %d",*rta);
	recv(socketPCP,&tamanio,TAMANIO_INT,0);
	recv(socketPCP,rta+TAMANIO_INT,tamanio,0);
														log_debug(logDebug,"stubImprimirTextoPP a punto de finalizar");

	close (socketPCP);

}
