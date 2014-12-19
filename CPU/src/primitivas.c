#include "headerCompleto.h"



//------------------------------ Primitivas ------------------------------------//

int definirVariable(char identificador_variable){
	int* direccion=malloc(tamanioInt);
	char* nombreVariable=string_repeat(identificador_variable,1);
	char* contenidoAGuardar=malloc(5);
	memcpy(contenidoAGuardar,nombreVariable,1);
	*direccion=PCB.cursorDelStack+5*PCB.tamanioContextoActual;
	char* variableAGuardar=generarStreamEscritura(PCB.segmentoDeStack,*direccion,5,contenidoAGuardar);
	escribirEnStack(variableAGuardar,4*tamanioInt+5);
	dictionary_put(diccionarioDeVariables,nombreVariable,direccion);
	PCB.tamanioContextoActual++;
	log_debug(logDebug,"Definí variable %c",identificador_variable);
	free(variableAGuardar);
	free(nombreVariable);
	return *direccion;
}

//----------------------------------------------------------------------------//

int obtenerPosicionVariable(char identificador_variable){
	char* nombreVariable=string_repeat(identificador_variable,1);
	int* posicionVariable;
	posicionVariable = (int*)dictionary_get(diccionarioDeVariables,nombreVariable);

	log_debug(logDebug,"Obtuve la posición de la variable %c, que es %d",identificador_variable,*(posicionVariable));
	free(nombreVariable);

	if (posicionVariable == NULL)
		return -1;
	else
		return *(posicionVariable);

}

//----------------------------------------------------------------------------//

int dereferenciar(int direccion_variable){
	char* variableALeer=generarStreamLectura(PCB.segmentoDeStack,direccion_variable+1,4);
	int* contenidoVariable=pedirDeMemoria(variableALeer,4);
	log_debug(logDebug,"Dereferencié la dirección %d",direccion_variable);
	free(variableALeer);
	return *(contenidoVariable);
}

//----------------------------------------------------------------------------//

void asignar(int direccion_variable, int valor ){
	if(flagError==0){
	char* variableAGuardar=generarStreamEscritura(PCB.segmentoDeStack,direccion_variable+1,4,&valor);
	escribirEnMemoria(variableAGuardar,4*tamanioInt+4);
	log_debug(logDebug,"Asigné el valor %d a la variable ubicada en la dirección %d",valor,direccion_variable);
	free (variableAGuardar);
	}
}

//----------------------------------------------------------------------------//

int obtenerValorCompartida(char* variable){log_debug(logDebug,"Entré a obtenerValorCompartida");fflush(stdout);
	int codigoDeOperacion = 11;
	int valorVariable;
	int tamanioNombre = strlen(variable);
	char* streamObtenerValor = malloc(2*tamanioInt+tamanioNombre);
	memcpy(streamObtenerValor,&codigoDeOperacion,tamanioInt);
	memcpy(streamObtenerValor+tamanioInt,&tamanioNombre,tamanioInt);
	memcpy(streamObtenerValor+2*tamanioInt,variable,tamanioNombre);
	enviar(socketKernel,streamObtenerValor,2*tamanioInt+tamanioNombre);
	validarCodigoDeError(socketKernel);
	recv(socketKernel,&valorVariable,tamanioInt,MSG_WAITALL);
	free(streamObtenerValor);
	log_debug(logDebug,"Obtuve el valor de la variable compartida %s",variable);
	return valorVariable;
}

//----------------------------------------------------------------------------//

int asignarValorCompartida(char* variable, int valor){log_debug(logDebug,"Entré a la primitiva asignarCompartida");fflush(stdout);
	int codigoDeOperacion = 12;
	int tamanioNombre = strlen(variable);
	char* streamAsignarValor = malloc(3*tamanioInt+tamanioNombre);
	memcpy(streamAsignarValor,&codigoDeOperacion,tamanioInt);
	memcpy(streamAsignarValor+tamanioInt,&tamanioNombre,tamanioInt);
	memcpy(streamAsignarValor+2*tamanioInt,variable,tamanioNombre);
	memcpy(streamAsignarValor+2*tamanioInt+tamanioNombre,&valor,tamanioInt);
	enviar(socketKernel,streamAsignarValor,3*tamanioInt+tamanioNombre);
	validarCodigoDeError(socketKernel);
	log_debug(logDebug,"Asigné el valor %d a la variable %s\n",valor,variable);
	return valor;
}

