

#include "kernel.h"

void crearProceso(int pid){
	char* crearProceso=malloc(2*TAMANIO_INT);
	int codigoOperacion = 3;
	memcpy(crearProceso,&codigoOperacion,TAMANIO_INT);
	memcpy(crearProceso+TAMANIO_INT,&pid,TAMANIO_INT);
	enviar(socketUMV,crearProceso,2*TAMANIO_INT);
	validarCodigoDeError();
	free(crearProceso);
}

char* generarStreamCreacionSegmento(int pid,int tamanioSegmento){
	char* stream = malloc(3*TAMANIO_INT);
	int codigoDeOperacion = 4;
	memcpy(stream,&codigoDeOperacion,TAMANIO_INT);
	memcpy(stream+TAMANIO_INT,&pid,TAMANIO_INT);
	memcpy(stream+2*TAMANIO_INT,&tamanioSegmento,TAMANIO_INT);
	return stream;
}

int crearSegmento(char* pedidoCreacionSegmentoCodigo){
	if(segmentosCreadosFlag==0){
	int direccion;
	enviar(socketUMV,pedidoCreacionSegmentoCodigo,3*TAMANIO_INT);
	recv(socketUMV,&direccion,TAMANIO_INT,MSG_WAITALL);
	if(direccion==-2)
		segmentosCreadosFlag=1;
	return direccion;
	}
	else return 0;
}

char* generarStreamEscritura(int segmento,int longitud,void* datosAGuardar){

		int codigoDeOperacion=1;
		int offset = 0;
		char* stream=malloc (4*TAMANIO_INT+longitud);
		memcpy(stream,&codigoDeOperacion,TAMANIO_INT);
		memcpy(stream+TAMANIO_INT,&segmento,TAMANIO_INT);
		memcpy(stream+2*TAMANIO_INT,&offset,TAMANIO_INT);
		memcpy(stream+3*TAMANIO_INT,&longitud,TAMANIO_INT);
		memcpy(stream+4*TAMANIO_INT,datosAGuardar,longitud);
		return stream;
	}


void escribirEnMemoria(char* contenidoAGuardar,int tamanioAEnviar){
	if(segmentosCreadosFlag==0){
		if (tamanioAEnviar > 4*TAMANIO_INT){
		enviar(socketUMV, contenidoAGuardar,tamanioAEnviar);
		validarCodigoDeError();
		}
	}
}








void imprimirProcesos(t_colaSincronizada* colaProcesosNuevos){
	system("clear");
	printf("-----------------PROCESOS NUEVOS-------------------\n");
	printf("\n");
	pthread_mutex_lock(colaProcesosNuevos->mutex);
	list_iterate(colaProcesosNuevos->cola,imprimirProceso);
	pthread_mutex_unlock(colaProcesosNuevos->mutex);
	printf("\n");
	printf("---------------------------------------------------\n");


}

void imprimirProceso(void* proceso){

	tipoProceso* procesoPosta=(tipoProceso*)proceso;
	printf("Proceso:%d  Peso:%d\n",procesoPosta->PCB.identificador,procesoPosta->peso);

}

