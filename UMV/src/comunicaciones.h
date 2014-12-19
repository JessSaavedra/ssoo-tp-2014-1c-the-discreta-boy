/*
 * comunicaciones.h
 *
 *  Created on: 24/06/2014
 *      Author: utnso
 */

#ifndef COMUNICACIONES_H_
#define COMUNICACIONES_H_

#include <stdbool.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct cliente{
	int socket;
	int (*operacionEnvio)(int,void*,size_t);
	int (*operacionRecepcion) (int,void*,size_t,int);
	bool estaVivo;
} t_cliente;

int simularEnvio(int , void*, size_t );
int simularRecepcion(int , void* , size_t ,int );
int enviarACliente(t_cliente* ,void* ,size_t );
int recibirDeCliente(t_cliente* ,void* ,size_t );
t_cliente* newCliente(int , int (*)(int,void*,size_t),int (*)(int,void*,size_t,int),bool );
int enviar(int socket, void* ,size_t );
int crearSocketEscuchaSobre(char* ,int );

#endif /* COMUNICACIONES_H_ */
