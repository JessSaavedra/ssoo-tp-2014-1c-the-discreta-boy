#include "stdUMV.h"
#include <commons/log.h>

//Funciones Auxiliares (probablemente privadas en el futuro)

int posicionFisicaSegmentoEsMayor(void* actual, void* nuevo){

	t_segmento* nuevoSegmento = (t_segmento*) nuevo;
	t_segmento* actualSegmento = (t_segmento*) actual;

pthread_rwlock_rdlock(&nuevoSegmento->lock);
	int nuevaUbicacionFisica = nuevoSegmento->ubicacionFisica;
pthread_rwlock_unlock(&nuevoSegmento->lock);

pthread_rwlock_rdlock(&actualSegmento->lock);
	int actualUbicacionFisica = actualSegmento->ubicacionFisica;
pthread_rwlock_unlock(&actualSegmento->lock);

	return nuevaUbicacionFisica >= actualUbicacionFisica;
}

int getSegmentoActual(int proceso, int baseSegmento,t_segmento** segmento){

	t_tablaSegmentosProceso* tablaSegmentosActual;

	char* key =string_itoa(proceso);
	if (!dictionary_has_key(tablasDeSegmentos,key)){
		free(key);
		return PROCESOINEXISTENTE;
	}
	free(key);
	key = string_itoa(proceso);
	tablaSegmentosActual = (t_tablaSegmentosProceso*) dictionary_get(tablasDeSegmentos,key);
	free(key);
	key = string_itoa(baseSegmento);
	if (!dictionary_has_key(tablaSegmentosActual->segmentos, key)){
		free(key);
		return SEGMENTOINEXISTENTE;
	}
	free(key);
	key = string_itoa(baseSegmento);
	*segmento = (t_segmento*) dictionary_get(tablaSegmentosActual->segmentos, key);
	free(key);
	return 0;
}

int ultimaPosicionFisica(t_segmento* unSegmento){
pthread_rwlock_rdlock(&unSegmento->lock);
	int posicion = ((unSegmento->ubicacionFisica)+(unSegmento->tamanio)-1);
pthread_rwlock_unlock(&unSegmento->lock);
	return posicion;
}

int distanciaAFinMemoria(t_segmento* unSegmento){
	return (tamanioMemoriaPrincipal-ultimaPosicionFisica(unSegmento)-1);
}

int posicionFirstFit(int tamanioSegmento){log_debug(logDebug,"Entre al algoritmo FirstFit, para tamanio %d",tamanioSegmento);//

	int i=0;
		int tamanioDisponible;
		int distanciaAlFinal;
		int posicionFinal;
		t_segmento* primerSegmento;
		t_segmento* segmentoSiguiente;

		if(list_size(segmentosEnMemoria)==0){ log_debug(logDebug,"Entre con list size 0");
			return tamanioSegmento<=tamanioMemoriaPrincipal?0:SINESPACIO;
			}

		if(list_size(segmentosEnMemoria)==1){	log_debug(logDebug,"Entre con list size 1");
			primerSegmento =list_get(segmentosEnMemoria,0);
		pthread_rwlock_rdlock(&primerSegmento->lock);
			tamanioDisponible =primerSegmento->ubicacionFisica;
		pthread_rwlock_unlock(&primerSegmento->lock);

			if(tamanioDisponible<tamanioSegmento && distanciaAFinMemoria(primerSegmento)<tamanioSegmento){
				return SINESPACIO;
			}

			if (tamanioDisponible != 0 && tamanioDisponible>tamanioSegmento)
				posicionFinal=0;
				else posicionFinal= ultimaPosicionFisica(primerSegmento)+1;
			return posicionFinal;
			}

		//-----------------------


		int tamanioLista=list_size(segmentosEnMemoria);
		for(i=1;i<tamanioLista;i++){	log_debug(logDebug,"Entre con list size %d",tamanioLista);
			primerSegmento = list_get(segmentosEnMemoria,i-1); log_debug(logDebug,"Elegi como segmento primero base %d, sid %d",primerSegmento->base,primerSegmento->SID);
			segmentoSiguiente = list_get(segmentosEnMemoria,i); log_debug(logDebug,"Elegi como segmento siguiente base %d,sid %d,con i %d",segmentoSiguiente->base,segmentoSiguiente->SID,i);

		pthread_rwlock_rdlock(&segmentoSiguiente->lock);
			tamanioDisponible= (segmentoSiguiente->ubicacionFisica)-ultimaPosicionFisica(primerSegmento)-1;
		pthread_rwlock_unlock(&segmentoSiguiente->lock);
			log_debug(logDebug,"Estoy buscando para un tamanio de segmetno %d",tamanioSegmento);

			if (tamanioDisponible >= tamanioSegmento){
				posicionFinal=ultimaPosicionFisica(primerSegmento)+1;
				return posicionFinal;
			}
		}

		//---------------



		distanciaAlFinal=distanciaAFinMemoria(segmentoSiguiente);
		posicionFinal=ultimaPosicionFisica(segmentoSiguiente)+1;

		if (distanciaAlFinal<tamanioSegmento){
			return SINESPACIO;
			} else {
			return posicionFinal;
			}

}

