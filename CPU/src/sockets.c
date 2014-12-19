
#include "headerCompleto.h"



void enviar(int socket, char* mensaje,int tamanio){
	int bytesEnviados;
	int bytesReenviados;


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

	memset(&criteriosConfiguracion,0,sizeof(criteriosConfiguracion)); //Inicializo todo en cero

	criteriosConfiguracion.ai_family=AF_UNSPEC;          //Cargo datos del socket
	criteriosConfiguracion.ai_socktype=SOCK_STREAM;

	getaddrinfo(ip,puerto, &criteriosConfiguracion,&datosSocketServidor);

	socketServidor = socket(datosSocketServidor->ai_family,datosSocketServidor->ai_socktype,datosSocketServidor->ai_protocol );

	connect(socketServidor, datosSocketServidor->ai_addr,datosSocketServidor->ai_addrlen);
	printf("conectado");
	freeaddrinfo(datosSocketServidor);

	return socketServidor;

}

int escuchar(const char* puerto){

	struct addrinfo criteriosConfiguracion;
			struct addrinfo* datosSocketEscucha;
			int socketEscucha;
			struct sockaddr_in datosSocketCliente;
			socklen_t largoDatosSocketCliente = sizeof(datosSocketCliente);

			memset(&criteriosConfiguracion,0,sizeof(criteriosConfiguracion)); //Inicializo todo en cero

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



