

#include "headerCompleto.h"

extern void* stack;


char* recibirPCB(){

	int bytesRecibidos;

	int codigoDeVida;
	recv(socketKernel,&codigoDeVida,tamanioInt,MSG_WAITALL);
	enviar(socketKernel,(char*)&codigoDeVida,tamanioInt);

	int* PCBRecibido=malloc(11*tamanioInt);
	bytesRecibidos=recv(socketKernel, PCBRecibido,11*tamanioInt, MSG_WAITALL); log_debug(logDebug,"Recv devolvió %d %d",bytesRecibidos,errno);
	int i;
	for(i=0;i<9;i++){
		log_debug(logDebug,"Campo %d: %d",i,PCBRecibido[i]);
	}
	return (char*) PCBRecibido;
}

char* generarStreamEscritura(int segmento,int offset,int longitud,void* datosAGuardar){

		int codigoDeOperacion=1;
		char* stream=malloc (4*tamanioInt+longitud);
		memcpy(stream,&codigoDeOperacion,tamanioInt);
		memcpy(stream+tamanioInt,&segmento,tamanioInt);
		memcpy(stream+2*tamanioInt,&offset,tamanioInt);
		memcpy(stream+3*tamanioInt,&longitud,tamanioInt);
		memcpy(stream+4*tamanioInt,datosAGuardar,longitud);
		return stream;
	}

char* generarStreamLectura(int segmento,int offset,int longitud){
			char* stream=malloc(4*tamanioInt);
			int codigoDeOperacion=0;
			memcpy(stream,&codigoDeOperacion,tamanioInt);
			memcpy(stream+tamanioInt,&segmento,tamanioInt);
			memcpy(stream+2*tamanioInt,&offset,tamanioInt);
			memcpy(stream+3*tamanioInt,&longitud,tamanioInt);
			return stream;
	}


void escribirEnMemoria2(char* contenidoAGuardar,int tamanioAEnviar){

		int offset;
		memcpy(&offset,contenidoAGuardar+2*tamanioInt,tamanioInt);

		int longitud;
		memcpy(&longitud,contenidoAGuardar+3*tamanioInt,tamanioInt);

		memcpy(stack+offset,contenidoAGuardar+4*tamanioInt,longitud);


	}

void escribirEnMemoria(char* contenidoAGuardar,int tamanioAEnviar){
	enviar(socketUMV, contenidoAGuardar,tamanioAEnviar);
	validarCodigoDeError(socketUMV);
}
void escribirEnStack(char* contenidoAGuardar,int tamanioAEnviar){
	enviar(socketUMV, contenidoAGuardar,tamanioAEnviar);
	validarStackOverflow();
}

char* pedirDeMemoriaContexto(char* pedido,int tamanioARecibir){
	enviar(socketUMV, pedido,4*tamanioInt);
	validarStackOverflow();
	void* valorObtenido=malloc(tamanioARecibir);
	if (flagError==0)
		recv(socketUMV,valorObtenido,tamanioARecibir,MSG_WAITALL);
	return valorObtenido;

}

void* pedirDeMemoria2(char* pedido,int tamanio){

	int offset;
	memcpy(&offset,pedido+2*tamanioInt,tamanioInt);
	int longitud;
	memcpy(&longitud,pedido+3*tamanioInt,tamanioInt);
	void* valorObtenido=malloc(longitud);
	memcpy(valorObtenido,stack+offset,longitud);
	return valorObtenido;
	}

void* pedirDeMemoria(char* pedido,int tamanioARecibir){
		enviar(socketUMV, pedido,4*tamanioInt);
		validarCodigoDeError(socketUMV);
		void* valorObtenido=malloc(tamanioARecibir);
		if(flagError==0)
			recv(socketUMV,valorObtenido,tamanioARecibir,MSG_WAITALL);
		return valorObtenido;


}

