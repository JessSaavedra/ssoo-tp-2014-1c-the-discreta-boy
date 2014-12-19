/*
 * syscalls.c
 *
 *  Created on: 19/06/2014
 *      Author: utnso
 */

#include "kernel.h"

int manejarSyscall(int codigoOperacion,t_cliente* CPU){

	int excepcion;

	switch(codigoOperacion){
		case QUANTUMFINALIZADO:
			excepcion = recuperarProcesoListo(CPU);
			break;
		case OBTENERCOMPARTIDA:
			excepcion = operarElementoCompartido(CPU,(int(*)(void*,t_cliente*))enviarCompartida,LEER_COMPARTIDAINEXISTENTE,variablesGlobales);
			break;
		case GRABARCOMPARTIDA:
			excepcion = operarElementoCompartido(CPU,(int(*)(void*,t_cliente*))escribirCompartida,ESCRIBIR_COMPARTIDAINEXISTENTE,variablesGlobales);
			break;
		case WAIT:
			excepcion = operarElementoCompartido(CPU,(int(*)(void*,t_cliente*))wait,SEMAFOROINEXISTENTE,semaforos);
			break;
		case SIGNAL:
			excepcion = operarElementoCompartido(CPU,(int(*)(void*,t_cliente*))post,SEMAFOROINEXISTENTE,semaforos);
			break;
		case IO:
			excepcion = operarElementoCompartido(CPU,(int(*)(void*,t_cliente*))realizarIO,IOINEXISTENTE,dispositivosIO);
			break;
		case IMPRIMIR:
			excepcion = imprimir(CPU);
			break;
		case IMPRIMIRTEXTO:
			excepcion = imprimirTexto(CPU);
			break;
		case PROGRAMATERMINADO:
			excepcion = recuperarProcesoFinalizado(CPU);
			break;
		case CPUSEDESCONECTA:
			excepcion = recuperarProcesoDeCPUSaliente(CPU);
			break;
		default:
			excepcion = CPUMURIO; //Si se recibe algo raro se asume que el CPU murio y se mantiene al sistema en un estado consistente
			break;
		}																								log_debug(logDebug,"La ejecucion de la syscall retorno: %d",excepcion);
	return excepcion;
}

int recuperarProcesoListo(t_cliente* CPU){																log_debug(logDebug,"Entre a recuperarProcesoListo con el cpu %d",CPU->socket);
	tipoProceso* proceso;
	recuperarProceso(CPU,&proceso);
	encolar(colaProcesosListos,proceso);																log_debug(logDebug,"Proceso encolado en Ready");
	encolar(colaCPUsListos,CPU);
	return 0;
}

int recuperarProcesoFinalizado(t_cliente* CPU){															log_debug(logDebug,"Entre a recuperarProcesoFinalizado con el cpu %d",CPU->socket);
	tipoProceso* proceso;
	char* cpu=string_itoa(CPU->socket);
	pthread_mutex_lock(&lockProcesosEnEjecucion);
	proceso = dictionary_remove(procesosEnEjecucion,cpu);
	pthread_mutex_unlock(&lockProcesosEnEjecucion);
	free(cpu);
	encolar(colaProcesosFinalizados,proceso);															log_debug(logDebug,"Proceso encolado en Finalizados");
	encolar(colaCPUsListos,CPU);
	return 0;
}

int recuperarProcesoDeCPUSaliente(t_cliente* CPU){														log_debug(logDebug,"Entre a recuperarProcesoCPUSaliente con el cpu %d",CPU->socket);
	tipoProceso* proceso;
	recuperarProceso(CPU,&proceso);
	encolar(colaProcesosListos,proceso);																log_debug(logDebug,"Proceso encolado en Ready");
	free(CPU);
	return 0;
}

