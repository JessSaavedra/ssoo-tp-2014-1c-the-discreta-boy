/*
 * Configuracion.c
 *
 *  Created on: 27/06/2014
 *      Author: utnso
 */


/*
 * configuracion.c
 *
 *  Created on: 26/06/2014
 *      Author: utnso
 */
#include "kernel.h"

int stringANumero(char* cadena, long* numero){

	char*primerCaracterNoNumerico;

	errno=0;
	*numero = strtol(cadena,&primerCaracterNoNumerico,10);
	if ( errno !=0|| *primerCaracterNoNumerico!='\0') //Chequeo de errores: caracteres no numericos
		return 1;
	return 0;
}

int configuracionesValidas(char* archivo){;
	t_config* configuracion = config_create(archivo);

	if (	config_has_property(configuracion,"ID_HIO") &&
			config_has_property(configuracion,"HIO") &&
			config_has_property(configuracion,"SEMAFOROS") &&
			config_has_property(configuracion,"VALOR_SEMAFORO") &&
			config_has_property(configuracion,"MULTIPROGRAMACION") &&
			config_has_property(configuracion,"PUERTO_PROG") &&
			config_has_property(configuracion,"PUERTO_CPU") &&
			config_has_property(configuracion,"PUERTO_UMV") &&
			config_has_property(configuracion,"IP_UMV") &&
			config_has_property(configuracion,"QUANTUM") &&
			config_has_property(configuracion,"RETARDO") &&
			config_has_property(configuracion,"STACK") &&
			config_has_property(configuracion,"VAR_COMPARTIDAS")){
		config_destroy(configuracion);
		return 1;
	}
	else{
		config_destroy(configuracion);
		return 0;
	}
}

int cargarConfiguraciones(char* archivo){
	int error=0,i,cantidadHilosIO,cantidadSemaforos,cantidadCompartidas;
	long int valorEnInt=0;
	tipoDispositivo* dispositivo;
	t_semaforo* semaforo;

	t_config* configuracion = config_create(archivo);

//-----------------Dispositivos IO----------------------//


	char** hilosIO=config_get_array_value(configuracion,"ID_HIO");
	char** valoresIO=config_get_array_value(configuracion,"HIO");

	for(cantidadHilosIO=0;hilosIO[cantidadHilosIO]!=NULL;cantidadHilosIO++);

	for(i=0;i<cantidadHilosIO;i++){
		dispositivo=malloc(sizeof(tipoDispositivo));
		error=stringANumero(valoresIO[i],&valorEnInt);
		free(valoresIO[i]);
		if (error)	return -1;
		dispositivo->valorDeRetardo=valorEnInt;
		dictionary_put(dispositivosIO,string_duplicate(hilosIO[i]),dispositivo);
		free(hilosIO[i]);
	}

	free(hilosIO);
	free(valoresIO);

//--------------Semaforos----------//

	char** arraySemaforos=config_get_array_value(configuracion,"SEMAFOROS");
	char** valoresSemaforo=config_get_array_value(configuracion,"VALOR_SEMAFORO");

	for(cantidadSemaforos=0;arraySemaforos[cantidadSemaforos]!=NULL;cantidadSemaforos++);log_debug(logDebug,"Se cragarán %d semáforos",cantidadSemaforos);

	for(i=0;i<cantidadSemaforos;i++){
		semaforo=malloc(sizeof(t_semaforo));
		error=stringANumero(valoresSemaforo[i],&valorEnInt);
		free(valoresSemaforo[i]);
		if (error)	return -1;
		semaforo->bloqueados=queue_create();
		semaforo->contador=valorEnInt;
		dictionary_put(semaforos,string_duplicate(arraySemaforos[i]),semaforo);log_debug(logDebug,"Se agrega el semaforo %s",arraySemaforos[i]);
		free(arraySemaforos[i]);
	}

	free(arraySemaforos);
	free(valoresSemaforo);

//-----------Variables Compartidas---------------//

	char** arrayCompartidas=config_get_array_value(configuracion,"VAR_COMPARTIDAS");

	for(cantidadCompartidas=0;arrayCompartidas[cantidadCompartidas]!=NULL;cantidadCompartidas++);

	for(i=0;i<cantidadCompartidas;i++){
		dictionary_put(variablesGlobales,string_duplicate(arrayCompartidas[i]),malloc(sizeof(int)));
		free(arrayCompartidas[i]);
	}

	free(arrayCompartidas);

//-----------Otros-------//

	valorEnInt=config_get_int_value(configuracion,"MULTIPROGRAMACION");
	sem_init(gradoDeMultiprogramacion,0,valorEnInt);

	valorEnInt=config_get_int_value(configuracion,"QUANTUM");
	quantum=valorEnInt;

	valorEnInt=config_get_int_value(configuracion,"RETARDO");
	retardo=valorEnInt;

	valorEnInt=config_get_int_value(configuracion,"STACK");
	tamanioStack=valorEnInt;

	IPUMV=string_duplicate(config_get_string_value(configuracion,"IP_UMV"));
	puertoCPU=string_duplicate(config_get_string_value(configuracion,"PUERTO_CPU"));
	puertoPP=string_duplicate(config_get_string_value(configuracion,"PUERTO_PROG"));
	puertoUMV=string_duplicate(config_get_string_value(configuracion,"PUERTO_UMV"));

	config_destroy(configuracion);

	return 0;
}
