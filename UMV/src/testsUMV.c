#include "stdUMV.h"
#include "CUnit/Basic.h"



int stubCPU(int* argv);
char* leidoPorCPU;
int sinErrorE,sinErrorL,procesoInexistenteE,segmentoInexistenteE,segmentationFaultE,procesoInexistenteL,segmentoInexistenteL,segmentationFaultL;

void testDumpsVarios(void){
	CargarEstructurasAdministrativas

	crearTablaProceso(0);
	int baseSegmento = crearSegmento(0,5);
	crearTablaProceso(1);
	crearSegmento(1,8);
	crearSegmento(1,8);
		printf("El proceso 0 ha sido creado con un segmento que comienza en %d\n de 5 bytes de tamaño, con una MemoriaPrincipal de 1000 posiciones\n",baseSegmento);
		printf("tambien fue creado el proceso 1 con dos segmentos de 8 bytes cada uno\n");
	consola();
}

void testSerRetardoPorConsola(void){
	printf("\n Por favor setee el retardo a 5 segundos escribiendo \"setRetardo: 5\"\n");
	consola();
	CU_ASSERT_EQUAL(retardo,5);
}

void testConexionesConCPU(void){
	CargarEstructurasAdministrativas

	pthread_t hiloCPU, hiloUMV;

	char* escrito="jose";
	char* leido;


	crearTablaProceso(20);
	int baseSegmentoEscritura=crearSegmento(20,10);
	int baseSegmentoLectura=crearSegmento(20,10);

	escribirMemoria(20,baseSegmentoEscritura,0,5,escrito);


	int* bases=malloc(2*sizeof(int));
	bases[0]=baseSegmentoEscritura;
	bases[1]=baseSegmentoLectura;

	pthread_create(&hiloUMV,NULL,(void* (*)(void*)) administrarConexiones,NULL);
	sleep(1);
	pthread_create(&hiloCPU,NULL,(void* (*)(void*))stubCPU,(void*) bases);

	pthread_join(hiloCPU,NULL);

	leerMemoria(20,baseSegmentoLectura,0,5,(void**)&leido);

	CU_ASSERT(string_equals_ignore_case(leido,"caca"));
	CU_ASSERT(string_equals_ignore_case(leidoPorCPU,escrito));
	CU_ASSERT_EQUAL(sinErrorE,0);
	CU_ASSERT_EQUAL(sinErrorL,0);
	CU_ASSERT_EQUAL(procesoInexistenteE,-1);
	CU_ASSERT_EQUAL(procesoInexistenteL,-1);
	CU_ASSERT_EQUAL(segmentoInexistenteE,-3);
	CU_ASSERT_EQUAL(segmentoInexistenteL,-3);
	CU_ASSERT_EQUAL(segmentationFaultE,-4);
	CU_ASSERT_EQUAL(segmentationFaultL,-4);


	LiberarEstructurasAdministrativas


}

void testAlgoritmoFF1elementosFrag(void){

	CargarEstructurasAdministrativas

	crearTablaProceso(42);
	crearTablaProceso(43);
	int dirVirtualSegmentoA=crearSegmento(43,15);
	crearSegmento(42,6);


	//----------------------------------------------------------// Buscar y destruir segmentoA

	char* key=string_itoa(43);
	t_tablaSegmentosProceso* procesoEncontradoA = dictionary_get(tablasDeSegmentos,key);
	free(key);

	key = string_itoa(dirVirtualSegmentoA);
	t_segmento* segmentoEncontradoA= dictionary_get(procesoEncontradoA->segmentos,key);
	free(key);

	destruirSegmento(segmentoEncontradoA);

	//----------------------------------------------------------//

	int dirVirtualSegmentoB=crearSegmento(42,10);

	key=string_itoa(42);
	t_tablaSegmentosProceso* procesoEncontradoB = dictionary_get(tablasDeSegmentos,key);
	free(key);

	key = string_itoa(dirVirtualSegmentoB);
	t_segmento* segmentoEncontradoB= dictionary_get(procesoEncontradoB->segmentos,key);
	free(key);

	int ubicacion = segmentoEncontradoB->ubicacionFisica;

	CU_ASSERT_EQUAL(ubicacion, 0);

	LiberarEstructurasAdministrativas
}

