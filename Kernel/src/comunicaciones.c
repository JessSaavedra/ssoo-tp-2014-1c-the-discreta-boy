/*
 * comunicaciones.c
 *
 *  Created on: 24/06/2014
 *      Author: utnso
 */

#include "comunicaciones.h"

t_cliente* newCliente(int socket){

	t_cliente* cliente = malloc(sizeof(t_cliente));

	cliente->socket = socket;
	cliente->estaVivo = true;

	return cliente;
}


int enviar(int socket, void* mensaje,size_t tamanio){
	int bytesEnviados;
	int bytesReenviados;

	bytesEnviados=bytesReenviados=send(socket,mensaje,tamanio,0);

	while ((bytesReenviados>0) && (bytesEnviados < tamanio)) {
			bytesReenviados=send(socket,mensaje+bytesEnviados,tamanio-bytesEnviados,0);
			bytesEnviados += bytesReenviados;
		}
	return bytesReenviados==-1?-1:bytesEnviados;
}

int crearSocketEscuchaSobre(char* puerto,int cantidadEsperas){

	struct addrinfo criteriosConfiguracion;
	struct addrinfo* datosSocketEscucha;
	int socketEscucha;

	memset(&criteriosConfiguracion,0,sizeof(criteriosConfiguracion)); //Inicializo tutto en cero

	criteriosConfiguracion.ai_family=AF_UNSPEC;
	criteriosConfiguracion.ai_flags=AI_PASSIVE;			//Cargo datos del socket
	criteriosConfiguracion.ai_socktype=SOCK_STREAM;

	getaddrinfo(NULL,puerto, &criteriosConfiguracion,&datosSocketEscucha);

	socketEscucha=socket(datosSocketEscucha->ai_family,datosSocketEscucha->ai_socktype,datosSocketEscucha->ai_protocol);
	int optval = 1;
	setsockopt(socketEscucha, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

	bind(socketEscucha, datosSocketEscucha->ai_addr, datosSocketEscucha->ai_addrlen);
	freeaddrinfo(datosSocketEscucha);

	listen(socketEscucha,cantidadEsperas);

	return socketEscucha;
}