//----------------------------------------------------------------------------//

void irAlLabel(char* etiqueta){
	int direccionEtiqueta;
	direccionEtiqueta = metadata_buscar_etiqueta(etiqueta,indiceDeEtiquetas,PCB.tamanioIndiceEtiquetas);
	log_debug(logDebug,"Fui al label %s y obtuve el número de su primera instrucción ejecutable",etiqueta);
	log_debug(logDebug,"%d",direccionEtiqueta);
	PCB.programCounter=direccionEtiqueta;
}

//----------------------------------------------------------------------------//

void llamarSinRetorno(char* etiqueta){
	char* datosAGuardar=malloc(2*tamanioInt);
	memcpy(datosAGuardar,&PCB.cursorDelStack,tamanioInt);
	memcpy(datosAGuardar+tamanioInt,&(PCB.programCounter),tamanioInt);
	dictionary_clean_and_destroy_elements(diccionarioDeVariables,free);
	char* direccionDeRetorno=generarStreamEscritura(PCB.segmentoDeStack,PCB.cursorDelStack+5*PCB.tamanioContextoActual,2*tamanioInt,datosAGuardar);
	escribirEnStack(direccionDeRetorno,6*tamanioInt);
	PCB.programCounter= metadata_buscar_etiqueta(etiqueta,indiceDeEtiquetas,PCB.tamanioIndiceEtiquetas);
	PCB.cursorDelStack+=(PCB.tamanioContextoActual*5+2*tamanioInt);
	PCB.tamanioContextoActual=0;
	log_debug(logDebug,"Llamé sin retorno %s",etiqueta);
	free(direccionDeRetorno);
	free(datosAGuardar);
}

//----------------------------------------------------------------------------//

void llamarConRetorno(char* etiqueta, int donde_retornar){
	char* datosAGuardar=malloc(3*tamanioInt);
	memcpy(datosAGuardar,&PCB.cursorDelStack,tamanioInt);
	memcpy(datosAGuardar+tamanioInt,&(PCB.programCounter),tamanioInt);
	memcpy(datosAGuardar+2*tamanioInt,&donde_retornar,tamanioInt);
	dictionary_clean_and_destroy_elements(diccionarioDeVariables,free);
	char* direccionDeRetorno=generarStreamEscritura(PCB.segmentoDeStack,PCB.cursorDelStack+5*PCB.tamanioContextoActual,3*tamanioInt,datosAGuardar);
	escribirEnStack(direccionDeRetorno,7*tamanioInt);
	PCB.programCounter=metadata_buscar_etiqueta(etiqueta,indiceDeEtiquetas,PCB.tamanioIndiceEtiquetas);
	PCB.cursorDelStack+=(PCB.tamanioContextoActual*5+3*tamanioInt);
	PCB.tamanioContextoActual=0;
	log_debug(logDebug,"Llamé con retorno %sh. Dirección de retorno: %d",etiqueta,donde_retornar);
	log_debug(logDebug,"%d",PCB.programCounter);
	log_debug(logDebug,"%d",PCB.cursorDelStack);
	free(direccionDeRetorno);
	free(datosAGuardar);

}



//----------------------------------------------------------------------------//

 void finalizar(void){
	if (PCB.cursorDelStack==0
			){
		finDePrograma=-1;

	}
	else {
		dictionary_destroy(diccionarioDeVariables);
		diccionarioDeVariables=dictionary_create();

		int ultimoContexto = PCB.cursorDelStack-2*tamanioInt;

		char* pedidoRecuperarContexto=generarStreamLectura(PCB.segmentoDeStack,ultimoContexto,2*tamanioInt);
		char* streamContexto=pedirDeMemoria(pedidoRecuperarContexto,2*tamanioInt);


		memcpy(&PCB.cursorDelStack,streamContexto,tamanioInt);
		memcpy(&PCB.programCounter,streamContexto+tamanioInt,tamanioInt);

		PCB.tamanioContextoActual=(ultimoContexto-PCB.cursorDelStack)/5;

		recuperarContextoActual();


		free(streamContexto);
		free(pedidoRecuperarContexto);

		log_debug(logDebug,"Finalicé, cambié el contexto de ejecución actual para volver al anterior");

	}


}

//----------------------------------------------------------------------------//

