/*
 * testsPP.c
 *
 *  Created on: 31/05/2014
 *      Author: utnso
 */
#include "stdPP.h"
#include "CUnit/Basic.h"

//Internas---------------

void crearArchivoDeConfiguraciones(char* unaIP,char* unPuerto){
	char* pathConfig=getenv("ANSISOP_CONFIG");
	FILE* archivo = fopen(pathConfig,"w");
	fprintf(archivo,"IP=%s\nPuerto=%s",unaIP,unPuerto);
	fclose(archivo);
}

//Tests------------------
void testCargarConfiguraciones(void){
	crearArchivoDeConfiguraciones("laIP","elPuerto");
	int resultado=cargarConfiguraciones();
	CU_ASSERT(resultado==0);
	CU_ASSERT(string_equals_ignore_case(ip,"laIP"));
	CU_ASSERT(string_equals_ignore_case(puerto,"elPuerto"));
}

void testCrearSocketComunicacion(void){
	ip="127.0.0.1";
	puerto="6068";
	int socket=newSocketServidor();
	CU_ASSERT(socket!=-1);
	close(socket);
}

void testEnviarScript(void){

}

void testMocking(void){
	printf("Soy una imitacion barata de una prueba :D\n");
	CU_ASSERT(1);
}


int runTests(void){

	CU_initialize_registry();

	CU_pSuite suiteTests = CU_add_suite("Suite de Testeo PP",NULL,NULL);;
	CU_add_test(suiteTests,"Carga de Configuraciones",testCargarConfiguraciones);
	CU_add_test(suiteTests,"Creacion del Socket para Comunicacion",testCrearSocketComunicacion);
	CU_add_test(suiteTests,"Preparacion Exitosa del Header para Envio",testMocking);

	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();

	return CU_get_error();
}