void recuperarContextoActual(){

	dictionary_clean_and_destroy_elements(diccionarioDeVariables,free);


	char* pedidoDeStack=generarStreamLectura(PCB.segmentoDeStack,PCB.cursorDelStack,5*PCB.tamanioContextoActual);
	char* contextoActual=pedirDeMemoriaContexto(pedidoDeStack,5*PCB.tamanioContextoActual);

	int desplazamiento=0;


	while(flagError==0 && desplazamiento != 5*PCB.tamanioContextoActual){

		char* nombreVariable=malloc(2);
		int* direccion=malloc(tamanioInt);


		memcpy(nombreVariable, contextoActual + desplazamiento, 1);
		memcpy(nombreVariable+1,"\0",1);

		*direccion=PCB.cursorDelStack+desplazamiento;


		dictionary_put(diccionarioDeVariables,nombreVariable,direccion);




		desplazamiento +=5;

		free(nombreVariable);


	}
	free(contextoActual);
	free(pedidoDeStack);
}

void avisarAlKernel(int codigoOperacion,char* identificador_semaforo){
		int largoIdentificador=strlen(identificador_semaforo);
		char* streamSemaforo=malloc(2*tamanioInt+largoIdentificador);
		memcpy(streamSemaforo,&codigoOperacion,tamanioInt);
		memcpy(streamSemaforo+tamanioInt,&largoIdentificador,tamanioInt);
		memcpy(streamSemaforo+2*tamanioInt,identificador_semaforo,largoIdentificador);
		enviar(socketKernel,streamSemaforo,2*tamanioInt+largoIdentificador);
		free(streamSemaforo);
}

void enviarPCBActualizado(tipoPCB PCB,int codigoDeOperacion){
char* PCBAEnviar=malloc(10*tamanioInt);
			memcpy(PCBAEnviar,&codigoDeOperacion,tamanioInt);
			memcpy(PCBAEnviar+tamanioInt,&PCB.identificador,tamanioInt);
			memcpy(PCBAEnviar+2*tamanioInt,&PCB.segmentoDeCodigo,tamanioInt);
			memcpy(PCBAEnviar+3*tamanioInt,&PCB.segmentoDeStack,tamanioInt);
			memcpy(PCBAEnviar+4*tamanioInt,&PCB.cursorDelStack,tamanioInt);
			memcpy(PCBAEnviar+5*tamanioInt,&PCB.indiceDeCodigo,tamanioInt);
			memcpy(PCBAEnviar+6*tamanioInt,&PCB.indiceEtiquetas,tamanioInt);
			memcpy(PCBAEnviar+7*tamanioInt,&PCB.programCounter,tamanioInt);
			memcpy(PCBAEnviar+8*tamanioInt,&PCB.tamanioContextoActual,tamanioInt);
			memcpy(PCBAEnviar+9*tamanioInt,&PCB.tamanioIndiceEtiquetas,tamanioInt);
			log_debug(logDebug,"envie operacion %d",codigoDeOperacion);
			enviar(socketKernel,PCBAEnviar,10*tamanioInt);
			free(PCBAEnviar);
}

char* agregarBarraCero(char* etiqueta){
	int i;
	char letra= *etiqueta;
	for(i=0;letra != '\n' ;i++){
		letra=*(etiqueta+i);
	}
	char* etiqueta2=malloc(i);
	memcpy(etiqueta2,etiqueta,i-1);
	memcpy(etiqueta2+i-1,"\0",1);
	return etiqueta2;
}

void milisleep(unsigned int milisegundos){
	usleep(1000*milisegundos);
	return;
}

char* generarPCB(){
	char* PCBAEnviar=malloc(9*tamanioInt);
	memcpy(PCBAEnviar,&PCB.identificador,tamanioInt);
	memcpy(PCBAEnviar+1*tamanioInt,&PCB.segmentoDeCodigo,tamanioInt);
	memcpy(PCBAEnviar+2*tamanioInt,&PCB.segmentoDeStack,tamanioInt);
	memcpy(PCBAEnviar+3*tamanioInt,&PCB.cursorDelStack,tamanioInt);
	memcpy(PCBAEnviar+4*tamanioInt,&PCB.indiceDeCodigo,tamanioInt);
	memcpy(PCBAEnviar+5*tamanioInt,&PCB.indiceEtiquetas,tamanioInt);
	memcpy(PCBAEnviar+6*tamanioInt,&PCB.programCounter,tamanioInt);
	memcpy(PCBAEnviar+7*tamanioInt,&PCB.tamanioContextoActual,tamanioInt);
	memcpy(PCBAEnviar+8*tamanioInt,&PCB.tamanioIndiceEtiquetas,tamanioInt);
	return PCBAEnviar;
}
