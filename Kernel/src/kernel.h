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
#include <commons/string.h>
#include "comunicaciones.h"

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

typedef struct {
	t_queue* bloqueados;
	int contador;
}t_semaforo;

typedef struct {
	pthread_t hiloIO;
	int valorDeRetardo;
	t_colaSincronizada* colaDeIO;
}tipoDispositivo;

typedef struct {
	tipoProceso* proceso;
	int cantidadUnidadesDeIO;
}tipoProcesoEnIO;

//Variables Globales

extern t_colaSincronizada *colaCPUsListos,*colaCPUsEnUso,*colaProcesosListos,*colaProcesosFinalizados,*colaProcesosNuevos,*colaProcesosEnEjecucion;
extern int quantum;
extern int socketUMV;
extern int tamanioStack;
extern int retardo;
extern char *puertoPP,*puertoCPU,*puertoUMV,*IPUMV;
extern t_dictionary *variablesGlobales,*procesosEnEjecucion,*semaforos,*dispositivosIO;
extern t_log* logDebug;
extern sem_t* gradoDeMultiprogramacion;
extern int segmentosCreadosFlag;
extern pthread_mutex_t lockProcesosEnEjecucion;
extern sem_t* procesosNuevos;

//Prototipos

int configuracionesValidas(char* archivo);
int cargarConfiguraciones(char* archivo);

void ejecutarIO(tipoDispositivo*);
void funcionIO(t_dictionary*);
void deshacerRetirar(t_colaSincronizada* cola,void* elemento);
void reencolarCPUsEnUso(t_list*);
void escucharPedidosCPU(void);
void encolar(t_colaSincronizada* ,void*);
void* retirar(t_colaSincronizada*);
t_list* seleccionarListosLectura(t_list* socketsDados);
void despacharProcesos(void);
t_list* list_complement(t_list* original, t_list* sublist);
char* generarStreamCreacionSegmento(int pid,int tamanioSegmento);
tipoProceso* crearSegmentosDeProceso(int pid,char* codigo,t_medatada_program* metadata,int tamanioCodigo,tipoProceso* proceso);
void imprimirProceso(void* proceso);
void imprimirProcesos(t_colaSincronizada* colaProcesosNuevos);
t_list* desencolarCPUsEnUso(void);
void atenderCPU(t_cliente*);
void atenderPedidosCPUs(t_list* );
void grabarEnStream(char**, int);
char* serializarPCB(tipoPCB);
char* generarStreamEscritura(int segmento,int longitud,void* datosAGuardar);
void recibirConexionesCPUs(int* escuchador);
int crearSocketEscuchaSobre(char*,int );
t_colaSincronizada* newColaSincronizada(void);
int enviarCompartida(int*,t_cliente*);
int escribirCompartida(int*,t_cliente*);
int imprimir(t_cliente* CPU);
int imprimirTexto(t_cliente* CPU);
int recuperarProcesoListo(t_cliente* );
void list_add_order_by(t_list* list, void* data, int(*isBigger)(void*,void*));
int esDeMenorPeso(void* proceso1 , void* proceso2);
void encolar_order_by (t_colaSincronizada* cola,void* data, int(*isBigger)(void*,void*));
int escuchar(const char* puerto);
int conectar(const char* puerto,const char* ip);
void organizadorDeProgramas(void);
void manejarExcepcion(int,t_cliente*);
tipoPCB* newPCB(int, int, int, int, int, int, int, int, int);
int manejarSyscall(int ,t_cliente* );
void terminarProceso(tipoProceso* proceso);
void planificadorDeProgramasListos(void);
void planificadorDeProgramasEliminados(void);
void notificar(tipoProceso* proceso, char* mensaje);
void crearProceso(int pid);
void validarCodigoDeError();
int wait(t_semaforo*,t_cliente*);
int post(t_semaforo*,t_cliente*);
int operarElementoCompartido(t_cliente*, int(*)(void*,t_cliente*),int,t_dictionary*);
int operarCompartida(t_cliente*, int (*)(int*,t_cliente*));
int recuperarProcesoFinalizado(t_cliente*);
int recuperarProcesoDeCPUSaliente(t_cliente*);
int __recibirProceso(t_cliente*,tipoProceso**);
void milisleep(unsigned int );
void crearHilosIO(char* key,tipoDispositivo* dispositivo);
void ejecutarIO(tipoDispositivo* dispositivo);
int realizarIO(tipoDispositivo* , t_cliente*);
int recibirPendientes(int codigoExcepcion,t_cliente* CPU);
void pcp(void);
void plp(void);
int tamanioCola(t_colaSincronizada* cola);

//Stubs
void stubRecuperarPCB(void);
void stubSelectNoVuelve(void);
void stubSelectVuelve(void);
void stubDespachar(tipoProceso*);
void stubEscucharPedidosNoVuelve(void);
void stubEscucharPedidosVuelve(void);
void stubEnviarCompartida(int*);
void stubEscribirCompartida(int*);
void stubEnviarCompartidaFalla(int*);
void stubEscribirCompartidaInexistenteFalla(int*);
void stubImprimirCPU(int*);
void stubImprimirPP(char*);
void stubImprimirTextoCPU(char*);
void stubImprimirTextoPP(char*);

//Defines

#define TAMANIO_STREAM_PCB 9*sizeof(int)
#define TAMANIO_INT sizeof(int)
//Excepciones
#define CPUMURIO -10
#define ESCRIBIR_COMPARTIDAINEXISTENTE -11
#define LEER_COMPARTIDAINEXISTENTE -12
#define SEMAFOROINEXISTENTE -13
#define IOINEXISTENTE -14
#define MEMORIALLENA -15
#define SEGMENTATIONFAULT -4
#define STACKOVERFLOW -20
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
#define CPUSEDESCONECTA 19

#define CrearEstructurasGlobales colaCPUsEnUso = newColaSincronizada();\
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

#define AdministrarInicio if(argc==2){if (string_equals_ignore_case(argv[1],"testOnly")){\
								runTests();\
								return 0;\
							}\
							else if (configuracionesValidas(argv[1])){\
								if((cargarConfiguraciones(argv[1]))<0)	log_debug(logDebug,"configuraciones o parametro invalido, finalizando Kernel");\
							}}\
							else{\
								printf("Configuraciones o parametros invalidos - \n - A la Mirtha le gusta le le den - Julio Cortazar\n");\
								log_debug(logDebug,"configuraciones o parametro invalido, finalizando Kernel");\
							}

#define recibirProceso(CPU,proceso) if(0<__recibirProceso(CPU,proceso)) return CPUMURIO;

#define retirarProcesoDeEjecucion(CPU)	char* cpu=string_itoa(CPU->socket);\
										pthread_mutex_lock(&lockProcesosEnEjecucion);\
										dictionary_remove(procesosEnEjecucion,cpu);\
										pthread_mutex_unlock(&lockProcesosEnEjecucion);\
										free(cpu);

#define recuperarProceso(CPU,proceso) 	recibirProceso(CPU,proceso)\
										retirarProcesoDeEjecucion(CPU)

#endif /* KERNEL_H_ */
