#include "headerCompleto.h"


tipoPCB PCB;
t_dictionary* diccionarioDeVariables;
char* indiceDeEtiquetas;
t_size tamanioInt;
int finDePrograma;
int flagError;
int socketUMV;
int socketKernel;
void* stack;
t_log* logDebug;
int desconexionCPU;
int flagBloqueo;


int main(int argc, char** argv){

	logDebug = log_create("debugCPU.txt","CPU",1,LOG_LEVEL_DEBUG);
	char* puertoKernel;
	char* ipKernel;
	char* puertoUMV;
	char* ipUMV;

	desconexionCPU=0;
	signal(SIGUSR1,desconectarCPU);

	if(argc!=2){
		fprintf(stderr,"Error en Parametros, el CPU requiere uno de las siguientes opciones: \n -El path del archivo de configuraciones \n -El parametro testOnly \n");
		return 1;
			}
		if(string_equals_ignore_case(argv[1],"testOnly")){
			//runTests();
			return 0;

		}

		t_config* configuracion = config_create(argv[1]);

		if(config_has_property(configuracion,"PuertoUMV") && config_has_property(configuracion,"PuertoKernel") &&
			config_has_property(configuracion,"IPUMV") && config_has_property(configuracion,"IPKernel")){

				puertoKernel=config_get_string_value(configuracion,"PuertoKernel");	log_debug(logDebug,"Configuré el puerto del Kernel con el valor %s",puertoKernel);
				ipKernel=config_get_string_value(configuracion,"IPKernel");	log_debug(logDebug,"Configuré la IP del Kernel con el valor %s",ipKernel);
				puertoUMV=config_get_string_value(configuracion,"PuertoUMV");	log_debug(logDebug,"Configuré el puerto de la UMV con el valor %s",puertoUMV);
				ipUMV=config_get_string_value(configuracion,"IPUMV");	log_debug(logDebug,"Configuré la IP de la UMV con el valor %s",ipUMV);
			} else {
				fprintf(stderr,"El archivo de configuracion no cumple con los parametros requeridos\n");
				return 1;
		}



	log_debug(logDebug,"CPU iniciado");
	AnSISOP_kernel kernel_functions = {.AnSISOP_signal=signalCPU,.AnSISOP_wait=wait};
	AnSISOP_funciones functions = {.AnSISOP_definirVariable = definirVariable,.AnSISOP_obtenerPosicionVariable = obtenerPosicionVariable,.AnSISOP_dereferenciar=dereferenciar,.AnSISOP_asignar=asignar,.AnSISOP_imprimir=imprimir,.AnSISOP_imprimirTexto=imprimirTexto,.AnSISOP_irAlLabel=irAlLabel,.AnSISOP_llamarConRetorno=llamarConRetorno,.AnSISOP_llamarSinRetorno=llamarSinRetorno,.AnSISOP_retornar=retornar,.AnSISOP_finalizar=finalizar,.AnSISOP_obtenerValorCompartida=obtenerValorCompartida,.AnSISOP_asignarValorCompartida=asignarValorCompartida,.AnSISOP_entradaSalida=entradaSalida};
	tamanioInt=sizeof(int);
	diccionarioDeVariables=dictionary_create();

	int quantaEjecutada;


	//-------------------------------------------- Conexiones --------------------------------------------//


	socketUMV = conectar(puertoUMV,ipUMV);
	socketKernel=conectar(puertoKernel,ipKernel);

	// ------------------------------------------ Handshake ------------------------------------------- //

	char* handshake=malloc(tamanioInt);
	int numero = 11;
	memcpy(handshake,&numero,tamanioInt);
	enviar(socketUMV,handshake,tamanioInt);
	free(handshake);

	// -------------------------------------------- Inicio -------------------------------------------- //
	while(1){


		//-----------------------------------------Seteo flags-----------------------------------------//

		flagBloqueo=0;
		flagError=0;
		finDePrograma=0;
		quantaEjecutada=1;

		//------------------------------------------ Recibir PCB -------------------------------------------//

		log_debug(logDebug,"Esperando nuevo PCB");
		char* PCBRecibido = recibirPCB ();


		// --------------------------------------- Creación del PCB --------------------------------------- //

		memcpy(&PCB.identificador,PCBRecibido,tamanioInt);
		memcpy(&PCB.segmentoDeCodigo,PCBRecibido+tamanioInt,tamanioInt);
		memcpy(&PCB.segmentoDeStack,PCBRecibido+2*tamanioInt,tamanioInt);
		memcpy(&PCB.cursorDelStack,PCBRecibido+3*tamanioInt,tamanioInt);
		memcpy(&PCB.indiceDeCodigo,PCBRecibido+4*tamanioInt,tamanioInt);
		memcpy(&PCB.indiceEtiquetas,PCBRecibido+5*tamanioInt,tamanioInt);
		memcpy(&PCB.programCounter,PCBRecibido+6*tamanioInt,tamanioInt);
		memcpy(&PCB.tamanioContextoActual,PCBRecibido+7*tamanioInt,tamanioInt);
		memcpy(&PCB.tamanioIndiceEtiquetas,PCBRecibido+8*tamanioInt,tamanioInt);

		// ----------------------------------------- Setear proceso ----------------------------------------- //

		char* seteoProceso=malloc(2*tamanioInt);
		int codigoOperacion = 2;
		memcpy(seteoProceso,&codigoOperacion,tamanioInt);
		memcpy(seteoProceso+tamanioInt,&PCB.identificador,tamanioInt);
		enviar(socketUMV,seteoProceso,2*tamanioInt);
		validarCodigoDeError(socketUMV);
		free(seteoProceso);

		// --------------------------------- Creación del índice de etiquetas --------------------------------- //

		if(PCB.tamanioIndiceEtiquetas>0){
			char* pedidoIndiceDeEtiquetas=generarStreamLectura(PCB.indiceEtiquetas,0,PCB.tamanioIndiceEtiquetas);
			indiceDeEtiquetas=pedirDeMemoria(pedidoIndiceDeEtiquetas,PCB.tamanioIndiceEtiquetas);
			free(pedidoIndiceDeEtiquetas);
			log_debug(logDebug,"Indice de etiquetas creado");
		}

		// ------------------------------------ Recuperar contexto actual -------------------------------------- //

		if (PCB.tamanioContextoActual!=0)
			recuperarContextoActual();

		// ----------------------------------------- Ejecutar programa ----------------------------------------- //

		if(flagError==0){
			log_debug(logDebug,"Voy a ejecutar el programa");
			int quantum;
			int retardo;
			memcpy(&quantum,PCBRecibido+9*tamanioInt,tamanioInt);
			memcpy(&retardo,PCBRecibido+10*tamanioInt,tamanioInt);
			int programCounterActual;
			paquetePedidoLinea.segmento = PCB.segmentoDeCodigo;
			while(finDePrograma==0 && quantaEjecutada<=quantum && flagError==0 && flagBloqueo==0){
				milisleep(retardo);  log_debug(logDebug,"retardo: %d",retardo);

				programCounterActual=PCB.programCounter;
				PCB.programCounter++;

				char* pedidoDireccionDeLinea=generarStreamLectura(PCB.indiceDeCodigo,programCounterActual*8,8);
				char* streamDireccionDeLinea=pedirDeMemoria(pedidoDireccionDeLinea,8);

				memcpy(&paquetePedidoLinea.offset,streamDireccionDeLinea,4);
				memcpy(&paquetePedidoLinea.longitud,streamDireccionDeLinea+4,4);

				char* pedidoEnviado=generarStreamLectura(paquetePedidoLinea.segmento,paquetePedidoLinea.offset,paquetePedidoLinea.longitud);
				char* lineaRecibida=pedirDeMemoria(pedidoEnviado,paquetePedidoLinea.longitud);
				char* lineaEjecutable=agregarBarraCero(lineaRecibida);
				log_debug(logDebug,"por ejecutar linea:%s",lineaEjecutable);
				analizadorLinea(strdup(lineaEjecutable), &functions, &kernel_functions);

				quantaEjecutada++;

				free(pedidoEnviado);
				free(streamDireccionDeLinea);
				free(pedidoDireccionDeLinea);
				free(lineaRecibida);
			}

		//-------------------------------------------------Terminar Proceso------------------------------------------------------------//

			if(finDePrograma==-1){
				log_debug(logDebug,"Programa terminado");
				int codigoDeProcesoTerminado=18;
				char* terminarProceso=malloc(tamanioInt);
				memcpy(terminarProceso,&codigoDeProcesoTerminado,tamanioInt);
				enviar(socketKernel,terminarProceso,tamanioInt);
				free(terminarProceso);
			}

			if(quantaEjecutada>quantum && finDePrograma==0 && flagError==0 && desconexionCPU==0 && flagBloqueo==0){
				enviarPCBActualizado(PCB,10);
				log_debug(logDebug,"Quantum terminado");
			}

			if(quantaEjecutada>quantum && desconexionCPU==-1){
				log_debug(logDebug,"CPU se desconectó");
				enviarPCBActualizado(PCB,19);
				return 0;
			}

		// ------------------------------------ Liberación de memoria ------------------------------------- //
		}
		dictionary_clean_and_destroy_elements(diccionarioDeVariables,free);
		free(PCBRecibido);


	}
	free(indiceDeEtiquetas);
	config_destroy(configuracion);
	close(socketUMV);
	close(socketKernel);
	return 0;
}

