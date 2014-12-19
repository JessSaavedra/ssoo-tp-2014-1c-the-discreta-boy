/*
 * kernel.h
 *
 *  Created on: 02/06/2014
 *      Author: utnso
 */

#ifndef KERNEL_H_
#define KERNEL_H_

//Includes

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <stdbool.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <parser/parser.h>
#include <parser/metadata_program.h>
#include <commons/log.h>
#include <netdb.h>
#include <unistd.h>
#include <semaphore.h>
#include <commons/log.h>
#include <errno.h>

//Tipos

typedef struct {
		int identificador;
		int segmentoDeCodigo;
		int segmentoDeStack;
		int cursorDelStack;
		int indiceDeCodigo;
		int indiceEtiquetas;
		int programCounter;
		int tamanioContextoActual;
		int tamanioIndiceEtiquetas;
} tipoPCB;

typedef struct {
	tipoPCB PCB;
	int socketPP;
	int peso;
}tipoProceso;

typedef struct {
	t_list* cola;
	sem_t* semaforo;
	pthread_mutex_t* mutex;
} t_colaSincronizada;

//Variables Globales

extern t_colaSincronizada *colaCPUsListos,*colaCPUsEnUso,*colaProcesosListos,*colaProcesosFinalizados,*colaProcesosNuevos,*colaProcesosEnEjecucion;
extern int quantum;
extern int socketUMV;
extern int tamanioStack;
extern t_dictionary *variablesGlobales,*procesosEnEjecucion;
extern t_log* logDebug;

//Prototipos


void reencolarCPUsEnUso(t_list*);
void escucharPedidosCPU(void);
void encolar(t_colaSincronizada* ,void*);
void* retirar(t_colaSincronizada*);
t_list* seleccionarListosLectura(t_list* socketsDados);
void despacharProcesos(void);
t_list* list_complement(t_list* original, t_list* sublist);
char* generarStreamCreacionSegmento(int pid,int tamanioSegmento);
tipoProceso* crearSegmentosDeProceso(int pid,char* codigo,t_medatada_program* metadata,int tamanioCodigo,tipoProceso* proceso);
void imprimirProceso(tipoProceso* proceso);
void imprimirProcesos(t_colaSincronizada* colaProcesosNuevos);
t_list* desencolarCPUsEnUso(void);
void atenderCPU(int*);
void atenderPedidosCPUs(t_list* );
void grabarEnStream(char**, int);
char* serializarPCB(tipoPCB);
void enviar(int socket, char* mensaje,int tamanio);
char* generarStreamEscritura(int segmento,int longitud,void* datosAGuardar);
void recibirConexionesCPUs(int* escuchador);
int crearSocketEscuchaSobre(char*,int );
t_colaSincronizada* newColaSincronizada(void);
int* newCPU(int socket);
void enviarCompartida(int* CPU);
void escribirCompartida(int* CPU);
void imprimirGenerico(int* CPU,int codigo, int tamanioDatos);
void imprimir(int* CPU);
void imprimirTexto(int* CPU);
void recuperarProcesoListo(int* CPU);
tipoPCB* newPCB(int, int, int, int, int, int, int, int, int);

//Stubs
void stubRecuperarPCB(void);

//Defines

#define TAMANIO_STREAM_PCB 9*sizeof(int)
#define SEGUIREJECUTANDO 0
#define TEMRINAREJECUCION 1
#define TAMANIO_INT sizeof(int)
//Excepciones
#define CPUMURIO -10
#define COMPARTIDAINEXISTENTE -11
//Syscalls
#define QUANTUMFINALIZADO 10
#define OBTENERCOMPARTIDA 11
#define GRABARCOMPARTIDA 12
#define WAIT 13
#define SIGNAL 14
#define IO 15
#define IMPRIMIR 16
#define IMPRIMIRTEXTO 17
#define PROGRAMATERMINADO 18

#define CrearEstructurasGlobales logDebug = log_create("logDebugKernel.txt","Kernel",false,LOG_LEVEL_DEBUG);\
								 colaCPUsEnUso = newColaSincronizada();\
								 colaCPUsListos = newColaSincronizada();\
								 colaProcesosFinalizados = newColaSincronizada();\
								 colaProcesosListos = newColaSincronizada();\
								 colaProcesosNuevos = newColaSincronizada();\
								 procesosEnEjecucion = dictionary_create();\
								 variablesGlobales = dictionary_create();

#define LiberarEstructurasGlobales /*logDebug = log_destroy();\
		 TODO colaCPUsEnUso = newColaSincronizada();
		 colaCPUsListos = newColaSincronizada();
		 colaProcesosFinalizados = newColaSincronizada();
		 colaProcesosListos = newColaSincronizada();
		 colaProcesosNuevos = newColaSincronizada();
		 variablesGlobales = dictionary_create();*/

#define AdministrarTests if(argc>1){\
							if (string_equals_ignore_case(argv[1],"runTests")){\
								runTests();\
							}\
							else if (string_equals_ignore_case(argv[1],"testOnly")){\
								runTests();\
								return 0;\
							}\
						}

#endif /* KERNEL_H_ */
