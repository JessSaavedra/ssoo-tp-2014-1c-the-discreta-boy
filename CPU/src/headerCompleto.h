/*
 * headerCompleto.h
 *
 *  Created on: 29/05/2014
 *      Author: utnso
 */

#ifndef HEADERCOMPLETO_H_
#define HEADERCOMPLETO_H_

//Includes

#include <parser/parser.h>
#include <parser/metadata_program.h>
#include <commons/string.h>
#include <stdio.h>
#include <commons/log.h>
#include <string.h>
#include <stdint.h>
#include <commons/collections/dictionary.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/config.h>
#include <signal.h>
#include <errno.h>

//Typedefs

typedef struct {
		int identificador;
		int segmentoDeCodigo;
		int segmentoDeStack;
		int cursorDelStack;
		int indiceDeCodigo;
		int tamanioIndiceEtiquetas;
		int indiceEtiquetas;
		int programCounter;
		int tamanioContextoActual;
} tipoPCB;

struct datosLecturaUMV {
	int codOperacion;
	int segmento;
	int offset;
	int longitud;
}paquetePedidoLinea;

struct datosEscrituraUMV {
		int codOperacion;
		int segmento;
		int offset;
		int longitud;
		char* datosAGuardar;
	} ;

//Variables globales

extern tipoPCB PCB;
extern t_dictionary* diccionarioDeVariables;
extern char* indiceDeEtiquetas;
extern int socketKernel;
extern int socketUMV;
extern int flagError;
extern t_size tamanioInt;
extern void* stack;
extern t_log* logDebug;
extern int finDePrograma;
extern int desconexionCPU;
extern int flagBloqueo;

//Prototipos

void enviar(int socket, char* mensaje,int tamanio);

int conectar(const char* puerto,const char* IP);

int escuchar(const char* puerto);

int definirVariable(char identificador_variable);

int obtenerPosicionVariable(char identificador_variable);

int dereferenciar(int direccion_variable);

void asignar(int direccion_variable, int valor );

int obtenerValorCompartida(char* variable);

int asignarValorCompartida(char* variable, int valor);

void irAlLabel(char* etiqueta);

void llamarSinRetorno(char* etiqueta);

void llamarConRetorno(char* etiqueta, int donde_retornar);

void finalizar(void);

void retornar(int retorno);

void imprimir(int valor_mostrar);

void imprimirTexto(char* texto);

void wait(char* identificador_semaforo);

void signalCPU(char* identificador_semaforo);

char* recibirPCB();

char* generarStreamEscritura(int segmento,int offset,int longitud,void* datosAGuardar);

char* generarStreamLectura(int segmento,int offset,int longitud);

int validarCodigoDeError(int socket);

void escribirEnMemoria(char* contenidoAGuardar,int tamanioAEnviar);

void escribirEnMemoria2(char* contenidoAGuardar,int tamanioAEnviar);

void* pedirDeMemoria(char* pedido,int tamanio);

void* pedirDeMemoria2(char* pedido,int tamanio);

void recuperarContextoActual();

void avisarAlKernel(int codigoOperacion,char* identificador_semaforo);

void entradaSalida(char* dispositivo, int tiempo);

void enviarPCBActualizado(tipoPCB PCB,int codigoDeOperacion);

char* agregarBarraCero(char* etiqueta);

void desconectarCPU(int signal);

void milisleep(unsigned int milisegundos);

void validarStackOverflow();

char* generarPCB();

char* pedirDeMemoriaContexto(char* pedido,int tamanioARecibir);

void escribirEnStack(char* contenidoAGuardar,int tamanioAEnviar);

//Macros

#define INICIALIZARTESTS 	PCB.cursorDelStack = 5;\
							PCB.tamanioContextoActual=0;\
							stack=malloc(100);\
							tamanioInt=sizeof(int);\
							indiceDeEtiquetas=dictionary_create();\
							diccionarioDeVariables=dictionary_create();

#define FINALIZARTEST		dictionary_destroy(diccionarioDeVariables);\
							dictionary_destroy(indiceDeEtiquetas);\
							free(stack);



#endif /* HEADERCOMPLETO_H_ */
