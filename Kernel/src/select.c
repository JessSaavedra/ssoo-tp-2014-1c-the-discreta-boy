/*
 * select.c
 *
 *  Created on: 05/06/2014
 *      Author: utnso
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <commons/collections/list.h>
#include "kernel.h"
#include <sys/select.h>

#define maximo(A,B) A>B?A:B

static int maximoFileDescriptor;
static fd_set setFileDescriptors;

static void agregarEnFdSet(t_cliente* cliente){
		FD_SET(cliente->socket,&setFileDescriptors);
		maximoFileDescriptor = maximo(maximoFileDescriptor,cliente->socket);
}

static bool estaEnElSet(t_cliente* cliente){
	return FD_ISSET(cliente->socket,&setFileDescriptors);
}

t_list* seleccionarListosLectura(t_list* clientesDados){

	t_list* listos;

	FD_ZERO(&setFileDescriptors);
	maximoFileDescriptor=0;
	list_iterate(clientesDados,(void(*)(void*))agregarEnFdSet);
	select(maximoFileDescriptor+1,&setFileDescriptors,NULL, NULL, NULL);log_debug(logDebug,"Error en el select: %d",errno);
	listos= list_filter(clientesDados,(bool(*)(void*))estaEnElSet);
	return listos;
}

