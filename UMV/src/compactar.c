/*
 * compactar.c
 *
 *  Created on: 21/05/2014
 *      Author: utnso
 */

#include "stdUMV.h"
#include <commons/log.h>

static int proximaPosicion;

void acomodarSegmento(t_segmento* segmento){log_debug(logDebug,"Entre a acomodarSegmento y la prox posicion es %d",proximaPosicion);

	char* ubicacionOriginal=memoriaPrincipal+segmento->ubicacionFisica;
	char* ubicacionFinal=memoriaPrincipal+proximaPosicion;

	if(ubicacionFinal==ubicacionOriginal){
		proximaPosicion+=segmento->tamanio;
		return;
	}
pthread_rwlock_rdlock(&lockDump);
pthread_rwlock_wrlock(&segmento->lock);
	memmove((void*)ubicacionFinal,(void*)ubicacionOriginal,segmento->tamanio);log_debug(logDebug,"Antes de reasignar ubicacion fisica la prox posicion es %d",proximaPosicion);
	segmento->ubicacionFisica=proximaPosicion;log_debug(logDebug,"A la ubicacionFisica le asigne %d, proximaPosicion es %d",segmento->ubicacionFisica,proximaPosicion);
pthread_rwlock_unlock(&segmento->lock);
pthread_rwlock_unlock(&lockDump);
	proximaPosicion+=segmento->tamanio;
	return;
}

int compactarMemoria(void){log_debug(logDebug,"Entre a compactarmemoria");


pthread_rwlock_rdlock(&lockSegmentosEnMemoria);
	list_iterate(segmentosEnMemoria,(void(*)(void*))acomodarSegmento);
pthread_rwlock_unlock(&lockSegmentosEnMemoria);
	proximaPosicion=0;
	return 0;

}

