/*
 * main.c
 *
 *  Created on: 02/05/2014
 *      Author: utnso
 */

#include "estructurasAdministrativas.h"
#include "stdUMV.h"


//Variables globales

t_log* logDebug,*logConexiones;//Debug
t_dictionary* tablasDeSegmentos;
pthread_rwlock_t lockTablasDeSegmentos = PTHREAD_RWLOCK_INITIALIZER;
t_list* segmentosEnMemoria;
pthread_rwlock_t lockSegmentosEnMemoria = PTHREAD_RWLOCK_INITIALIZER;
char* memoriaPrincipal;
pthread_rwlock_t lockDump = PTHREAD_RWLOCK_INITIALIZER;
int (*algoritmoPosicionFisica)(int);
int tamanioMemoriaPrincipal;
long retardo=0;
char* puertoUMV;

//Prototipos

void consola(void);
int runTests(void);
int runInteractiveTests(void);
void administrarConexiones(void);
void cargarConfiguraciones(char*);
int configuracionesValidas(char*);


void cerrarHilos(int n){

}

int main(int argc, char* argv[]){

	pthread_t hiloConsola, hiloConexiones;
	logDebug= log_create("debugUMV.txt","UMV",false,LOG_LEVEL_DEBUG);
	logConexiones= log_create("testConexionesUMV.txt","UMV",false,LOG_LEVEL_TRACE);
	tablasDeSegmentos = dictionary_create();
	segmentosEnMemoria = list_create();

	if(argc!=2){
			fprintf(stderr,"\nError en Parametros, la UMV requiere uno de las siguientes opciones: \n -El path del archivo de configuraciones\n -El parametro testOnly \n");
			return 1;
		}
	if(string_equals_ignore_case(argv[1],"testOnly")){
		AdministrarTests

	}else if(configuracionesValidas(argv[1]))
		cargarConfiguraciones(argv[1]);
	else
		return 1;


	pthread_create(&hiloConsola,NULL,(void* (*)(void*))consola,NULL);
	pthread_create(&hiloConexiones,NULL,(void* (*)(void*)) administrarConexiones,NULL);

	pthread_join(hiloConsola,NULL);
	pthread_join(hiloConexiones,NULL);

	log_destroy(logConexiones);
	log_destroy(logDebug);
	free(memoriaPrincipal);
	return 0;
}