void retornar(int retorno){

	int direccionDeRetorno;
	int ultimoContexto = PCB.cursorDelStack-3*tamanioInt;


	char* pedidoRecuperarContexto=generarStreamLectura(PCB.segmentoDeStack,ultimoContexto,3*tamanioInt);
	char* streamContexto=pedirDeMemoria(pedidoRecuperarContexto,3*tamanioInt);



	memcpy(&PCB.cursorDelStack,streamContexto,tamanioInt);
	memcpy(&PCB.programCounter,streamContexto+tamanioInt,tamanioInt);
	memcpy(&direccionDeRetorno,streamContexto+2*tamanioInt,tamanioInt);


	PCB.tamanioContextoActual=(ultimoContexto-PCB.cursorDelStack)/5;

	char* retornarElValor=generarStreamEscritura(PCB.segmentoDeStack,direccionDeRetorno+1,4,&retorno);
	escribirEnMemoria(retornarElValor,4*tamanioInt+4);
	recuperarContextoActual();
	log_debug(logDebug,"Retorné, cambié el contexto de ejecución actual para volver al anterior. Dirección de retorno: %d",direccionDeRetorno);
	free( streamContexto);
	free(pedidoRecuperarContexto);
	free( retornarElValor);

}

//----------------------------------------------------------------------------//

void imprimir(int valor_mostrar){
	if(flagError==0){
	int codigoDeOperacion=16;
	char* streamImprimir=malloc(2*tamanioInt);
	memcpy(streamImprimir,&codigoDeOperacion,tamanioInt);
	memcpy(streamImprimir+tamanioInt,&valor_mostrar,tamanioInt);
	enviar(socketKernel,streamImprimir,2*tamanioInt);
	free(streamImprimir);
	log_debug(logDebug,"Imprimí el valor %d",valor_mostrar);

	}
}

//----------------------------------------------------------------------------//

void imprimirTexto(char* texto){
	avisarAlKernel(17,texto);
	log_debug(logDebug,"Imprimí el texto %s",texto);
	return ;
}

//----------------------------------------------------------------------------//

void entradaSalida(char* dispositivo, int tiempo){

	int codigoDeOperacion=15;
	int largoDispositivo=strlen(dispositivo);
	char* streamEntradaSalida=malloc(12*tamanioInt+largoDispositivo);
	memcpy(streamEntradaSalida,&codigoDeOperacion,tamanioInt);
	memcpy(streamEntradaSalida+tamanioInt,&largoDispositivo,tamanioInt);
	memcpy(streamEntradaSalida+2*tamanioInt,dispositivo,largoDispositivo);
	memcpy(streamEntradaSalida+2*tamanioInt+largoDispositivo,&tiempo,tamanioInt);
	char* PCBStream=generarPCB();
	memcpy(streamEntradaSalida+3*tamanioInt+largoDispositivo,PCBStream,9*tamanioInt);
	enviar(socketKernel,streamEntradaSalida,12*tamanioInt+largoDispositivo);
	int error=validarCodigoDeError(socketKernel);
	if(error >= 0){
		flagBloqueo=-1;
	}
	free(streamEntradaSalida);
	log_debug(logDebug,"Informé al Kernel que el programa actual pretende usar el dispositivo %s durante %d tiempo",dispositivo,tiempo);

}

//----------------------------------------------------------------------------//

void wait(char* identificador_semaforo){log_debug(logDebug,"Ejecutando la primitiva Wait");fflush(stdout);
	avisarAlKernel(13,identificador_semaforo);
	log_debug(logDebug,"Pedi wait al kernel, %s",identificador_semaforo);
	int error=validarCodigoDeError(socketKernel);
	log_debug(logDebug,"El kernel me contesto :D");
	if (error){
		char* PCBAEnviar=generarPCB();
     	enviar(socketKernel,PCBAEnviar,9*tamanioInt);
     	flagBloqueo=-1;
		free(PCBAEnviar);
	}
	log_debug(logDebug,"Informé al Kernel que ejecute wait para el semáforo %s",identificador_semaforo);
}

//----------------------------------------------------------------------------//

void signalCPU(char* identificador_semaforo){log_debug(logDebug,"Ejecutando la primitiva Signal");
	avisarAlKernel(14,identificador_semaforo);
	validarCodigoDeError(socketKernel);
	log_debug(logDebug,"Informé al Kernel que ejecute la función signal para el semáforo %s",identificador_semaforo);
}
