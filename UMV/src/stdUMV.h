/*
 * stdUMV.h
 *
 *  Created on: 06/05/2014
 *      Author: utnso
 */

#ifndef STDUMV_H_
#define STDUMV_H_

//Includes

#include <stdio.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/config.h>
#include "estructurasAdministrativas.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <pthread.h>
#include <sys/select.h>
#include <unistd.h>
#include "comunicaciones.h"
#include <commons/temporal.h>

//Funciones De Comparacion

int esMayor(void* nuevo, void* actual);
int posicionFisicaSegmentoEsMayor(void* actual, void* nuevo);

//Funciones agregadas a las commons

void list_add_order_by(t_list* list, void* data, int(*isBigger)(void*,void*));


//Prototipos De Funciones Internas

void milisleep(unsigned int milisegundos);
int posicionFirstFit(int tamanioSegmento);
int posicionWorstFit(int tamanioSegmento);
int escribirMemoria(int, int, int, int, void*);
void setAlgoritmo(int (*algoritmo)(int));
void setRetardo(long);
int compactarMemoria(void);
int compactarMemoria(void);
int leerMemoria(int, int, int, int,void**);
int crearSegmento(int PID, int tamanio);
int crearTablaProceso(int PID);
t_segmento* newSegmento(int baseVirtual, int tamanio, int ubicacionFisica);
bool (*segmentoComienzaEnBaseDada(int baseSegmento)) (t_segmento*);
void destruirSegmento (t_segmento*);
void buscarYDestruirProceso(t_tablaSegmentosProceso*);
void destruirTablaProceso(t_tablaSegmentosProceso*);
void destruirEstructurasEnMemoria(void);
void consola (void);
void administrarConexiones(void);
int destruirProceso(int PID);
int stringANumero(char*,long*);

//Prototipos De Funciones Para Comandos

int generarInformeSegun(char*);
int cambiarAlgoritmoSegun(char*);
int setRetardoEn(char*);
void escribirMemoriaPorTeclado(int, int, int, int);
void leerMemoriaPorTeclado(int proceso, int baseSegmento, int desplazamiento, int tamanioBloque);
int esElComando(char*,char*,char**);
int operarMemoriaSegun(char* ,void (*operacion)(int, int, int, int));
int getSegmentoActual(int, int ,t_segmento**);
void imprimirError(int error);
int dumpContenido(void*);
int dumpEstructurasDe(void*);
int dumpTodasLasEstructuras(void);
int dumpMemoriaCompleta(void);
int dumpSegun(void*);

//stubs
void stubCPUConexion(void);

//Variables Globales (definidas en main.c)

extern t_dictionary* tablasDeSegmentos;
extern t_list* segmentosEnMemoria;
extern char* memoriaPrincipal;
extern int (*algoritmoPosicionFisica)(int);
extern t_log* logDebug;//Debug
extern t_log* logConexiones;//Debug
extern int tamanioMemoriaPrincipal;
extern long retardo;
extern char* puertoUMV;
extern pthread_rwlock_t lockTablasDeSegmentos;
extern pthread_rwlock_t lockSegmentosEnMemoria;
extern pthread_rwlock_t lockDump;


//Macros

#define PROCESOINEXISTENTE -1
#define SINESPACIO -2
#define PROCESOYAEXISTE -5
#define SEGMENTOINEXISTENTE -3
#define SEGMENTATIONFAULT -4


#define FINAL '\0'
#define LARGOMAXIMOCOMANDO 25

#define CargarEstructurasAdministrativas 	tamanioMemoriaPrincipal = 1000;\
											memoriaPrincipal= malloc(tamanioMemoriaPrincipal);\
											retardo=0;\
											puertoUMV="6068";\
											tablasDeSegmentos = dictionary_create();\
											segmentosEnMemoria = list_create();\
											algoritmoPosicionFisica=posicionFirstFit;

#define LiberarEstructurasAdministrativas   destruirEstructurasEnMemoria();\
											free(memoriaPrincipal);\


#define AdministrarTests 	LiberarEstructurasAdministrativas;\
							runTests();\
							printf("Ejecutar tests interactivos? Y/N ");\
							char c = getchar();\
							getchar();\
							if ((c == 'Y')||(c == 'y'))\
							runInteractiveTests();\
							log_destroy(logDebug);\
							log_destroy(logConexiones);\
							return 0;\


#endif /* STDUMV_H_ */
