/*
 * errores.c
 *
 *  Created on: 29/06/2014
 *      Author: ricardo
 */
#include "headerCompleto.h"

int validarCodigoDeError(int socket){
	int codigoDeError;

	recv(socket,&codigoDeError,tamanioInt,MSG_WAITALL);

	if(codigoDeError < 0){
		if (socket==socketUMV){
			char* informarError=malloc(tamanioInt);
			memcpy(informarError,&codigoDeError,tamanioInt);
			enviar(socketKernel,informarError,tamanioInt); log_debug(logDebug,"Informé error con valor %d",codigoDeError);
			free(informarError);
		}

		flagError = -1;
	}

	return codigoDeError;
}

void validarStackOverflow(){
	int codigoDeError;

	recv(socketUMV,&codigoDeError,tamanioInt,MSG_WAITALL);

	if(codigoDeError < 0){
		flagError = -1;
		char* informarError=malloc(tamanioInt);
		if (codigoDeError==-4){
			codigoDeError=-20;
			memcpy(informarError,&codigoDeError,tamanioInt);
				}
		else {
			memcpy(informarError,&codigoDeError,tamanioInt);

		}

		enviar(socketKernel,informarError,tamanioInt); log_debug(logDebug,"Informé error con valor %d id: %d",codigoDeError,PCB.identificador);
		free(informarError);
	}


}