void testAlgoritmoFFNelementosFrag(void){

	CargarEstructurasAdministrativas

	crearTablaProceso(42);
	crearTablaProceso(43);
	int dirVirtualSegmentoA=crearSegmento(43,6);
	crearSegmento(42,6);
	int dirVirtualSegmentoB=crearSegmento(42,10);
	crearSegmento(43,5);

	//----------------------------------------------------------// Buscar y destruir segmentoA y B

	char* key=string_itoa(43);
	t_tablaSegmentosProceso* procesoEncontrado = dictionary_get(tablasDeSegmentos,key);
	free(key);

	key = string_itoa(dirVirtualSegmentoA);
	t_segmento* segmentoEncontrado= dictionary_get(procesoEncontrado->segmentos,key);
	free(key);

	destruirSegmento(segmentoEncontrado);

	key=string_itoa(42);
	procesoEncontrado = dictionary_get(tablasDeSegmentos,key);
	free(key);

	key = string_itoa(dirVirtualSegmentoB);
	segmentoEncontrado= dictionary_get(procesoEncontrado->segmentos,key);
	free(key);

	destruirSegmento(segmentoEncontrado);

	//---------------------------------------------------------------------//

	int dirVirtualSegmentoATestear = crearSegmento(42,8);

	key=string_itoa(42);
	procesoEncontrado = dictionary_get(tablasDeSegmentos,key);
	free(key);

	key = string_itoa(dirVirtualSegmentoATestear);
	segmentoEncontrado= dictionary_get(procesoEncontrado->segmentos,key);
	free(key);

	int ubicacion= segmentoEncontrado->ubicacionFisica;

	CU_ASSERT_EQUAL(ubicacion, 12);

	LiberarEstructurasAdministrativas
}

void testAlgoritmoWF1elementosFrag(void){

	CargarEstructurasAdministrativas

	algoritmoPosicionFisica=posicionWorstFit;

	crearTablaProceso(42);
	crearTablaProceso(43);
	int dirVirtualSegmentoA=crearSegmento(43,15);
	crearSegmento(42,6);

	//----------------------------------------------------------// Buscar y destruir segmentoA

	char* key=string_itoa(43);
	t_tablaSegmentosProceso* procesoEncontradoA = dictionary_get(tablasDeSegmentos,key);
	free(key);

	key = string_itoa(dirVirtualSegmentoA);
	t_segmento* segmentoEncontradoA= dictionary_get(procesoEncontradoA->segmentos,key);
	free(key);

	destruirSegmento(segmentoEncontradoA);

	//----------------------------------------------------------//

	int dirVirtualSegmentoB=crearSegmento(42,10);

	key=string_itoa(42);
	t_tablaSegmentosProceso* procesoEncontradoB = dictionary_get(tablasDeSegmentos,key);
	free(key);

	key = string_itoa(dirVirtualSegmentoB);
	t_segmento* segmentoEncontradoB= dictionary_get(procesoEncontradoB->segmentos,key);
	free(key);

	int ubicacion = segmentoEncontradoB->ubicacionFisica;

	CU_ASSERT_EQUAL(ubicacion, 21);

	LiberarEstructurasAdministrativas
}