int __recibirProceso(t_cliente* CPU,tipoProceso** proceso){					log_debug(logDebug,"Entre a recuperarProceso con el cpu %d",CPU->socket);

	char* cpu=string_itoa(CPU->socket);
	pthread_mutex_lock(&lockProcesosEnEjecucion);
	*proceso = dictionary_get(procesosEnEjecucion,cpu);
	pthread_mutex_unlock(&lockProcesosEnEjecucion);
	free(cpu);
	recibirDeCliente(CPU,&((*proceso)->PCB.identificador),sizeof(int));			log_debug(logDebug,"recibi id %d",(*proceso)->PCB.identificador);
	recibirDeCliente(CPU,&((*proceso)->PCB.segmentoDeCodigo),sizeof(int));		log_debug(logDebug,"recibi segmento codigo %d",(*proceso)->PCB.segmentoDeCodigo);
	recibirDeCliente(CPU,&((*proceso)->PCB.segmentoDeStack),sizeof(int));		log_debug(logDebug,"recibi segmento stack %d",(*proceso)->PCB.segmentoDeStack);
	recibirDeCliente(CPU,&((*proceso)->PCB.cursorDelStack),sizeof(int));		log_debug(logDebug,"recibi cursor stack %d",(*proceso)->PCB.cursorDelStack);
	recibirDeCliente(CPU,&((*proceso)->PCB.indiceDeCodigo),sizeof(int));		log_debug(logDebug,"recibi indice codigo %d",(*proceso)->PCB.indiceDeCodigo);
	recibirDeCliente(CPU,&((*proceso)->PCB.indiceEtiquetas),sizeof(int));		log_debug(logDebug,"recibi indice etiquetas %d",(*proceso)->PCB.indiceEtiquetas);
	recibirDeCliente(CPU,&((*proceso)->PCB.programCounter),sizeof(int));		log_debug(logDebug,"recibi pc %d",(*proceso)->PCB.programCounter);
	recibirDeCliente(CPU,&((*proceso)->PCB.tamanioContextoActual),sizeof(int));	log_debug(logDebug,"recibi tamanio contexto %d",(*proceso)->PCB.tamanioContextoActual);
	recibirDeCliente(CPU,&((*proceso)->PCB.tamanioIndiceEtiquetas),sizeof(int));log_debug(logDebug,"recibi tam indice etiquetas %d",(*proceso)->PCB.tamanioIndiceEtiquetas);log_debug(logDebug,"PCB actualizado");
	return 0;
}


int imprimirTexto(t_cliente* CPU){log_debug(logDebug,"Entre a imprimirTexto con el cpu %d",CPU->socket);
	int tamanioNombre;
	tipoProceso* proceso;
																		log_debug(logDebug,"ImprimirTexto a punto de recibir tamanio");
	recibirDeCliente(CPU,&tamanioNombre,sizeof(tamanioNombre));
	char* texto = malloc(tamanioNombre+1);
	texto[tamanioNombre]='\0';
	recibirDeCliente(CPU,texto,tamanioNombre);log_debug(logDebug,"Se indico imprimir el texto:\"%s\"",texto);
	char* cpu=string_itoa(CPU->socket);
	pthread_mutex_lock(&lockProcesosEnEjecucion);
	proceso = dictionary_get(procesosEnEjecucion,cpu);
	pthread_mutex_unlock(&lockProcesosEnEjecucion);
	free(cpu);
	notificar(proceso,texto);
	encolar(colaCPUsEnUso,CPU);
	return 0;
}

int imprimir(t_cliente* CPU){log_debug(logDebug,"Entre a imprimir con el cpu %d",CPU->socket);
	int codigo=0;
	tipoProceso* proceso;
	char* comando=malloc(sizeof(codigo)+sizeof(int));

	memcpy(comando,&codigo,sizeof(codigo));
	recibirDeCliente(CPU,comando+sizeof(codigo),sizeof(int));
	char* cpu=string_itoa(CPU->socket);
	pthread_mutex_lock(&lockProcesosEnEjecucion);
	proceso = dictionary_get(procesosEnEjecucion,cpu);
	pthread_mutex_unlock(&lockProcesosEnEjecucion);
	free(cpu);
	enviar(proceso->socketPP,comando,sizeof(codigo)+sizeof(int));
	encolar(colaCPUsEnUso,CPU);
	return 0;
}

int wait(t_semaforo* semaforo,t_cliente* CPU){				log_debug(logDebug,"Entre a wait con el cpu %d",CPU->socket);

	int bloquea;

	if(semaforo->contador<1){								log_debug(logDebug,"El wait bloqueo");
		bloquea=1;
		enviarACliente(CPU,&bloquea,sizeof(bloquea));
		tipoProceso* proceso;
		recuperarProceso(CPU,&proceso);
		queue_push(semaforo->bloqueados,proceso);
		encolar(colaCPUsListos,CPU);
	}
	else{													log_debug(logDebug,"El wait no bloqueo");
		bloquea=0;
		enviarACliente(CPU,&bloquea,sizeof(bloquea));
		encolar(colaCPUsEnUso,CPU);
	}
	semaforo->contador--;
		return 0;
}

