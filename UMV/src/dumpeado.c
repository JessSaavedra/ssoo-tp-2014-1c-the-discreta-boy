/*
 * dumpeado.c
 *
 *  Created on: 13/06/2014
 *      Author: utnso
 */

#include "stdUMV.h"


int obtenerParametrosNumericos(char*, long*, int);

static FILE* archivoAbiertoParaDump;
static int ultimaPosicionUsadaParaDump;
static int espacioLibre;
static int contadorAuxiliarOrganizacion;

//Funciones auxiliares

void dumpearSegmentoEnArchivoAbierto(char* segID,t_segmento* segmento){
pthread_rwlock_rdlock(&segmento->lock);
	fprintf(archivoAbiertoParaDump,"%5u     |%10d        | %5d     |%10d        \n",segmento->SID,segmento->base,segmento->tamanio,segmento->ubicacionFisica);
pthread_rwlock_unlock(&segmento->lock);
fflush(archivoAbiertoParaDump);
}

void dumpearProcesoEnArchivoAbierto(char* PID,t_tablaSegmentosProceso* tablaADumpear){

	fprintf(archivoAbiertoParaDump,"Dump de Estructuras del proceso %d\n",tablaADumpear->PID);
	fprintf(archivoAbiertoParaDump," Segmento | Posicion Inicial |  Tamanio  | Ubicacion Fisica \n");

	dictionary_iterator(tablaADumpear->segmentos,(void(*)(char*,void*))dumpearSegmentoEnArchivoAbierto);

	fprintf(archivoAbiertoParaDump,"\n----------o----------\n \n");
	fflush(archivoAbiertoParaDump);

}

void dumpearEstadoDelSegmento(t_segmento* segmento){
pthread_rwlock_rdlock(&segmento->lock);
	while(ultimaPosicionUsadaParaDump<(segmento->ubicacionFisica)){
	pthread_rwlock_unlock(&segmento->lock);
		fprintf(archivoAbiertoParaDump,"[ ]");
		contadorAuxiliarOrganizacion++;
		espacioLibre++;
		ultimaPosicionUsadaParaDump++;
		if(contadorAuxiliarOrganizacion%10==0){
			fprintf(archivoAbiertoParaDump,"\n");
		}
	pthread_rwlock_rdlock(&segmento->lock);
	}
	int posicionFinSegmento=(segmento->ubicacionFisica)+(segmento->tamanio);
pthread_rwlock_unlock(&segmento->lock);
	while(ultimaPosicionUsadaParaDump<posicionFinSegmento){
		fprintf(archivoAbiertoParaDump,"[X]");
		contadorAuxiliarOrganizacion++;
		ultimaPosicionUsadaParaDump++;
		if(contadorAuxiliarOrganizacion%10==0){
			fprintf(archivoAbiertoParaDump,"\n");
		}
	}

	fflush(archivoAbiertoParaDump);

}
//funciones Principales

int dumpContenido(void* parametros){

	long parametrosSeparados[2];
	if(obtenerParametrosNumericos((char*)parametros,parametrosSeparados,2)) {
		return 1;
	}
	int i=0;
	int offset= parametrosSeparados[0];
	int cantidadALeer= parametrosSeparados[1];

	FILE* archivo;
	archivo=fopen("./dumpContenido","a");

	char* hora = temporal_get_string_time();
	fprintf(archivo,"Dump realizado a las: %s\n\n",hora);
	free(hora);

	fprintf(archivo,"Dump de Contenido de memoria fisica con offset %d, %d bytes leidos\n",offset,cantidadALeer);

	fprintf(archivo," | Contenido | Posicion en Memoria Principal\n");

pthread_rwlock_wrlock(&lockDump);
	for(i=0;i<cantidadALeer;i++){
		fprintf(archivo," |    Ox%X    |                 %10u\n",*(int*)memoriaPrincipal+offset+i,offset+i);
	}
pthread_rwlock_unlock(&lockDump);

	fflush(archivo);
	fclose(archivo);
/*
	printf("Desea guardar este informe como archivo? S/N :");	Comentado por si en algun momento se requiere
	char confirmacion;
	confirmacion=getchar();
	getchar();		//Catchea el /n de mas y evita que se quede esperando
	if(string_equals_ignore_case(&confirmacion,"N"))
		remove("./dumpContenido");
*/
	return 0;
}

int dumpEstructurasDe(void* PID){

	PID++;
pthread_rwlock_rdlock(&lockTablasDeSegmentos);
	if(!dictionary_has_key(tablasDeSegmentos,(char*)PID)){
		pthread_rwlock_unlock(&lockTablasDeSegmentos);
		fprintf(stderr,"El proceso indicado no existe\n");
		return 1;
	}

	t_tablaSegmentosProceso* tablaADumpear=dictionary_get(tablasDeSegmentos,(char*)PID);

	FILE* archivo=fopen("./dumpEstructurasProceso","a");

	char* hora = temporal_get_string_time();
	fprintf(archivo,"Dump realizado a las: %s\n\n",hora);
	free(hora);
	fprintf(archivo,"Dump de Estructuras del proceso %d\n",tablaADumpear->PID);

	fprintf(archivo,"     Segmento | Posicion Inicial | Tamanio | Ubicacion Fisica\n");

	archivoAbiertoParaDump=archivo;
	dictionary_iterator(tablaADumpear->segmentos,(void(*)(char*,void*))dumpearSegmentoEnArchivoAbierto);
pthread_rwlock_unlock(&lockTablasDeSegmentos);
	fclose(archivo);
	archivoAbiertoParaDump=NULL;

	return 0;

}

int dumpTodasLasEstructuras(void){
	FILE* archivo=fopen("./dumpEstructurasCompleto","a");
	archivoAbiertoParaDump=archivo;
	char* hora = temporal_get_string_time();
	fprintf(archivo,"Dump realizado a las: %s\n\n",hora);
	free(hora);
pthread_rwlock_rdlock(&lockTablasDeSegmentos);
	dictionary_iterator(tablasDeSegmentos,(void(*)(char*,void*))dumpearProcesoEnArchivoAbierto);
pthread_rwlock_unlock(&lockTablasDeSegmentos);
	fclose(archivo);
	archivoAbiertoParaDump=NULL;

	return 0;
}

int dumpMemoriaCompleta(void){
	FILE* archivo=fopen("./dumpEstadoMemoriaCompleto","a");
	int i;
	char* hora = temporal_get_string_time();
	fprintf(archivo,"Dump realizado a las: %s\n\n",hora);
	free(hora);
	fprintf(archivo,"Estado Actual De la Memoria\n\n [ ]=Vacio [X]=Alocado\n");

	archivoAbiertoParaDump=archivo;
	ultimaPosicionUsadaParaDump=0;
	espacioLibre=0;
pthread_rwlock_rdlock(&lockSegmentosEnMemoria);
	list_iterate(segmentosEnMemoria,(void (*)(void*))dumpearEstadoDelSegmento);
pthread_rwlock_unlock(&lockSegmentosEnMemoria);

	for(i=1;i<=(tamanioMemoriaPrincipal-ultimaPosicionUsadaParaDump);i++){

			fprintf(archivoAbiertoParaDump,"[ ]");
			contadorAuxiliarOrganizacion++;
			espacioLibre++;
			if(contadorAuxiliarOrganizacion%10==0){
				fprintf(archivoAbiertoParaDump,"\n");
			}
	}

	fprintf(archivo,"\nEspacio libre = %d\n Tamanio Total = %d\n",espacioLibre,tamanioMemoriaPrincipal);

	fclose(archivo);
	archivoAbiertoParaDump=NULL;

	return 0;
}