void testAlgoritmoWFNelementosFrag(void){

	CargarEstructurasAdministrativas

	algoritmoPosicionFisica=posicionWorstFit;

	crearTablaProceso(42);
	crearTablaProceso(43);
	int dirVirtualSegmentoA=crearSegmento(43,6);
	crearSegmento(42,6);
	int dirVirtualSegmentoB=crearSegmento(42,10);
	crearSegmento(43,5);

	//----------------------------------------------------------// Buscar y destruir segmentoA y B

	char* key=string_itoa(43);
	t_tablaSegmentosProceso* procesoEncontrado = dictionary_get(tablasDeSegmentos,key);
	free(key);

	key = string_itoa(dirVirtualSegmentoA);
	t_segmento* segmentoEncontrado= dictionary_get(procesoEncontrado->segmentos,key);
	free(key);

	destruirSegmento(segmentoEncontrado);

	key=string_itoa(42);
	procesoEncontrado = dictionary_get(tablasDeSegmentos,key);
	free(key);

	key = string_itoa(dirVirtualSegmentoB);
	segmentoEncontrado= dictionary_get(procesoEncontrado->segmentos,key);
	free(key);

	destruirSegmento(segmentoEncontrado);

	//---------------------------------------------------------------------//

	int dirVirtualSegmentoATestear = crearSegmento(42,8);

	key=string_itoa(42);
	procesoEncontrado = dictionary_get(tablasDeSegmentos,key);
	free(key);

	key = string_itoa(dirVirtualSegmentoATestear);
	segmentoEncontrado= dictionary_get(procesoEncontrado->segmentos,key);
	free(key);

	int ubicacion= segmentoEncontrado->ubicacionFisica;

	CU_ASSERT_EQUAL(ubicacion, 27);

	LiberarEstructurasAdministrativas
}

void testMocking(void){
	printf("Soy una imitacion barata de una prueba :D\n");
	CU_ASSERT(1);
}

void testCompactacion2Segmentos1Proceso(void){
	CargarEstructurasAdministrativas

	t_segmento *segmento1,*segmento2,*segmento3;

	crearTablaProceso(15);
	int base1=crearSegmento(15,10);
	int base2=crearSegmento(15,20);
	int base3=crearSegmento(15,25);
	getSegmentoActual(15,base1,&segmento1);
	getSegmentoActual(15,base2,&segmento2);
	getSegmentoActual(15,base3,&segmento3);
	escribirMemoria(15,base1,0,10,"aaaaaaaaaa");
	escribirMemoria(15,base2,0,20,"bbbbbbbbbbbbbbbbbbbb");
	escribirMemoria(15,base3,0,25,"ccccccccccccccccccccccccc");log_debug(logDebug,"A punto de destruirSegmento %d",segmento2->base);
	destruirSegmento(segmento2);log_debug(logDebug,"Segmento Destruido");
	compactarMemoria();
	memoriaPrincipal[35]='\0';
	CU_ASSERT_EQUAL(segmento1->ubicacionFisica+segmento1->tamanio,segmento3->ubicacionFisica)
	CU_ASSERT(string_equals_ignore_case(memoriaPrincipal,"aaaaaaaaaaccccccccccccccccccccccccc"))

	LiberarEstructurasAdministrativas
}

void testCompactacionMuchosSegmentosMuchosProceso(void){
	CargarEstructurasAdministrativas

	t_segmento* segmentosPorProcesos[4][2];
	int bases[4][2];
	int i,j,tamanio,letra=0;
	char*buffer;



	printf("hola");
	for (i=0; i<4;i++){
		crearTablaProceso(i); printf("proceso creado");
		for(j=0,tamanio=10;j<2;j++,tamanio+=5){
			bases[i][j]=crearSegmento(i,tamanio);
			getSegmentoActual(i,bases[i][j],&segmentosPorProcesos[i][j]);
			buffer=string_repeat(letra+97,tamanio);
			escribirMemoria(i,bases[i][j],0,tamanio,buffer);
			free(buffer);
			letra++;
		}
	}
	destruirSegmento(segmentosPorProcesos[0][0]);
	destruirSegmento(segmentosPorProcesos[1][1]);
	destruirSegmento(segmentosPorProcesos[3][0]);
	compactarMemoria();
	memoriaPrincipal[65]='\0';

	CU_ASSERT(string_equals_ignore_case(memoriaPrincipal,"bbbbbbbbbbbbbbbcccccccccceeeeeeeeeefffffffffffffffhhhhhhhhhhhhhhh"))

	LiberarEstructurasAdministrativas
}

void testListaOrdenadaAgregar1(void){
	 t_list* lista = list_create();
	 int numero = 1;
	 int* numero2;
	 list_add_order_by(lista,(void*) &numero, esMayor);
	 numero2= (int*) list_get(lista,0);

	 CU_ASSERT_EQUAL(*numero2,1);

	 list_destroy(lista);

}

