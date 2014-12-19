#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/string.h>
#include <parser/parser.h>
#include <commons/config.h>
#include <errno.h>
#include "kernel.h"


void enviarMensaje(int socket, char* mensaje,int tamanio){//DEPRECATED
	int bytesEnviados;
	int bytesReenviados;

	printf("Esta funcion esta deprecada, en su lugar debe usarse int enviar(...) que contempla errores en el envio\n");
	bytesEnviados=send(socket,mensaje,tamanio,0);

	while (bytesEnviados < tamanio) {

	 bytesReenviados=send(socket,mensaje+bytesEnviados,tamanio-bytesEnviados,0);
	 bytesEnviados += bytesReenviados;

	}

}


int conectar(const char* puerto,const char* ip){


	struct addrinfo criteriosConfiguracion;
	struct addrinfo* datosSocketServidor;
	int socketServidor;

	memset(&criteriosConfiguracion,0,sizeof(criteriosConfiguracion)); //Inicializo todoh en cero

	criteriosConfiguracion.ai_family=AF_UNSPEC;          //Cargo datos del socket
	criteriosConfiguracion.ai_socktype=SOCK_STREAM;

	getaddrinfo(ip,puerto, &criteriosConfiguracion,&datosSocketServidor);

	socketServidor = socket(datosSocketServidor->ai_family,datosSocketServidor->ai_socktype,datosSocketServidor->ai_protocol );

	int aux = connect(socketServidor, datosSocketServidor->ai_addr,datosSocketServidor->ai_addrlen);					if(aux<0)log_debug(logDebug,"Error al conectar: %d",errno);

	freeaddrinfo(datosSocketServidor);

	return socketServidor;

}

int escuchar(const char* puerto){

	struct addrinfo criteriosConfiguracion;
			struct addrinfo* datosSocketEscucha;
			int socketEscucha;
			struct sockaddr_in datosSocketCliente;
			socklen_t largoDatosSocketCliente = sizeof(datosSocketCliente);

			memset(&criteriosConfiguracion,0,sizeof(criteriosConfiguracion)); //Inicializo todoh en cero

			criteriosConfiguracion.ai_family=AF_UNSPEC;
			criteriosConfiguracion.ai_flags=AI_PASSIVE;			//Cargo datos del socket
			criteriosConfiguracion.ai_socktype=SOCK_STREAM;

			getaddrinfo(NULL,puerto, &criteriosConfiguracion,&datosSocketEscucha);

			socketEscucha=socket(datosSocketEscucha->ai_family,datosSocketEscucha->ai_socktype,datosSocketEscucha->ai_protocol);

			bind(socketEscucha, datosSocketEscucha->ai_addr, datosSocketEscucha->ai_addrlen);
			freeaddrinfo(datosSocketEscucha);

			listen(socketEscucha,1);

			printf("Esperando conexiones...\n");

			return accept(socketEscucha,(struct sockaddr*) &datosSocketCliente, &largoDatosSocketCliente);

}


