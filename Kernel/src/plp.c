/*
 ============================================================================
 Name        : Prueba.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "kernel.h"

tipoPCB PCB;


void plp(void) {

	pthread_t hiloOrganizadorDeProgramas;
	pthread_t hiloDeProgramasListos;
	pthread_t hiloDeProgramasEliminados;

	pthread_create(&hiloOrganizadorDeProgramas,NULL,(void* (*)(void*))organizadorDeProgramas,NULL);
	pthread_create(&hiloDeProgramasListos,NULL,(void* (*)(void*))planificadorDeProgramasListos,NULL);
	pthread_create(&hiloDeProgramasEliminados,NULL,(void* (*)(void*))planificadorDeProgramasEliminados,NULL);

	pthread_join(hiloOrganizadorDeProgramas,NULL);
	pthread_join(hiloDeProgramasListos,NULL);
	pthread_join(hiloDeProgramasEliminados,NULL);



}







