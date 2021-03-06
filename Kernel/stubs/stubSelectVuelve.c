/*
 * stubRecuperarPCB.c
 *
 *  Created on: 14/06/2014
 *      Author: utnso
 */

#include "kernel.h"

#define IPLOCAL "127.0.0.1"

void stubSelectVuelve(sem_t* sem){

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

	tipoPCB* PCB;
	PCB = newPCB(0,1,2,3,4,5,6,7,8);

	char* streamPCB = serializarPCB(*PCB);

	log_debug(logDebug,"stubSelectVuelve a punto de enviar");

	send(socketPCP,streamPCB,10*TAMANIO_INT,0);
	sem_post(sem);

	log_debug(logDebug,"stubSelectVuelve finalizo");

	close(socketPCP);
}