void testListaOrdenadaAgregarMuchos(void){

	 t_list* lista = list_create();
	 int numero = 1, numero2 = 2, numero3=3,numero4=4;
	 int* numeroB,*numero2B,*numero3B,*numero4B;



	 list_add_order_by(lista,(void*) &numero2, esMayor);
	 list_add_order_by(lista,(void*) &numero4, esMayor);
	 list_add_order_by(lista,(void*) &numero3, esMayor);
	 list_add_order_by(lista,(void*) &numero, esMayor);
	 numeroB= (int*) list_get(lista,0);
	 numero2B= (int*) list_get(lista,1);
	 numero3B= (int*) list_get(lista,2);
	 numero4B= (int*) list_get(lista,3);

	 CU_ASSERT_EQUAL(*numeroB,1);
	 CU_ASSERT_EQUAL(*numero2B,2);
	 CU_ASSERT_EQUAL(*numero3B,3);
	 CU_ASSERT_EQUAL(*numero4B,4);

	 list_destroy(lista);
}

void testSegmentosQuedanOrdenados(void){

	 CargarEstructurasAdministrativas



	 t_segmento* segmento2 = (t_segmento*) newSegmento(10,20,200);
	 t_segmento* segmento4 = (t_segmento*) newSegmento(10,20,400);
	 t_segmento* segmento3 = (t_segmento*) newSegmento(10,20,300);
	 t_segmento* segmento1 = (t_segmento*) newSegmento(10,20,1);

	 list_add_order_by(segmentosEnMemoria,segmento2,posicionFisicaSegmentoEsMayor);
	 list_add_order_by(segmentosEnMemoria,segmento4,posicionFisicaSegmentoEsMayor);
	 list_add_order_by(segmentosEnMemoria,segmento3,posicionFisicaSegmentoEsMayor);
	 list_add_order_by(segmentosEnMemoria,segmento1,posicionFisicaSegmentoEsMayor);

	 t_segmento* segmento1b = list_get(segmentosEnMemoria,0);
	 t_segmento* segmento2b = list_get(segmentosEnMemoria,1);
	 t_segmento* segmento3b = list_get(segmentosEnMemoria,2);
	 t_segmento* segmento4b = list_get(segmentosEnMemoria,3);

	 CU_ASSERT_EQUAL(segmento1b,segmento1);
	 CU_ASSERT_EQUAL(segmento2b,segmento2);
	 CU_ASSERT_EQUAL(segmento3b,segmento3);
	 CU_ASSERT_EQUAL(segmento4b,segmento4);


	 LiberarEstructurasAdministrativas
}

void testAlgoritmoFF0elementos(void){

	CargarEstructurasAdministrativas

	crearTablaProceso(42);

	int dirVirtualSegmento = crearSegmento(42,10);

	char* key=string_itoa(42);
	t_tablaSegmentosProceso* procesoEncontrado = dictionary_get(tablasDeSegmentos,key);
	free(key);

	key=string_itoa(dirVirtualSegmento);
	t_segmento* segmentoEncontrado= dictionary_get(procesoEncontrado->segmentos,key);
	free(key);
	int ubicacion = segmentoEncontrado->ubicacionFisica;

	CU_ASSERT_EQUAL(ubicacion, 0);

	LiberarEstructurasAdministrativas
}

void testAlgoritmoFF1elementos(void){

	CargarEstructurasAdministrativas

	crearTablaProceso(42);
	crearTablaProceso(43);
	crearSegmento(43,6);
	int dirVirtualSegmento = crearSegmento(42,10);

	char* key=string_itoa(42);
	t_tablaSegmentosProceso* procesoEncontrado = dictionary_get(tablasDeSegmentos,key);
	free(key);

	key = string_itoa(dirVirtualSegmento);
	t_segmento* segmentoEncontrado= dictionary_get(procesoEncontrado->segmentos,key);
	free(key);

	int ubicacion = segmentoEncontrado->ubicacionFisica;

	CU_ASSERT_EQUAL(ubicacion, 6);

	LiberarEstructurasAdministrativas
}