int posicionWorstFit(int tamanioSegmento){//TODO lee
	int i=1;
	int tamanioMaxDisponible=0;
	int tamanioMaxAnterior;
	int posicionTamanioMaximo;

	t_segmento* primerSegmento;
	t_segmento* segmentoSiguiente;


	if(list_size(segmentosEnMemoria)==0){
		return 0;
		};

	if(list_size(segmentosEnMemoria)==1){
		primerSegmento =list_get(segmentosEnMemoria,0);
	pthread_rwlock_rdlock(&primerSegmento->lock);
		tamanioMaxAnterior= primerSegmento->ubicacionFisica;
	pthread_rwlock_unlock(&primerSegmento->lock);
		tamanioMaxDisponible= distanciaAFinMemoria(primerSegmento);

		if(tamanioMaxAnterior< tamanioSegmento && distanciaAFinMemoria(primerSegmento)< tamanioSegmento){
			return SINESPACIO;
		}

		if (tamanioMaxAnterior != 0 && tamanioMaxDisponible<tamanioMaxAnterior)
			posicionTamanioMaximo = 0;
			else posicionTamanioMaximo= ultimaPosicionFisica(primerSegmento)+1;
		return posicionTamanioMaximo;
		}

	//-----------------
	while(i<list_size(segmentosEnMemoria)){
		tamanioMaxAnterior=tamanioMaxDisponible;
		primerSegmento = list_get(segmentosEnMemoria,i-1);
		segmentoSiguiente = list_get(segmentosEnMemoria,i);
	pthread_rwlock_rdlock(&segmentoSiguiente->lock);
		tamanioMaxDisponible= (segmentoSiguiente->ubicacionFisica)-ultimaPosicionFisica(primerSegmento)-1;
	pthread_rwlock_unlock(&segmentoSiguiente->lock);
		if (tamanioMaxDisponible > tamanioMaxAnterior && tamanioMaxDisponible >= tamanioSegmento){
			posicionTamanioMaximo=ultimaPosicionFisica(primerSegmento)+1;
			}

		i++;
		}

	//-------------

	tamanioMaxDisponible=distanciaAFinMemoria(segmentoSiguiente);

	if (tamanioMaxDisponible > tamanioMaxAnterior && tamanioMaxDisponible >= tamanioSegmento){
						posicionTamanioMaximo=ultimaPosicionFisica(segmentoSiguiente)+1;
						return posicionTamanioMaximo;
	} else {
	return SINESPACIO;
	}

}

t_tablaSegmentosProceso* newTablaSegmentos(int PID, t_dictionary* segmentos,  int ultimaPosicionVirtualAsignada){

	t_tablaSegmentosProceso* tabla = (t_tablaSegmentosProceso*) malloc(sizeof(t_tablaSegmentosProceso));

	tabla->PID=PID;
	tabla->segmentos=segmentos;
	tabla->ultimaDireccionVirtualAsignada=ultimaPosicionVirtualAsignada;
	return tabla;
}

t_segmento* newSegmento(int baseVirtual, int tamanio, int ubicacionFisica){log_debug(logDebug,"Entre a newSegmento");

	if (ubicacionFisica == SINESPACIO){
		log_debug(logDebug,"newSegmento devuelve NULL por no haber espacio");
		return NULL;
	}

	t_segmento* segmento= (t_segmento*) malloc(sizeof(t_segmento)); log_debug(logDebug,"Aloque memoria para el segmento :D");
	pthread_rwlock_t lockAtributos = PTHREAD_RWLOCK_INITIALIZER;

	segmento->base=baseVirtual;
	segmento->tamanio=tamanio;
	segmento->ubicacionFisica=ubicacionFisica;							log_debug(logDebug,"Ubicacion fisica asignada");
	segmento->lock = lockAtributos;							log_debug(logDebug,"Segmento instanciado");
	return segmento;
}


