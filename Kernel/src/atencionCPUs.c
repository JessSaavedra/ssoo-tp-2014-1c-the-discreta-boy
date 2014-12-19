/*
 * atencionCPUs.c
 *
 *  Created on: 05/06/2014
 *      Author: utnso
 */

#include "kernel.h"

void escucharPedidosCPU(void){log_debug(logDebug,"Entre a escucharpedidosCPU");

	t_list *CPUsAVerificar,*CPUsListos,*CPUsEnUso;

	while(1){
		CPUsAVerificar = desencolarCPUsEnUso();log_debug(logDebug,"Se desencolaron %d CPUs en uso",list_size(CPUsAVerificar));
		CPUsListos = seleccionarListosLectura(CPUsAVerificar);log_debug(logDebug,"Hay %d CPUs listos",list_size(CPUsListos));
		CPUsEnUso = list_complement(CPUsAVerificar,CPUsListos);log_debug(logDebug,"Se reencolaran %d CPUs que siguen en uso",list_size(CPUsEnUso));
		reencolarCPUsEnUso(CPUsEnUso);log_debug(logDebug,"Se reencolaron los CPUs en uso",list_size(CPUsAVerificar));
		atenderPedidosCPUs(CPUsListos);log_debug(logDebug,"Se atendio a los CPUs listos");
		list_destroy(CPUsAVerificar);
		list_destroy(CPUsListos);
		list_destroy(CPUsEnUso);
	}
	return;
}

void reencolarCPUsEnUso(t_list* CPUsEnUso){

	void _encolarCPUEnUso(t_cliente* CPU){
		encolar(colaCPUsEnUso,CPU);
	}

	list_iterate(CPUsEnUso,(void(*)(void*))_encolarCPUEnUso);
}

t_list* desencolarCPUsEnUso(void){

	t_list* CPUs = list_create();

	do list_add(CPUs,retirar(colaCPUsEnUso)); while(list_size(colaCPUsEnUso->cola)>0);
	return CPUs;
}

void atenderCPU(t_cliente* CPU){

	int codigoOperacion,excepcion;

	if(0>=recv(CPU->socket,&codigoOperacion,sizeof(codigoOperacion),MSG_WAITALL)){	log_debug(logDebug,"Error en comunicacion al atender CPU, errno: %d", errno);
		excepcion=CPUMURIO;
	}
	else
		excepcion=codigoOperacion;													log_debug(logDebug,"Codigo de operacion recibido: %d",codigoOperacion);
	if(codigoOperacion>=0)
		excepcion = manejarSyscall(codigoOperacion,CPU);
	if(excepcion)
		manejarExcepcion(excepcion,CPU);
	return;
}

void atenderPedidosCPUs(t_list* CPUsParaAtender){
	list_iterate(CPUsParaAtender,(void(*)(void*))atenderCPU);
	return;
}