int post(t_semaforo* semaforo,t_cliente* CPU){log_debug(logDebug,"Entre a post(signal) con el cpu %d",CPU->socket);

	int respuesta = 0;

	enviarACliente(CPU,&respuesta,sizeof(respuesta));
	semaforo->contador++;
	if(semaforo->contador<=0)
		encolar(colaProcesosListos,queue_pop(semaforo->bloqueados));											log_debug(logDebug,"Proceso encolado en Ready tras signal de semaforo");
	encolar(colaCPUsEnUso,CPU);
	return 0;
}

int enviarCompartida(int* variable, t_cliente* CPU){log_debug(logDebug,"Entre a enviarCompartida con el cpu %d",CPU->socket);

	int error=0;

	enviarACliente(CPU,&error,sizeof(error));
	enviarACliente(CPU,variable,sizeof(*variable));
	encolar(colaCPUsEnUso,CPU);
	return 0;
}

int escribirCompartida(int* variable,t_cliente* CPU){log_debug(logDebug,"Entre a escribirCompartida con el cpu %d",CPU->socket);

	int  valor,respuesta=0;

	enviarACliente(CPU,&respuesta,sizeof(respuesta));
	recibirDeCliente(CPU,&valor,sizeof(valor));
	*variable=valor;
	encolar(colaCPUsEnUso,CPU);
	return 0;
}

int realizarIO(tipoDispositivo* dispositivo, t_cliente* CPU){log_debug(logDebug,"Entre a realizarIO con el cpu %d",CPU->socket);

	int respuesta=0,unidadesIO;
	tipoProceso* proceso;
	recibirDeCliente(CPU,&unidadesIO,sizeof(unidadesIO));
	recibirProceso(CPU,&proceso);
	enviarACliente(CPU,&respuesta,sizeof(respuesta));
	tipoProcesoEnIO* procesoEnIO = malloc(sizeof(tipoProcesoEnIO));
	procesoEnIO->proceso=proceso;
	procesoEnIO->cantidadUnidadesDeIO=unidadesIO;
	retirarProcesoDeEjecucion(CPU);
	encolar(dispositivo->colaDeIO,procesoEnIO);				log_debug(logDebug,"Proceso encolado en IO");
	encolar(colaCPUsListos,CPU);

	return 0;
}

int operarElementoCompartido(t_cliente* CPU, int(*operacion)(void*,t_cliente*),int tipoExcepcion,t_dictionary* diccionario){log_debug(logDebug,"Entre a operarElementoCompartido con el cpu %d",CPU->socket);

	int excepcion,tamanioNombre=0;
	void* elemento;

	recibirDeCliente(CPU,&tamanioNombre,sizeof(tamanioNombre));log_debug(logDebug,"Tamanio del nombre del elem. comaprtido : %d",tamanioNombre);
	char* nombre = malloc(tamanioNombre+1);
	nombre[tamanioNombre]='\0';
	recibirDeCliente(CPU,nombre,tamanioNombre);log_debug(logDebug,"Se intentó acceder al elemto comaprtido de nombre : \"%s\"",nombre);
	if(!dictionary_has_key(diccionario,nombre)){
		free(nombre);
		excepcion=tipoExcepcion;
		enviarACliente(CPU,&excepcion,sizeof(excepcion));
	}
	else{
		elemento = dictionary_get(diccionario,nombre);
		free(nombre);
		excepcion=operacion(elemento,CPU);
	}
	return excepcion;
}

//Deprectaed
int operarCompartida(t_cliente* CPU, int (*operacion)(int*,t_cliente*)){log_debug(logDebug,"Entre a operarElementoCompartido con el cpu %d",CPU->socket);
	int *variable,tamanioNombre=0,respuesta,excepcion;
	char* nombre;

	recibirDeCliente(CPU,&tamanioNombre,sizeof(tamanioNombre));
	nombre = malloc(tamanioNombre+1);
	nombre[tamanioNombre]='\0';
	recibirDeCliente(CPU,nombre,tamanioNombre);
	if(!dictionary_has_key(variablesGlobales,nombre)){
		respuesta=excepcion=LEER_COMPARTIDAINEXISTENTE; //puesto leer para que no tire wrng pero esta deprecado, asi que Meh
		enviarACliente(CPU,&respuesta,sizeof(respuesta));
	}
	else{
		variable=dictionary_get(variablesGlobales,nombre);
		excepcion=respuesta=operacion(variable,CPU);
		enviarACliente(CPU,&respuesta,sizeof(respuesta));
		encolar(colaCPUsEnUso,CPU);
	}
	return excepcion;
}

