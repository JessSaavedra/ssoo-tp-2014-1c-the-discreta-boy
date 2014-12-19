/*
 * esctructurasAdministrativas.h
 *
 *  Created on: 02/05/2014
 *      Author: utnso
 */

#ifndef ESCTRUCTURASADMINISTRATIVAS_H_
#define ESCTRUCTURASADMINISTRATIVAS_H_

#include <commons/collections/dictionary.h>
#include <pthread.h>

typedef struct {
	int PID;
	t_dictionary* segmentos;
	int ultimaDireccionVirtualAsignada;
} t_tablaSegmentosProceso;

typedef struct {
	int SID;
	int base;
	int tamanio;
	unsigned int ubicacionFisica;
	pthread_rwlock_t lock;
} t_segmento;


#endif /* ESCTRUCTURASADMINISTRATIVAS_H_ */


