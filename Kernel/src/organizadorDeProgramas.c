/*
 * organizadorDeProgramas.c
 *
 *  Created on: 18/06/2014
 *      Author: utnso
 */
#include "kernel.h"

void organizadorDeProgramas(void){

		// ----------------------------------------- Handshake ------------------------------------------- //
		socketUMV=conectar(puertoUMV,IPUMV);
		char* handshake=malloc(TAMANIO_INT);
		int numero = 10;
		memcpy(handshake,&numero,TAMANIO_INT);
		enviar(socketUMV,handshake,TAMANIO_INT);
		free(handshake);

		//-------------------------------------------------------------------------------------------------//

		int socketEscuchador=crearSocketEscuchaSobre(puertoPP,20);
		int pid = 0;
		while(1){
			segmentosCreadosFlag=0;
			int tamanioCodigo;
			int socketPrograma=accept(socketEscuchador,NULL,NULL);											log_debug(logDebug,"Nuevo programa conectado");
			recv(socketPrograma,&tamanioCodigo,TAMANIO_INT,MSG_WAITALL);
			char* codigo=malloc(tamanioCodigo);
			recv(socketPrograma,codigo,tamanioCodigo,MSG_WAITALL);


			tipoProceso* proceso=malloc(sizeof(tipoProceso));

			proceso->socketPP=socketPrograma;

			t_medatada_program* metadata;
			metadata = metadata_desde_literal(codigo);

			//------------------------------ crear proceso ------------------------------------------//

				crearProceso(pid);

			//------------------------------ Crear segmentos de codigo ------------------------------//

			proceso=crearSegmentosDeProceso(pid,codigo,metadata,tamanioCodigo,proceso);

			if (segmentosCreadosFlag == 0){

				//--------------------------------- Calcular peso ----------------------------------------//

				proceso->peso = 5 * metadata->cantidad_de_etiquetas + 3 * metadata->cantidad_de_funciones + metadata->instrucciones_size;

				//-------------------------------- Encolar en news ----------------------------------//

				encolar_order_by(colaProcesosNuevos,proceso,esDeMenorPeso);										log_debug(logDebug,"Proceso encolado en New");

				//----------------------------- Imprimir cola de procesos ----------------------------------//

				imprimirProcesos(colaProcesosNuevos);
				sem_post(procesosNuevos);


				//---------------------------------------------------------------------------------------//

				pid++;

			}
			else {
				notificar(proceso,"No hay memoria suficiente");
				terminarProceso(proceso);
			}
			metadata_destruir(metadata);
		}
}


