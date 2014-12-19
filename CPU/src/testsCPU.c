/*
 * testsCPU.c
 *
 *  Created on: 30/05/2014
 *      Author: utnso
 */

/*#include "headerCompleto.h"
#include "CUnit/Basic.h"
extern void* stack;
extern tipoPCB PCB;
extern t_size tamanioInt;
extern t_dictionary* diccionarioDeVariables;




void testDefinirVariable(void){
	INICIALIZARTESTS

	char variable;
	definirVariable('a');
	memcpy(&variable,stack+5,1);

	CU_ASSERT(variable=='a');

	FINALIZARTEST

}

void testObtenerPosicionVariable(void){
	INICIALIZARTESTS

	definirVariable('a');
	int direccion1=obtenerPosicionVariable('a');
	printf("%d",direccion1);

	CU_ASSERT(direccion1==5);

	FINALIZARTEST

}

void testAsignar(void){
	INICIALIZARTESTS

	char variable;
	int valor;

	definirVariable('c');
	asignar(5,45);
	memcpy(&variable,stack+5,1);
	memcpy(&valor,stack+6,tamanioInt);

	printf("%c=%d/n",variable,valor);

	CU_ASSERT(valor==45);

	FINALIZARTEST

}

void testDereferenciar(void){
	INICIALIZARTESTS

	asignar(5,123);
	int valor=dereferenciar(5);


	CU_ASSERT(valor==123);

	FINALIZARTEST
}

void testIrAlLabel(void){
	INICIALIZARTESTS


	int direccionEtiqueta=34;

	dictionary_put(indiceDeEtiquetas,"funcionPrueba",&direccionEtiqueta);

	int direccion=irAlLabel("funcionPrueba");

	CU_ASSERT(direccion==34);

	FINALIZARTEST



}

void testStack(void){
	INICIALIZARTESTS

	definirVariable('a');
	definirVariable('b');
	definirVariable('c');
	int direccion1=obtenerPosicionVariable('a');
	int direccion2=obtenerPosicionVariable('b');
	int direccion3=obtenerPosicionVariable('c');
	asignar(direccion1,12);
	asignar(direccion2,24);
	asignar(direccion3,34);
	int valor1=dereferenciar(direccion1);
	int valor2=dereferenciar(direccion2);
	int valor3=dereferenciar(direccion3);
	char variable1;
	char variable2;
	char variable3;


	int direccionEtiqueta=34;
	int direccionEtiqueta2=25;




	dictionary_put(indiceDeEtiquetas,"funcionPrueba",&direccionEtiqueta);
	dictionary_put(indiceDeEtiquetas,"funcionPrueba2",&direccionEtiqueta2);


	llamarSinRetorno("funcionPrueba");

	definirVariable('a');
	int direccion4=obtenerPosicionVariable('a');
	asignar(direccion4,85);
	int valor4=dereferenciar(direccion4);
	char variable4;
	memcpy(&variable4,stack+28,1);


	finalizar();

	int direccion5=obtenerPosicionVariable('a');
	int valor5=dereferenciar(direccion5);


	llamarConRetorno("funcionPrueba2",direccion3);

	retornar(43);

	int valor6=dereferenciar(direccion3);

	memcpy(&variable1,stack+5,1);
	memcpy(&variable2,stack+10,1);
	memcpy(&variable3,stack+15,1);





	printf("%c=%d\n",variable1,valor1);
	printf("%c=%d\n",variable2,valor2);
	printf("%c=%d\n",variable3,valor3);
	printf("%c=%d\n",variable4,valor4);
	printf("%c=%d\n",variable1,valor5);
	printf("%c=%d\n",variable3,valor6);

	FINALIZARTEST

}

void testLlamarSinRetorno(void){

	INICIALIZARTESTS


	int direccionEtiqueta=34;

	definirVariable('a');


	dictionary_put(indiceDeEtiquetas,"funcionPrueba",&direccionEtiqueta);

	int programCounter =llamarSinRetorno("funcionPrueba");

	CU_ASSERT(programCounter==34);
	CU_ASSERT(PCB.cursorDelStack==18);
	CU_ASSERT(PCB.tamanioContextoActual==0);


	FINALIZARTEST

}

void testLlamarConRetorno(void){

	INICIALIZARTESTS

	int direccionEtiqueta=34;

	definirVariable('a');
	int direccion=21;
	int direccionComparable;


	dictionary_put(indiceDeEtiquetas,"funcionPrueba",&direccionEtiqueta);

	int programCounter=llamarConRetorno("funcionPrueba",direccion);

	memcpy(&direccionComparable,stack+18,tamanioInt);

	CU_ASSERT(programCounter==34);
	CU_ASSERT(PCB.cursorDelStack==22);
	CU_ASSERT(direccion==direccionComparable);
	CU_ASSERT(PCB.tamanioContextoActual==0);


	FINALIZARTEST

}

void testFinalizar(void){

	INICIALIZARTESTS


		int direccionEtiqueta=34;
		int direccionEtiqueta2=35;

		definirVariable('a');


		dictionary_put(indiceDeEtiquetas,"funcionPrueba",&direccionEtiqueta);
		dictionary_put(indiceDeEtiquetas,"funcionPrueba2",&direccionEtiqueta2);

		llamarSinRetorno("funcionPrueba");

		definirVariable('a');

		llamarSinRetorno("funcionPrueba2");

		int programCounter =finalizar();



		CU_ASSERT(programCounter==34);
		CU_ASSERT(PCB.cursorDelStack==18);
		CU_ASSERT(PCB.tamanioContextoActual==1);


		FINALIZARTEST


}

void testRetornar(void){

		INICIALIZARTESTS


			int direccionEtiqueta=34;
			int direccionEtiqueta2=35;

			definirVariable('a');
			int direccion=obtenerPosicionVariable('a');

			dictionary_put(indiceDeEtiquetas,"funcionPrueba",&direccionEtiqueta);
			dictionary_put(indiceDeEtiquetas,"funcionPrueba2",&direccionEtiqueta2);

			llamarSinRetorno("funcionPrueba");

			definirVariable('a');


			llamarConRetorno("funcionPrueba2",direccion);

			int programCounter =retornar(22);
			int valor=dereferenciar(direccion);

			CU_ASSERT(programCounter==34);
			CU_ASSERT(PCB.cursorDelStack==18);
			CU_ASSERT(PCB.tamanioContextoActual==1);
			CU_ASSERT(valor==22);


			FINALIZARTEST


	}


int runTests(void){

	CU_initialize_registry();

	CU_pSuite suiteTestsPrimitivas = CU_add_suite("Suite de Testeo de primitivas\n",NULL,NULL);
	CU_add_test(suiteTestsPrimitivas,"Test para probar la primitiva definirVariable\n",testDefinirVariable);
	CU_add_test(suiteTestsPrimitivas,"Test para probar la primitiva ObtenerPosicionVariable\n",testObtenerPosicionVariable);
	CU_add_test(suiteTestsPrimitivas,"Test para probar la primitiva Asignar\n",testAsignar);
	CU_add_test(suiteTestsPrimitivas,"Test para probar la primitiva deferenciar\n",testDereferenciar);
	CU_add_test(suiteTestsPrimitivas,"Test para probar la primitiva itAlLabel\n",testIrAlLabel);
	CU_add_test(suiteTestsPrimitivas,"Test para probar la primitiva llamarSinRetorno\n",testLlamarSinRetorno);
	CU_add_test(suiteTestsPrimitivas,"Test para probar la primitiva llamarConRetorno\n",testLlamarConRetorno);
	CU_add_test(suiteTestsPrimitivas,"Test para probar la primitiva finalizar\n",testFinalizar);
	CU_add_test(suiteTestsPrimitivas,"Test para probar la primitiva retornar\n",testRetornar);
	CU_add_test(suiteTestsPrimitivas,"Test para probar como queda el stack\n",testStack);

	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();

	return CU_get_error();
} */
