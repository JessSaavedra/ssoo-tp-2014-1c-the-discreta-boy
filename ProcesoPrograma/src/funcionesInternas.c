/*
 * funcionesInternas.c
 *
 *  Created on: 10/05/2014
 *      Author: utnso
 */
#include "stdPP.h"

void enviar(int socket, char* mensaje,int tamanio){
	int bytesEnviados;
	int bytesReenviados;


	bytesEnviados=send(socket,mensaje,tamanio,0);

	while (bytesEnviados < tamanio) {

	 bytesReenviados=send(socket,mensaje+bytesEnviados,tamanio-bytesEnviados,0);
	 bytesEnviados += bytesReenviados;

	}

}

int cargarConfiguraciones(void){
	char* pathConfig=getenv("ANSISOP_CONFIG");
	t_config* configuracion = config_create(pathConfig);

	if (config_has_property(configuracion,"Puerto") && config_has_property(configuracion,"IP")){
		puerto=config_get_string_value(configuracion,"Puerto");
		ip=config_get_string_value(configuracion,"IP");
		return 0;
	} else {
		return CONFIG_INVALIDA;
	}

}

void liberarConfiguraciones(void){
	free(puerto);
	free(ip);
}

int newSocketServidor(void){

	memset(&criteriosConfiguracion,0,sizeof(criteriosConfiguracion));

	criteriosConfiguracion.ai_family=AF_UNSPEC;
	criteriosConfiguracion.ai_socktype=SOCK_STREAM;

	getaddrinfo(ip,puerto,&criteriosConfiguracion,&datosSocketKernel);

	int socketServidor = socket(datosSocketKernel->ai_family,datosSocketKernel->ai_socktype,datosSocketKernel->ai_protocol );
																																	log_debug(logDebug,"Creado el socket para conexion con el kernel");
	return socketServidor;
}

int enviarScript(char* rutaDeScript){
	FILE* script;
	int i,tamanioPaquete,tamanioArchivo;
	char aux;
	script= fopen(rutaDeScript,"r");
	if (script==NULL) {
		fprintf(stderr,"Hubo un error con la apertura del archivo");
		return FALLOENLECTURA;
	}
	for(tamanioArchivo=0;!feof(script);tamanioArchivo++){
			fgetc(script);
		}

	char archivoAEnviar[tamanioArchivo+1];

	rewind(script);
	for(i=0;!feof(script);i++){
		aux=fgetc(script);
		archivoAEnviar[i]=aux;
	}
	fclose(script);

	archivoAEnviar[i-1]='\0';

	tamanioPaquete=i;


	char* paqueteAEnviar=malloc(tamanioPaquete+sizeof(int));

	memcpy(paqueteAEnviar,&tamanioPaquete,sizeof(int));
	memcpy(paqueteAEnviar+sizeof(int),&archivoAEnviar,tamanioPaquete);


	tamanioPaquete=i+sizeof(int);

	enviar(socketKernel,paqueteAEnviar,tamanioPaquete);

	return 0;

}

void* comandoPara(int codigoDeOperacion){
	if(codigoDeOperacion)
		return imprimirTexto;
	else
		return imprimir;
}

void recibirHeader(t_header* headerDestino){
	int ultimaRecepcion;
	recv(socketKernel,&ultimaRecepcion,sizeof(int),MSG_WAITALL);
	headerDestino->codigoOperacion=ultimaRecepcion;	log_debug(logDebug,"Recibido codigo de Operacion %d",ultimaRecepcion);

	if(ultimaRecepcion==IMPRIMIRTEXTO){
	recv(socketKernel,&ultimaRecepcion,sizeof(int),MSG_WAITALL);
	headerDestino->tamanioTransmitido=ultimaRecepcion;	log_debug(logDebug,"Recibido tamanio %d",ultimaRecepcion);
	} else headerDestino->tamanioTransmitido=sizeof(int);

	}

void recibirDatosCon(int unTamanio,void* bloqueDeDestino){
	recv(socketKernel,bloqueDeDestino,unTamanio,MSG_WAITALL);

}

void imprimir(void* unosDatos){
	int aux = *(int*)unosDatos;
	printf("%d \n",aux);
}

void imprimirTexto(void* unosDatos){
	printf("%s \n",(char*)unosDatos);
}