int addSegmento(t_tablaSegmentosProceso* proceso, t_segmento* segmento){

	char* key = string_itoa((segmento->base));

	segmento->SID=dictionary_size(proceso->segmentos);
	dictionary_put(proceso->segmentos,key,(void*) segmento);
	free(key);
	proceso->ultimaDireccionVirtualAsignada=segmento->base;
	return 0;
}

int getSiguienteDireccionVirtual(t_tablaSegmentosProceso* proceso){ log_debug(logDebug,"Entre a getSiguienteDirVirtual");


	t_segmento* ultimoSegmento;

	char* key = string_itoa(proceso->ultimaDireccionVirtualAsignada);
	if(dictionary_has_key(proceso->segmentos,key)){
		free(key);
		key= string_itoa(proceso->ultimaDireccionVirtualAsignada);
		ultimoSegmento = (t_segmento*) dictionary_get(proceso->segmentos,key); log_debug(logDebug,"Asigne el ultimo segmento");
		free(key);
		return proceso->ultimaDireccionVirtualAsignada + ultimoSegmento->tamanio + rand()%100;
	}
	else{
		free(key);
		return rand()%100;
	}

}

int getSiguienteDireccionFisica(int tamanioSegmento){log_debug(logDebug,"Entre a getSigDirFisica");

	int siguienteDireccion; log_debug(logDebug,"Declare SiguienteDireccion");

	pthread_rwlock_rdlock(&lockSegmentosEnMemoria);
	siguienteDireccion=(*algoritmoPosicionFisica)(tamanioSegmento); log_debug(logDebug,"asigne SiguienteDireccion");
	pthread_rwlock_unlock(&lockSegmentosEnMemoria);
	if (siguienteDireccion==SINESPACIO){
		compactarMemoria();
		pthread_rwlock_rdlock(&lockSegmentosEnMemoria);
		siguienteDireccion=(*algoritmoPosicionFisica)(tamanioSegmento);
		pthread_rwlock_unlock(&lockSegmentosEnMemoria);
	}							log_debug(logDebug,"Al salir de getSiguienteDireccionFisica se devuelve %d",siguienteDireccion);
	return siguienteDireccion;
}

int configuracionesValidas(char* pathConfiguracion){
	t_config* configuracion = config_create(pathConfiguracion);

	if(config_has_property(configuracion,"Puerto") && config_has_property(configuracion,"MemoriaPrincipal") &&
			config_has_property(configuracion,"Algoritmo") && config_has_property(configuracion,"Retardo")){
		config_destroy(configuracion);
		return 1;
	} else {
		fprintf(stderr,"El archivo de configuracion no cumple con los parametros requeridos\n");
		config_destroy(configuracion);
		return 0;
	}
}

//Funciones Internas UMV

void cargarConfiguraciones(char* pathConfiguracion){log_debug(logDebug,"Cargando configuraciones...");
	long valorEnInt=0;
	t_config* configuracion = config_create(pathConfiguracion);

	puertoUMV=string_duplicate(config_get_string_value(configuracion,"Puerto"));						log_debug(logDebug,"|%s|",puertoUMV);
	char* tamanioMemoria=string_duplicate(config_get_string_value(configuracion,"MemoriaPrincipal"));	log_debug(logDebug,"|%s|",tamanioMemoria);
	char* algoritmo=string_duplicate(config_get_string_value(configuracion,"Algoritmo"));				log_debug(logDebug,"|%s|",algoritmo);
	char* retardoInicial=string_duplicate(config_get_string_value(configuracion,"Retardo"));			log_debug(logDebug,"|%s|",retardoInicial);
	stringANumero(tamanioMemoria,&valorEnInt);
	config_destroy(configuracion);
	tamanioMemoriaPrincipal=valorEnInt;
	memoriaPrincipal=malloc(valorEnInt);																log_debug(logDebug,"Memoria alocada, tamaño: %d",tamanioMemoriaPrincipal);
	if(string_equals_ignore_case(algoritmo,"WF"))
			setAlgoritmo(posicionWorstFit);
		else
			setAlgoritmo(posicionFirstFit);
	stringANumero(retardoInicial,&valorEnInt);
	setRetardo(valorEnInt);
	free(tamanioMemoria);
	free(algoritmo);
	free(retardoInicial);
}

void setAlgoritmo(int (*algoritmo)(int)){
	algoritmoPosicionFisica=algoritmo;
}

void setRetardo(long tiempo){
	retardo = tiempo;
}

