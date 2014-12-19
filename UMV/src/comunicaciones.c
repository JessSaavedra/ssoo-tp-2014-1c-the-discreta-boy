/*
 * comunicaciones.c
 *
 *  Created on: 24/06/2014
 *      Author: utnso
 */

#include "comunicaciones.h"

extern t_log* logDebug;//Debug

int simularEnvio(int socket, void* datos, size_t tamanio){log_debug(logDebug,"Se simula un envio en lugar del envio real debido a algun error");
	return 0; //TODO loguear
}

int simularRecepcion(int socket, void* datos, size_t tamanio,int flags){log_debug(logDebug,"Se simula una recepcion en lugar de la recepcion real debido a algun error");
	return 0;
}

int enviarACliente(t_cliente* cliente,void* datos,size_t tamanio){

	int error = cliente->operacionEnvio(cliente->socket,datos,tamanio);

	if (error<0){														log_debug(logDebug,"Hubo un error al enviar, errno: %d",errno);
		cliente->estaVivo = false;
		cliente->operacionEnvio =  simularEnvio;
		cliente->operacionRecepcion =  simularRecepcion;
	}
	return error;
}

int recibirDeCliente(t_cliente* cliente,void* datos,size_t tamanio){

	int error = cliente->operacionRecepcion(cliente->socket,datos,tamanio,MSG_WAITALL);

	if (error<=0){														log_debug(logDebug,"Hubo un error al recibir o el cliente se cerro, errno: %d",errno);
		cliente->estaVivo = false;
		cliente->operacionEnvio = simularEnvio;
		cliente->operacionRecepcion =  simularRecepcion;
	}

	return error;

}

t_cliente* newCliente(int socket, int (*operacionEnvio)(int,void*,size_t),int (*operacionRecepcion)(int,void*,size_t,int),bool estaVivo){

	t_cliente* cliente = malloc(sizeof(t_cliente));

	cliente->socket = socket;
	cliente->operacionEnvio = operacionEnvio;
	cliente->operacionRecepcion = operacionRecepcion;
	cliente->estaVivo = estaVivo;

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

	memset(&criteriosConfiguracion,0,sizeof(criteriosConfiguracion)); //Inicializo todo en cero

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
