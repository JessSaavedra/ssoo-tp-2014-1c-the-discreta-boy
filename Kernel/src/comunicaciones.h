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

extern t_log* logDebug;

typedef struct cliente{
	int socket;
	bool estaVivo;
} t_cliente;

#include "kernel.h"

#define enviarACliente(cliente,datos,tamanio) if(cliente->estaVivo?(0>=enviar(cliente->socket,datos,tamanio)):true){\
													log_debug(logDebug,"Error en el envio al cliente");\
													cliente->estaVivo=false;\
													return CPUMURIO;\
												}
#define recibirDeCliente(cliente,datos,tamanio) if(cliente->estaVivo?(0>=recv(cliente->socket,datos,tamanio,MSG_WAITALL)):true){\
													log_debug(logDebug,"Error en la recepcion/conexion cerrada con el cliente");\
													cliente->estaVivo=false;\
													return CPUMURIO;\
												}

t_cliente* newCliente(int);
int enviar(int socket, void* ,size_t );
int crearSocketEscuchaSobre(char* ,int );

#endif /* COMUNICACIONES_H_ */