int leerMemoria(int proceso, int baseSegmento, int desplazamiento, int tamanioBloque,void** datosLeidos){log_debug(logDebug,"Entre a leerMemoria");

	t_segmento* segmentoActual;
	int error;

pthread_rwlock_rdlock(&lockTablasDeSegmentos);
	error = getSegmentoActual(proceso, baseSegmento,&segmentoActual);
pthread_rwlock_unlock(&lockTablasDeSegmentos);
	if (error <0/*Devolvio error*/)
		return error;
	if((segmentoActual->base+desplazamiento+tamanioBloque>segmentoActual->base+segmentoActual->tamanio)\
	||(segmentoActual->base+desplazamiento+tamanioBloque<segmentoActual->base))
		return SEGMENTATIONFAULT;
	*datosLeidos=malloc(tamanioBloque);log_debug(logDebug, "Aloque memoria para datos leidos\n");
pthread_rwlock_rdlock(&lockDump);
pthread_rwlock_rdlock(&segmentoActual->lock);
	memcpy(*datosLeidos,(void*)&memoriaPrincipal[segmentoActual->ubicacionFisica + desplazamiento],tamanioBloque);
pthread_rwlock_unlock(&segmentoActual->lock);
pthread_rwlock_unlock(&lockDump);
	return 0;
}

int escribirMemoria(int proceso, int baseSegmento, int desplazamiento, int tamanioBloque, void* buffer){log_debug(logDebug,"Entre a escribirMemoria");

	t_segmento* segmentoActual;
	int error;

pthread_rwlock_rdlock(&lockTablasDeSegmentos);
	error = getSegmentoActual(proceso, baseSegmento,&segmentoActual);
pthread_rwlock_unlock(&lockTablasDeSegmentos);
	if (error <0/*Devolvio error*/){																	log_debug(logDebug,"Error en el segmento");
		return error;}
	if((segmentoActual->base+desplazamiento+tamanioBloque>segmentoActual->base+segmentoActual->tamanio)\
		||(segmentoActual->base+desplazamiento+tamanioBloque<segmentoActual->base)){								log_debug(logDebug,"Error Segmentation fault");
			return SEGMENTATIONFAULT;}
pthread_rwlock_rdlock(&lockDump);																										log_debug(logDebug,"Antes de hacer el Memcpy de EscribirMemoria");
pthread_rwlock_rdlock(&segmentoActual->lock);
	memcpy((void*)&memoriaPrincipal[segmentoActual->ubicacionFisica + desplazamiento],buffer,tamanioBloque);log_debug(logDebug,"Dsp de hacer el Memcpy de EscribirMemoria");
pthread_rwlock_unlock(&segmentoActual->lock);
pthread_rwlock_unlock(&lockDump);
	return 0;
}

int getProcesoActual(int PID,t_tablaSegmentosProceso** procesoActual){

	char* key = string_itoa(PID);
	if (dictionary_has_key(tablasDeSegmentos,key)){
		free(key);
		key = string_itoa(PID);
		*procesoActual = (t_tablaSegmentosProceso*) dictionary_get(tablasDeSegmentos,key); log_debug(logDebug,"Asigne proceso Actual");
		free(key);
		return 0;
	}

	else{
		free(key);
		return PROCESOINEXISTENTE;
	}

}

int crearSegmento(int PID, int tamanio){

	t_tablaSegmentosProceso* procesoActual;
	t_segmento* nuevoSegmento;
	int direccionVirtual,error;

pthread_rwlock_wrlock(&lockTablasDeSegmentos);
	error = getProcesoActual(PID,&procesoActual);
	if(error){
		pthread_rwlock_unlock(&lockTablasDeSegmentos);
		return error;
	}																					log_debug(logDebug,"Sali del if");
	direccionVirtual=getSiguienteDireccionVirtual(procesoActual);
	int dirFisica=getSiguienteDireccionFisica(tamanio);
	nuevoSegmento = newSegmento(direccionVirtual,tamanio,dirFisica); 					log_debug(logDebug,"Asigne nuevo segmento con dir fisica %d",dirFisica);
	if (nuevoSegmento == NULL){															log_debug(logDebug,"newSegmento devolvio %s",nuevoSegmento);
		pthread_rwlock_unlock(&lockTablasDeSegmentos);log_debug(logDebug,"Unlockee lockTablasDeSegmentos");;
		return SINESPACIO;
	}
	addSegmento(procesoActual,nuevoSegmento); 											log_debug(logDebug,"Agregue el segmento");
pthread_rwlock_wrlock(&lockSegmentosEnMemoria);
	if(tamanio>0) list_add_order_by(segmentosEnMemoria,nuevoSegmento,posicionFisicaSegmentoEsMayor);	log_debug(logDebug,"el inicio del segmento antes de Retornar de la creacion es %d",nuevoSegmento->base);
pthread_rwlock_unlock(&lockTablasDeSegmentos);log_debug(logDebug,"Unlockee lockTablasDeSegmentos");
pthread_rwlock_unlock(&lockSegmentosEnMemoria);log_debug(logDebug,"Unlockee lockSegmentosEnMemoria");
	return direccionVirtual;
}