void testAlgoritmoFFNelementos(void){

	CargarEstructurasAdministrativas

	crearTablaProceso(42);
	crearTablaProceso(43);
	crearSegmento(43,6);
	crearSegmento(42,6);

	int dirVirtualSegmento = crearSegmento(42,10);

	char* key = string_itoa(42);
	t_tablaSegmentosProceso* procesoEncontrado = dictionary_get(tablasDeSegmentos,key);
	free(key);

	key = string_itoa(dirVirtualSegmento);
	t_segmento* segmentoEncontrado= dictionary_get(procesoEncontrado->segmentos,key);
	free(key);

	int ubicacion = segmentoEncontrado->ubicacionFisica;

	CU_ASSERT_EQUAL(ubicacion, 12);

	LiberarEstructurasAdministrativas
}

void testAlgoritmoWF0elementos(void){

	CargarEstructurasAdministrativas
	algoritmoPosicionFisica=posicionWorstFit;
	crearTablaProceso(42);

	int dirVirtualSegmento = crearSegmento(42,10);

	char* key = string_itoa(42);
	t_tablaSegmentosProceso* procesoEncontrado = dictionary_get(tablasDeSegmentos,key);
	free(key);

	key = string_itoa(dirVirtualSegmento);
	t_segmento* segmentoEncontrado= dictionary_get(procesoEncontrado->segmentos,key);
	free(key);

	int ubicacion = segmentoEncontrado->ubicacionFisica;

	CU_ASSERT_EQUAL(ubicacion, 0);

	LiberarEstructurasAdministrativas
}

void testAlgoritmoWF1elementos(void){

	CargarEstructurasAdministrativas
	algoritmoPosicionFisica=posicionWorstFit;
	crearTablaProceso(42);
	crearTablaProceso(43);
	crearSegmento(43,6);
	int dirVirtualSegmento = crearSegmento(42,10);

	char* key = string_itoa(42);
	t_tablaSegmentosProceso* procesoEncontrado = dictionary_get(tablasDeSegmentos,key);
	free(key);

	key = string_itoa(dirVirtualSegmento);
	t_segmento* segmentoEncontrado= dictionary_get(procesoEncontrado->segmentos,key);
	free(key);

	int ubicacion = segmentoEncontrado->ubicacionFisica;

	CU_ASSERT_EQUAL(ubicacion, 6);

	LiberarEstructurasAdministrativas
}

void testAlgoritmoWFNelementos(void){

	CargarEstructurasAdministrativas
	algoritmoPosicionFisica=posicionWorstFit;
	crearTablaProceso(42);
	crearTablaProceso(43);
	crearSegmento(43,6);
	crearSegmento(42,6);
	int dirVirtualSegmento = crearSegmento(42,10);

	char* key =string_itoa(42);
	t_tablaSegmentosProceso* procesoEncontrado = dictionary_get(tablasDeSegmentos,key);
	free(key);

	key = string_itoa(dirVirtualSegmento);
	t_segmento* segmentoEncontrado= dictionary_get(procesoEncontrado->segmentos,key);
	free(key);

	int ubicacion = segmentoEncontrado->ubicacionFisica;

	CU_ASSERT_EQUAL(ubicacion, 12);

	LiberarEstructurasAdministrativas
}

void testCambioAlgoritmosConsolaFaW(void){
	algoritmoPosicionFisica = posicionFirstFit;
	consola();
	CU_ASSERT_EQUAL(algoritmoPosicionFisica,posicionWorstFit);
}

void testCambioAlgoritmosConsolaWaF(void){
	algoritmoPosicionFisica = posicionWorstFit;
	consola();
	CU_ASSERT_EQUAL(algoritmoPosicionFisica,posicionFirstFit);
}

void testProcesoRepetidoFalla(void){

	CargarEstructurasAdministrativas

	crearTablaProceso(25);

	CU_ASSERT_EQUAL(crearTablaProceso(25),PROCESOYAEXISTE);

	LiberarEstructurasAdministrativas

}

void testProcesoOriginalAnda(void){

	CargarEstructurasAdministrativas

	CU_ASSERT_EQUAL(crearTablaProceso(25),0);
	CU_ASSERT_EQUAL(dictionary_has_key(tablasDeSegmentos,"25"),true);

	LiberarEstructurasAdministrativas

}

