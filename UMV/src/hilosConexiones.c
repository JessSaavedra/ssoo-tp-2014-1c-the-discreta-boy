/*
 * hilosConexiones.c
 *
 *  Created on: 26/05/2014
 *      Author: utnso
 */

#include "stdUMV.h"

#define LECTURA 0
#define ESCRITURA 1
#define CAMBIARPROCESO 2
#define CREARPROCESO 3
#define CREARSEGMENTO 4
#define DESTRUIRPROCESO 5
#define PROCESONOASIGNADO -6

#define KERNEL 10
#define CPU 11

t_log* logConexiones;//Debug


void manejarPedidos(t_cliente* cliente){													log_trace(logConexiones,"Comunicacion iniciada");

	int codigoOperacion,baseSegmento, desplazamiento,tamanio,PID,error,bytesEnviados, tamanioAEnviar;
	char* datos,*respuesta;

	recibirDeCliente(cliente,&codigoOperacion,sizeof(codigoOperacion));	log_trace(logConexiones,"Se recibio COP con el valor %d, errno: %d, el cliente esta %s",codigoOperacion,errno,cliente->estaVivo?"vivo":"muerto");

	while(cliente->estaVivo){
		sleep(retardo);
		tamanioAEnviar = sizeof(error);
		respuesta= (char*) malloc(tamanioAEnviar);
		switch(codigoOperacion){
			case LECTURA:																	log_trace(logConexiones,"Se indico lectura");
				recibirDeCliente(cliente,&baseSegmento,sizeof(baseSegmento));				log_trace(logConexiones,"Error en base %d",errno);
				recibirDeCliente(cliente,&desplazamiento,sizeof(desplazamiento));			log_trace(logConexiones,"Error en offset %d",errno);
				recibirDeCliente(cliente,&tamanio,sizeof(tamanio));							log_trace(logConexiones,"Error en tamanio %d",errno);log_trace(logConexiones,"A punto de leerDe: %d %d %d %d ",PID,baseSegmento,desplazamiento,tamanio);
				error = leerMemoria(PID,baseSegmento,desplazamiento,tamanio,(void**)&datos);if(tamanio==4) log_trace(logConexiones,"Se leyo: %d",*(int*)datos);
				if(!error){
					tamanioAEnviar+=tamanio;
					respuesta= (char*) realloc((void*)respuesta,tamanioAEnviar);
					memcpy((void*)respuesta+sizeof(error),(void*)datos,tamanio);			log_trace(logConexiones,"Se envio al CPU datos");
				}
				free(datos);
				break;
			case ESCRITURA:																	log_trace(logConexiones,"Se indico escritura");
				recibirDeCliente(cliente,&baseSegmento,sizeof(baseSegmento));				log_trace(logConexiones,"Error en base %d,base %d",errno,baseSegmento);
				recibirDeCliente(cliente,&desplazamiento,sizeof(desplazamiento));			log_trace(logConexiones,"Error en offset %d offset %d",errno,desplazamiento);
				recibirDeCliente(cliente,&tamanio,sizeof(tamanio));							log_trace(logConexiones,"Error en tamanio %d tamanio %d",errno,tamanio);
				datos=(char*)malloc(tamanio);
				recibirDeCliente(cliente,datos,tamanio);									log_trace(logConexiones,"Error en datos %d",errno);log_trace(logConexiones,"A punto de escribirEn: %d %d %d %d ",PID,baseSegmento,desplazamiento,tamanio);
				if(cliente->estaVivo) error = escribirMemoria(PID,baseSegmento,desplazamiento,tamanio,(void*)datos); if(tamanio==4) log_trace(logConexiones,"Se escribio: %d",*(int*)datos);
				free(datos);
				break;
			case CAMBIARPROCESO:															log_trace(logConexiones,"Se indico cambiar proceso");
				recibirDeCliente(cliente,&PID,sizeof(PID));									log_trace(logConexiones,"ProcesoActual fue seteado a %d",PID);
				error=0;
				break;
			case CREARPROCESO:																log_trace(logConexiones,"Se indico crear proceso");
				recibirDeCliente(cliente,&PID,sizeof(PID));									log_trace(logConexiones,"Error en pid %d,pid=%d",errno,PID);
				if(cliente->estaVivo) error = crearTablaProceso(PID);
				break;
			case CREARSEGMENTO:																log_trace(logConexiones,"Se indico crear segmento");
				recibirDeCliente(cliente,&PID,sizeof(PID));									log_trace(logConexiones,"Error en pid %d,pid=%d",errno,PID);
				recibirDeCliente(cliente,&tamanio,sizeof(tamanio));							log_trace(logConexiones,"Error en tamanio %d,tamanio=%d",errno,tamanio);
				if(cliente->estaVivo) error = crearSegmento(PID,tamanio);					log_trace(logConexiones,"Cree segmento con base %d",error);
				break;
			case DESTRUIRPROCESO:															log_trace(logConexiones,"Se indico destruir proceso");
				recibirDeCliente(cliente,&PID,sizeof(PID));									log_trace(logConexiones,"Error en pid %d,pid=%d",errno,PID);
				if(cliente->estaVivo) error = destruirProceso(PID);
				break;
		}																					log_debug(logDebug,"Sali del switch de manejarpedidos");
		memcpy((void*)respuesta,(void*)&error,sizeof(error));
		bytesEnviados=enviarACliente(cliente,respuesta,tamanioAEnviar);								log_trace(logConexiones,"Respondi codigo de error: %d, se enviaron %d bytes, errno: %d",*(int*)respuesta,bytesEnviados,errno);
		free(respuesta);
		recibirDeCliente(cliente,&codigoOperacion,sizeof(codigoOperacion));		log_trace(logConexiones,"Se recibio COP con el valor %d, errno: %d, el cliente esta %s", codigoOperacion,errno,cliente->estaVivo?"vivo":"muerto");
	}
																							log_trace(logConexiones, "Sali de manejarPedidos");
	free(cliente);
	pthread_exit(EXIT_SUCCESS);
	return;
}

void handshake(t_cliente* cliente){

	int tipo;

	recibirDeCliente(cliente,&tipo,sizeof(tipo));								log_trace(logConexiones,"El tipo es %d",tipo);
	pthread_t nuevoHilo;
	pthread_create(&nuevoHilo,NULL,(void* (*)(void*)) manejarPedidos,(void*)cliente);
	pthread_detach(nuevoHilo);
	return;
}

void administrarConexiones(void){
	int socketEscucha = crearSocketEscuchaSobre(puertoUMV,10);

	while (1){
		int socketCliente = accept(socketEscucha,NULL,NULL);							log_trace(logConexiones,"Aceptada conexion");
		t_cliente* cliente = newCliente(socketCliente,enviar,recv,true);
		handshake(cliente);
	}
	return;
}
