/*
 * stdPP.h
 *
 *  Created on: 29/05/2014
 *      Author: utnso
 */

#ifndef STDPP_H_
#define STDPP_H_

#include <stdio.h>
#include <string.h>
#include <commons/string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/config.h>
#include <commons/log.h>
#include <errno.h>

//Errores
#define FALLOEJECUCION -1
#define CONFIG_INVALIDA -2
#define FALLOENLECTURA -3

//CodigosRecibidos
#define IMPRIMIR 0
#define IMPRIMIRTEXTO 1
#define FINALIZAR 2


typedef struct {
	int codigoOperacion;
	int tamanioTransmitido;
}t_header;

void enviar(int,char*,int);
int cargarConfiguraciones(void);
void liberarConfiguraciones(void);
int newSocketServidor(void);
int enviarScript(char*);
void* comandoPara(int);
void recibirHeader(t_header*);
void recibirDatosCon(int,void*);
void imprimir(void*);
void imprimirTexto(void*);
int runTests(void);

extern t_log* logDebug;
extern struct addrinfo criteriosConfiguracion;
extern struct addrinfo* datosSocketKernel;
extern int socketKernel;
extern char* puerto;
extern char* ip;

#endif /* STDPP_H_ */