void testCrearSegmentoNuevoAnda(void){

	CargarEstructurasAdministrativas

	t_segmento *segmentoDeDiccionario, *segmentoDeLista;

	crearTablaProceso(25);
	char* key = string_itoa(25);
	t_tablaSegmentosProceso* proceso = dictionary_get(tablasDeSegmentos,key);
	free(key);
	int baseSegmento=crearSegmento(25,20);
	key = string_itoa(baseSegmento);
	CU_ASSERT(dictionary_has_key(proceso->segmentos,key));
	free(key);
		key = string_itoa(baseSegmento);
		segmentoDeDiccionario=dictionary_get(proceso->segmentos,key);
		free(key);
		segmentoDeLista=list_get(segmentosEnMemoria,0);
	CU_ASSERT_EQUAL(segmentoDeDiccionario,segmentoDeLista);


	LiberarEstructurasAdministrativas

}

void testSegmentoNuevoBienUbicado(void){

	CargarEstructurasAdministrativas

	char* key;

	crearTablaProceso(25);
	key = string_itoa(25);
	t_tablaSegmentosProceso* proceso = dictionary_get(tablasDeSegmentos,key);
	free(key);
	int baseSegmento1=crearSegmento(25,20);log_debug(logDebug,"Base1: %d",baseSegmento1);
	int baseSegmento2=crearSegmento(25,30);log_debug(logDebug,"Base2: %d",baseSegmento2);
	key = string_itoa(baseSegmento1);
	t_segmento* segmento1 = dictionary_get(proceso->segmentos,key);
	free(key);
	key = string_itoa(baseSegmento2);
	t_segmento* segmento2 = dictionary_get(proceso->segmentos,key);
	free(key);

	CU_ASSERT(baseSegmento2>=(baseSegmento1+20))
	CU_ASSERT(segmento2->base>=(segmento1->base+20))
	log_debug(logDebug,"InicioSegmento 1: %d",segmento1->base);
	log_debug(logDebug,"inicio Segmento2: %d",segmento2->base);
	CU_ASSERT_EQUAL(baseSegmento1,segmento1->base)
	CU_ASSERT_EQUAL(baseSegmento2,segmento2->base)

	LiberarEstructurasAdministrativas
}

void testCreacionSegmentoProcesoInexistenteFalla(void){

	CargarEstructurasAdministrativas

	crearTablaProceso(25);

	CU_ASSERT_EQUAL(crearSegmento(27,20),PROCESOINEXISTENTE);

	LiberarEstructurasAdministrativas
}

void testEscrituraLecturaCorrecta(void){
	CargarEstructurasAdministrativas

	char* escrito= "abcd";
	char* leido;

	crearTablaProceso(30);
	int baseSegmento = crearSegmento(30,10);log_debug(logDebug,"La base es %d",baseSegmento);
	escribirMemoria(30,baseSegmento,5,5,(void*) escrito);
	leerMemoria(30,baseSegmento,5,5,(void**) &leido);

	printf("\n  Se escribio: %s\n",escrito);
	printf("  Se leyo: %s\n  ",leido);

	CU_ASSERT(string_equals_ignore_case(leido,escrito))

	free(leido);

	LiberarEstructurasAdministrativas
}

void testEscrituraLargaSoloEscribeHastaLoPedido(void){
	CargarEstructurasAdministrativas

	char* escrito= "abcdefghi";

	crearTablaProceso(30);
	int baseSegmento = crearSegmento(30,10);log_debug(logDebug,"La base es %d",baseSegmento);
	escribirMemoria(30,baseSegmento,5,5,(void*) escrito);
	printf("\n  Se dio para escribir la cadena \"%s\", solo 5 bytes\n",escrito);
	t_tablaSegmentosProceso* proceso = dictionary_get(tablasDeSegmentos,"30");
	char* key =string_itoa(baseSegmento);
	t_segmento* segmentoEscrito = dictionary_get(proceso->segmentos,key);
	free(key);
	printf("  En la sexta posicion de lo escrito hay: \'%c\'", memoriaPrincipal[segmentoEscrito->ubicacionFisica+5+5]);

	CU_ASSERT(memoriaPrincipal[segmentoEscrito->ubicacionFisica+5+0]=='a');
	CU_ASSERT(memoriaPrincipal[segmentoEscrito->ubicacionFisica+5+1]=='b');
	CU_ASSERT(memoriaPrincipal[segmentoEscrito->ubicacionFisica+5+2]=='c');
	CU_ASSERT(memoriaPrincipal[segmentoEscrito->ubicacionFisica+5+3]=='d');
	CU_ASSERT(memoriaPrincipal[segmentoEscrito->ubicacionFisica+5+4]=='e');
	CU_ASSERT(memoriaPrincipal[segmentoEscrito->ubicacionFisica+5+5]!='f');

	LiberarEstructurasAdministrativas
}

