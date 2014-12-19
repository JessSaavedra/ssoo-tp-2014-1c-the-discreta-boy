#include "kernel.h"

void planificadorDeProgramasListos(void){

			while(1){
			sem_wait(procesosNuevos);
			sem_wait(gradoDeMultiprogramacion);

			tipoProceso* proceso=retirar(colaProcesosNuevos);

			encolar(colaProcesosListos,proceso);								log_debug(logDebug,"Proceso encolado en Ready");

			imprimirProcesos(colaProcesosNuevos);
			}


}

void planificadorDeProgramasEliminados(void){
		while(1){
			tipoProceso* proceso=retirar(colaProcesosFinalizados);
			terminarProceso(proceso);
			sem_post(gradoDeMultiprogramacion);
		}
}