tipoProceso* crearSegmentosDeProceso(int pid,char* codigo,t_medatada_program* metadata,int tamanioCodigo,tipoProceso* proceso){

	tipoPCB PCB;

	PCB.identificador= pid;


	//----------------------- Creacion de segmento de codigo y guardado -----------------------------//

	char* pedidoCreacionSegmentoCodigo = generarStreamCreacionSegmento(PCB.identificador,tamanioCodigo);
	PCB.segmentoDeCodigo = crearSegmento(pedidoCreacionSegmentoCodigo);

	char* codigoParaMemoria = generarStreamEscritura(PCB.segmentoDeCodigo,tamanioCodigo,codigo);
	escribirEnMemoria(codigoParaMemoria,4*TAMANIO_INT+tamanioCodigo);

	//---------------------- Creacion del indice de etiquetas y guardado ----------------------------//

	PCB.tamanioIndiceEtiquetas = metadata->etiquetas_size;

	char* pedidoCreacionSegmentoIndiceEtiquetas = generarStreamCreacionSegmento(PCB.identificador,PCB.tamanioIndiceEtiquetas);
	PCB.indiceEtiquetas = crearSegmento(pedidoCreacionSegmentoIndiceEtiquetas);

	char* indiceDeEtiquetasParaMemoria= generarStreamEscritura(PCB.indiceEtiquetas,PCB.tamanioIndiceEtiquetas,metadata->etiquetas);
	escribirEnMemoria(indiceDeEtiquetasParaMemoria,4*TAMANIO_INT+PCB.tamanioIndiceEtiquetas);

	//---------------------- Creacion del segmento de stack ----------------------------//

	char* pedidoCreacionSegmentoStack = generarStreamCreacionSegmento(PCB.identificador,tamanioStack);
	PCB.segmentoDeStack = crearSegmento(pedidoCreacionSegmentoStack);
	PCB.cursorDelStack = 0;

	//---------------------- Creacion del indice de codigo y guardado ----------------------------//

	int tamanioIndiceDeCodigo = 2*TAMANIO_INT*(metadata->instrucciones_size);

	char* pedidoCreacionIndiceCodigo = generarStreamCreacionSegmento(PCB.identificador,tamanioIndiceDeCodigo);
	PCB.indiceDeCodigo = crearSegmento(pedidoCreacionIndiceCodigo);

	char* indiceDeCodigoParaMemoria = generarStreamEscritura(PCB.indiceDeCodigo,tamanioIndiceDeCodigo,metadata->instrucciones_serializado);
	escribirEnMemoria(indiceDeCodigoParaMemoria,4*TAMANIO_INT+tamanioIndiceDeCodigo);


	PCB.tamanioContextoActual=0;
	PCB.programCounter = metadata->instruccion_inicio;

	proceso->PCB=PCB;

	free(pedidoCreacionIndiceCodigo);
	free(pedidoCreacionSegmentoCodigo);
	free(pedidoCreacionSegmentoIndiceEtiquetas);
	free(pedidoCreacionSegmentoStack);
	free(codigoParaMemoria);
	free(indiceDeEtiquetasParaMemoria);
	free(indiceDeCodigoParaMemoria);

	return proceso;
}

void list_add_order_by(t_list* list, void* data, int(*isBigger)(void*,void*)){

	int index=0;

	if(list_is_empty(list)){

		list_add_in_index(list,0,data);
		return;
	}
	while((index<list_size(list)))
		if ((*isBigger)((void*)list_get(list,index), data))
			index++;
		else
			break;
	list_add_in_index(list,index,data);
	return;
}

int esDeMenorPeso(void* proceso1 , void* proceso2){

	tipoProceso* procesoOne = (tipoProceso*) proceso1;
	tipoProceso* procesoTwo = (tipoProceso*) proceso2;

	return procesoOne->peso <= procesoTwo->peso;

}

void terminarProceso(tipoProceso* proceso){
	int codigoDeOperacion=5;
	char* procesoTerminado=malloc(2*TAMANIO_INT);
	memcpy(procesoTerminado,&codigoDeOperacion,TAMANIO_INT);
	memcpy(procesoTerminado+TAMANIO_INT,&proceso->PCB.identificador,TAMANIO_INT);
	send(socketUMV,procesoTerminado,2*TAMANIO_INT,0);
	validarCodigoDeError();
	free(procesoTerminado);
	codigoDeOperacion=2;
	procesoTerminado=malloc(TAMANIO_INT);
	memcpy(procesoTerminado,&codigoDeOperacion,TAMANIO_INT);
	send(proceso->socketPP,procesoTerminado,TAMANIO_INT,0);
	free(procesoTerminado);
	free(proceso);

}

void validarCodigoDeError(){
	int codigoDeError;

	recv(socketUMV,&codigoDeError,TAMANIO_INT,MSG_WAITALL);



}




