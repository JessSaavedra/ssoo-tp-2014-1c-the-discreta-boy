/*
 * stubCPU.c
 *
 *  Created on: 27/05/2014
 *      Author: utnso
 */


#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/log.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define IPLOCAL "127.0.0.1"

extern char* leidoPorCPU;
extern int sinErrorE,sinErrorL,procesoInexistenteE,segmentoInexistenteE,segmentationFaultE,procesoInexistenteL,segmentoInexistenteL,segmentationFaultL;
t_log *logConexiones;

void trabajarConUMV(int pid, int baseLectura,int baseEscritura, int offset, int tamanio, int socketUMV, int* errorL, int* errorE){

	char* mensaje=malloc(50);
	int error,segmento,cop;

	log_trace(logConexiones,"CPU preparandose para setear el proceso a %d",pid);

	cop=2;

	memcpy(mensaje,&cop,sizeof(cop));
	memcpy(mensaje+sizeof(cop),&pid,sizeof(pid));

	log_trace(logConexiones,"CPU a punto de setear el proceso a %d",pid);

	send(socketUMV,mensaje,sizeof(cop)+sizeof(pid),0);

	recv(socketUMV,&error,4,MSG_WAITALL);
	log_trace(logConexiones,"CPU recibio el codigo de error: %d",error);

	log_trace(logConexiones,"CPU preparandose para escribir caca");

	cop=1;
	segmento=baseEscritura;

	memcpy(mensaje,&cop,sizeof(cop));
	memcpy(mensaje+sizeof(cop),&segmento,sizeof(segmento));
	memcpy(mensaje+sizeof(cop)+sizeof(segmento),&offset,sizeof(offset));
	memcpy(mensaje+sizeof(cop)+sizeof(segmento)+sizeof(offset),&tamanio,sizeof(tamanio));
	memcpy(mensaje+sizeof(cop)+sizeof(segmento)+sizeof(offset)+sizeof(tamanio),"caca",strlen("caca")+1);

	log_trace(logConexiones,"CPU a punto de escribir en 20-%d-%d-%d-%s",mensaje[sizeof(cop)],mensaje[sizeof(cop)+sizeof(segmento)]\
			,mensaje[sizeof(cop)+sizeof(segmento)+sizeof(offset)],mensaje+sizeof(cop)+sizeof(segmento)+sizeof(offset)+sizeof(tamanio));

	send(socketUMV,mensaje,sizeof(cop)+sizeof(segmento)+sizeof(offset)+sizeof(tamanio)+strlen("caca")+1,0);


	recv(socketUMV,&error,4,MSG_WAITALL);
	log_trace(logConexiones," CPU recibio el codigo de error: %d",error);
	*errorE=error;

	cop=0;
	segmento=baseLectura;

	memcpy(mensaje,&cop,sizeof(cop));
	memcpy(mensaje+sizeof(cop),&segmento,sizeof(segmento));
	memcpy(mensaje+sizeof(cop)+sizeof(segmento),&offset,sizeof(offset));
	memcpy(mensaje+sizeof(cop)+sizeof(segmento)+sizeof(offset),&tamanio,sizeof(tamanio));

	log_trace(logConexiones,"CPU a punto de leer",mensaje[sizeof(cop)],mensaje[sizeof(cop)+sizeof(segmento)]\
				,mensaje[sizeof(cop)+sizeof(segmento)+sizeof(offset)]);

	send(socketUMV,mensaje,sizeof(cop)+sizeof(segmento)+sizeof(offset)+sizeof(tamanio),0);

	recv(socketUMV,&error,4,MSG_WAITALL);
	log_trace(logConexiones,"CPU recibio el codigo de error: %d",error);
	*errorL=error;
	if(!error){
		recv(socketUMV,mensaje,tamanio,MSG_WAITALL);
		log_trace(logConexiones,"CPU recibio datos: %s",mensaje);
		leidoPorCPU=malloc(tamanio);
		memcpy(leidoPorCPU,mensaje,tamanio);
	}
}

int stubCPU(int* argv){ //EAGAIN;EWOULDBLOCK;EBADF;ECONNREFUSED;EFAULT;EINTR;EINVAL;ENOMEM;ENOTCONN;ENOTSOCK;

	logConexiones= log_create("/home/utnso/git/tp-2014-1c-the-discreta-boy/UMV/Debug/testConexionesUMV.txt","UMV",false,LOG_LEVEL_TRACE);
	struct addrinfo criteriosConfiguracion;
	struct addrinfo* datosSocketServidor;
	int socketUMV;
	int baseLectura=argv[0],baseEscritura=argv[1];

	memset(&criteriosConfiguracion,0,sizeof(criteriosConfiguracion)); //Inicializo  en cero

	criteriosConfiguracion.ai_family=AF_UNSPEC;          //Cargo datos del socket
	criteriosConfiguracion.ai_socktype=SOCK_STREAM;

	getaddrinfo(IPLOCAL,"6068", &criteriosConfiguracion,&datosSocketServidor);

	socketUMV = socket(datosSocketServidor->ai_family,datosSocketServidor->ai_socktype,datosSocketServidor->ai_protocol );

	connect(socketUMV, datosSocketServidor->ai_addr,datosSocketServidor->ai_addrlen);
	freeaddrinfo(datosSocketServidor);

	log_trace(logConexiones,"CPU se conecto a la UMV");

	int ID=11;

	log_trace(logConexiones,"CPU a punto de enviar id=%d",ID);

	send(socketUMV,&ID,sizeof(ID),0);log_trace(logConexiones,"CPU envio id=%d",ID);

	log_trace(logConexiones,"CPU INTENTARA COMUNICACION NORMAL");
	trabajarConUMV(20,baseLectura,baseEscritura,0,5,socketUMV,&sinErrorL,&sinErrorE);
	log_trace(logConexiones,"CPU INTENTARA FALLAR POR PROCESO INEXISTENTE");
	trabajarConUMV(25,baseLectura,baseEscritura,0,5,socketUMV,&procesoInexistenteL,&procesoInexistenteE);
	log_trace(logConexiones,"CPU INTENTARA FALLAR POR SEGMENTO INEXISTENTE");
	trabajarConUMV(20,baseLectura+5,baseEscritura+5,0,5,socketUMV,&segmentoInexistenteL,&segmentoInexistenteE);
	log_trace(logConexiones,"CPU INTENTARA FALLAR POR SEGMENTATION FAULT");
	trabajarConUMV(20,baseLectura,baseEscritura,9,5,socketUMV,&segmentationFaultL,&segmentationFaultE);

	close(socketUMV);

	return 0;
}