int crearTablaProceso(int PID){										log_debug(logDebug,"Entre a crearTablaProceso");

	t_tablaSegmentosProceso* nuevoProceso;							log_debug(logDebug,"Antes de los wait");

pthread_rwlock_wrlock(&lockTablasDeSegmentos);						log_debug(logDebug,"Ya consegui el primer semaforo! :D");
pthread_rwlock_wrlock(&lockSegmentosEnMemoria);						log_debug(logDebug,"Ya consegui los 2 semaforos! :D");
	char* key = string_itoa(PID);
	if (dictionary_has_key(tablasDeSegmentos,key)){
		pthread_rwlock_unlock(&lockTablasDeSegmentos);
		pthread_rwlock_unlock(&lockSegmentosEnMemoria);
		free(key);
		return PROCESOYAEXISTE;
	}
	free(key);
	nuevoProceso=newTablaSegmentos(PID, dictionary_create(),0);
	key = string_itoa(PID);
	dictionary_put(tablasDeSegmentos,key,(void*) nuevoProceso);log_debug(logDebug,"A punto de desbloquear semaforos");
pthread_rwlock_unlock(&lockTablasDeSegmentos);
pthread_rwlock_unlock(&lockSegmentosEnMemoria);
	free(key);
	return 0;
}

void destruirSegmento (t_segmento* segmento){								log_debug(logDebug,"Entre a destruirSegmento!");

		bool _segmentoEsEsBuscado(t_segmento* segmentoDado){
				bool a= (segmento->base== segmentoDado->base);			log_debug(logDebug,"La base dada es %d y el segmento actual es %d",segmento->base,segmentoDado->base);
				return a;
		}
	if(segmento->tamanio>0)
		list_remove_and_destroy_by_condition(segmentosEnMemoria,(bool (*)(void*))_segmentoEsEsBuscado,free);
	else{
		log_debug(logDebug,"Se destruyo el segmento %d de tamanio %d",segmento->base,segmento->tamanio);
		free(segmento);
	}
	return;
}

void buscarYDestruirProceso(t_tablaSegmentosProceso* proceso){
	char* key = string_itoa(proceso->PID);
	dictionary_remove_and_destroy(tablasDeSegmentos,key,(void (*)(void*))destruirTablaProceso);
	free(key);
	return;
}

void destruirTablaProceso(t_tablaSegmentosProceso* proceso){
	dictionary_destroy_and_destroy_elements(proceso->segmentos,(void (*)(void*))destruirSegmento);
	free(proceso);
	return;
}

void destruirEstructurasEnMemoria(void){
	dictionary_destroy_and_destroy_elements(tablasDeSegmentos,(void (*)(void*))destruirTablaProceso);
	list_destroy_and_destroy_elements(segmentosEnMemoria,free);
	return;
}

int destruirProceso(int PID){					log_debug(logDebug,"entre a destruir el proceso %d",PID);


	t_tablaSegmentosProceso* procesoActual;
	int error;

pthread_rwlock_wrlock(&lockTablasDeSegmentos);					log_debug(logDebug,"lockeo primer semaforo");
pthread_rwlock_wrlock(&lockSegmentosEnMemoria);log_debug(logDebug,"lockeo segundo semaforo");fflush(stdout);
	error = getProcesoActual(PID,&procesoActual);
	if(error){
		pthread_rwlock_unlock(&lockTablasDeSegmentos);			log_debug(logDebug,"hubo error, unlockeo primer semaforo");
		pthread_rwlock_unlock(&lockSegmentosEnMemoria);			log_debug(logDebug,"hubo error, unlockeo segundo semaforo");
		return error;
	}
	buscarYDestruirProceso(procesoActual);
pthread_rwlock_unlock(&lockTablasDeSegmentos);				log_debug(logDebug,"unlockeo primer semaforo");
pthread_rwlock_unlock(&lockSegmentosEnMemoria);				log_debug(logDebug,"unlockeo segundo semaforo");
	return 0;
}