void testLecturaEscrituraPorPantalla(void){

	CargarEstructurasAdministrativas

	crearTablaProceso(0);
	int baseSegmento = crearSegmento(0,10);
	printf("El proceso 0 ha sido creado con un segmento que comienza en %d\n de 10 bytes de tamaño\n",baseSegmento);

	consola();

	LiberarEstructurasAdministrativas
}

void testDestruirSegmento(void){
	CargarEstructurasAdministrativas

		int baseSegmento;
		t_segmento* segmento;

		crearTablaProceso(20);
		baseSegmento = crearSegmento(20,10);
		if (getSegmentoActual(20,baseSegmento,&segmento) != 0)
			CU_FAIL("ERROR AL OBTENER EL SEGMENTO");
		destruirSegmento(segmento);

		CU_ASSERT(list_is_empty(segmentosEnMemoria))

		LiberarEstructurasAdministrativas
}

void testDestruirProcesoInexistente(void){
	CargarEstructurasAdministrativas

		int error = destruirProceso(20);

		CU_ASSERT_EQUAL(error,PROCESOINEXISTENTE)

	LiberarEstructurasAdministrativas
}

void testDestruirProcesoExistente(void){
	CargarEstructurasAdministrativas


		crearTablaProceso(20);
		crearSegmento(20,10);
		crearSegmento(20,15);
		int error = destruirProceso(20);

		CU_ASSERT(dictionary_is_empty(tablasDeSegmentos))
		CU_ASSERT(list_is_empty(segmentosEnMemoria))
		CU_ASSERT_EQUAL(error,0)

	LiberarEstructurasAdministrativas
}


int runTests(void){

	CU_initialize_registry();

	CU_pSuite suiteTestsAlgoritmosCompact = CU_add_suite("Suite de Testeo UMV : Algoritmos en memoria Compacta",NULL,NULL);
	CU_add_test(suiteTestsAlgoritmosCompact,"Segmento con FF - Memoria Vacia",testAlgoritmoFF0elementos); 			// <---- Tests Algoritmos
	CU_add_test(suiteTestsAlgoritmosCompact,"Segmento con FF - Memoria Un Elemento",testAlgoritmoFF1elementos);
	CU_add_test(suiteTestsAlgoritmosCompact,"Segmento con FF - Memoria Mas de Un Elemento",testAlgoritmoFFNelementos);
	CU_add_test(suiteTestsAlgoritmosCompact,"Segmento con WF - Memoria Vacia",testAlgoritmoWF0elementos);
	CU_add_test(suiteTestsAlgoritmosCompact,"Segmento con WF - Memoria Un Elemento",testAlgoritmoWF1elementos);
	CU_add_test(suiteTestsAlgoritmosCompact,"Segmento con WF - Memoria Mas de Un Elemento",testAlgoritmoWFNelementos);


	CU_pSuite suiteTestsAlgoritmosFrag = CU_add_suite("Suite de Testeo UMV : Algoritmos en memoria Fragmentada",NULL,NULL);
	CU_add_test(suiteTestsAlgoritmosFrag,"Segmento con FF - Frag - Memoria Un Elemento",testAlgoritmoFF1elementosFrag);
	CU_add_test(suiteTestsAlgoritmosFrag,"Segmento con FF - Frag - Memoria Mas de Un Elemento",testAlgoritmoFFNelementosFrag);
	CU_add_test(suiteTestsAlgoritmosFrag,"Segmento con WF - Frag - Memoria Un Elemento",testAlgoritmoWF1elementosFrag);
	CU_add_test(suiteTestsAlgoritmosFrag,"Segmento con WF - Frag - Memoria Mas de Un Elemento",testAlgoritmoWFNelementosFrag);

	CU_pSuite suiteTestsSegmentos = CU_add_suite("Suite de Testeo UMV : Segmentos",NULL,NULL);
	CU_add_test(suiteTestsSegmentos,"Proceso repetido falla",testProcesoRepetidoFalla);				//<---- Tests Segmentos
	CU_add_test(suiteTestsSegmentos,"Proceso no repetido anda",testProcesoOriginalAnda);
	CU_add_test(suiteTestsSegmentos,"Nuevo Segmento Anda",testCrearSegmentoNuevoAnda);
	CU_add_test(suiteTestsSegmentos,"Nuevo segmento se ubica bajo el anterior",testSegmentoNuevoBienUbicado);
	CU_add_test(suiteTestsSegmentos,"Nuevo segmento de proceso inexistente falla",testCreacionSegmentoProcesoInexistenteFalla);
	CU_add_test(suiteTestsSegmentos,"Los segmentos quedan ordenados", testSegmentosQuedanOrdenados);
	CU_add_test(suiteTestsSegmentos,"Destruir Segmento Anda",testDestruirSegmento);
	CU_add_test(suiteTestsSegmentos,"Destruir proceso existente Anda",testDestruirProcesoExistente);
	CU_add_test(suiteTestsSegmentos,"Destruir proceso inexistente falla",testDestruirProcesoInexistente);

	CU_pSuite suiteTestsLecturaEscritura = CU_add_suite("Suite de Testeo UMV : Lectura/Escritura",NULL,NULL);
	CU_add_test(suiteTestsLecturaEscritura,"Datos leidos coinciden con los escritos",testEscrituraLecturaCorrecta);
	CU_add_test(suiteTestsLecturaEscritura,"Se escribe solo la cantidad de datos indicados",testEscrituraLargaSoloEscribeHastaLoPedido);

	CU_pSuite suiteTestsOrderedLists = CU_add_suite("Suite de Testeo UMV : Listas ordenadas",NULL,NULL);
	CU_add_test(suiteTestsOrderedLists,"La lista ecibe 1 correctamente", testListaOrdenadaAgregar1);
	CU_add_test(suiteTestsOrderedLists,"La lista ecibe muchos correctamente", testListaOrdenadaAgregarMuchos);

	CU_pSuite suiteTestsCompactacion = CU_add_suite("Suite de Testeo UMV : Compactacion",NULL,NULL);
	CU_add_test(suiteTestsCompactacion,"La compactacion de 2 segmentos funciona", testCompactacion2Segmentos1Proceso);
	CU_add_test(suiteTestsCompactacion,"La compactacion de muchos segmentos funciona", testCompactacionMuchosSegmentosMuchosProceso);



	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();

	return CU_get_error();
}


int runInteractiveTests(void){

	CU_initialize_registry();

	CU_pSuite suiteTestsAInteractivos = CU_add_suite("Suite de Testeo UMV : Tests Interactivos",NULL,NULL);

	CU_add_test(suiteTestsAInteractivos,"Comunicacion con CPU",testConexionesConCPU);
	CU_add_test(suiteTestsAInteractivos,"Intercambiar Algoritmos desde Consola FF a WF",testCambioAlgoritmosConsolaFaW);
	CU_add_test(suiteTestsAInteractivos,"Intercambiar Algoritmos desde Consola WF a FF",testCambioAlgoritmosConsolaWaF);
	CU_add_test(suiteTestsAInteractivos,"L/E por consola",testLecturaEscrituraPorPantalla);
	CU_add_test(suiteTestsAInteractivos,"Dumps diversos por Consola", testDumpsVarios);
	CU_add_test(suiteTestsAInteractivos,"Seteo del retardo", testSerRetardoPorConsola);

	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();

	return CU_get_error();
}
