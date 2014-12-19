#include "kernel.h"
#include "CUnit/Basic.h"
#include <signal.h>

void testMocking(void){//Mocking
	printf("Soy un fraude testifero!\n");
	CU_ASSERT(1);
}

void testEnviarCompartida(void){

	CrearEstructurasGlobales

	int compartida = 5;
	pthread_t hiloPCP, hiloCPU;
	int* socketCPU = malloc(sizeof(int));
	*socketCPU = crearSocketEscuchaSobre("7012",2);
	t_cliente* cpu1;
	int compartidaEnviada;

	dictionary_put(variablesGlobales,"jose",&compartida);

	void _enviarComaprtida(void){

			cpu1 = newCliente(accept(*socketCPU,NULL,NULL));

			operarCompartida(cpu1,enviarCompartida);
		}

	pthread_create(&hiloPCP,NULL,(void*(*)(void*))_enviarComaprtida,NULL);
	sleep(1);
	pthread_create(&hiloCPU,NULL,(void*(*)(void*))stubEnviarCompartida,(void*) &compartidaEnviada);

	pthread_join(hiloCPU,NULL);
	pthread_join(hiloPCP,NULL);

	CU_ASSERT_EQUAL(compartidaEnviada,compartida);
	t_cliente* cpuReencolado = list_get(colaCPUsEnUso->cola,0);
	CU_ASSERT_EQUAL(cpuReencolado->socket,cpu1->socket);

	LiberarEstructurasGlobales
}

void testEscribirCompartida(void){

	CrearEstructurasGlobales

	int compartida,error;
	pthread_t hiloPCP, hiloCPU;
	int* socketCPU = malloc(sizeof(int));
	*socketCPU = crearSocketEscuchaSobre("7014",2);
	t_cliente* cpu1;

	dictionary_put(variablesGlobales,"jose",&compartida);



	void _enviarComaprtida(void){

		cpu1 = newCliente(accept(*socketCPU,NULL,NULL));

			operarCompartida(cpu1,escribirCompartida);
		}

	pthread_create(&hiloPCP,NULL,(void*(*)(void*))_enviarComaprtida,NULL);
	sleep(1);
	pthread_create(&hiloCPU,NULL,(void*(*)(void*))stubEscribirCompartida,(void*) &error);

	pthread_join(hiloCPU,NULL);
	pthread_join(hiloPCP,NULL);

	CU_ASSERT_EQUAL(error,0);
	CU_ASSERT_EQUAL(compartida,5);
	t_cliente* cpuReencolado = list_get(colaCPUsEnUso->cola,0);
	CU_ASSERT_EQUAL(cpuReencolado->socket,cpu1->socket);

	LiberarEstructurasGlobales
}

void testEscribirCompartidaInexistenteFalla(void){

	CrearEstructurasGlobales

	int compartida,error;
	pthread_t hiloPCP, hiloCPU;
	int* socketCPU = malloc(sizeof(int));
	*socketCPU = crearSocketEscuchaSobre("7016",2);
	t_cliente* cpu1;

	dictionary_put(variablesGlobales,"juan",&compartida);

	void _enviarComaprtida(void){

		cpu1 = newCliente(accept(*socketCPU,NULL,NULL));

			operarCompartida(cpu1,escribirCompartida);
		}

	pthread_create(&hiloPCP,NULL,(void*(*)(void*))_enviarComaprtida,NULL);
	sleep(1);
	pthread_create(&hiloCPU,NULL,(void*(*)(void*))stubEscribirCompartidaInexistenteFalla,(void*) &error);

	pthread_join(hiloCPU,NULL);
	pthread_join(hiloPCP,NULL);

	CU_ASSERT_EQUAL(error,ESCRIBIR_COMPARTIDAINEXISTENTE);
	CU_ASSERT_EQUAL(list_size(colaCPUsEnUso->cola),0);

	LiberarEstructurasGlobales
}

void testEnviarCompartidaInexistenteFalla(void){

	CrearEstructurasGlobales

	int compartida = 5;
	pthread_t hiloPCP, hiloCPU;
	int* socketCPU = malloc(sizeof(int));
	*socketCPU = crearSocketEscuchaSobre("7013",2);
	t_cliente* cpu1;
	int compartidaEnviada;

	dictionary_put(variablesGlobales,"juan",&compartida);

	void _enviarComaprtida(void){

		cpu1 = newCliente(accept(*socketCPU,NULL,NULL));


			operarCompartida(cpu1,enviarCompartida);
		}

	pthread_create(&hiloPCP,NULL,(void*(*)(void*))_enviarComaprtida,NULL);
	sleep(1);
	pthread_create(&hiloCPU,NULL,(void*(*)(void*))stubEnviarCompartidaFalla,(void*) &compartidaEnviada);

	pthread_join(hiloCPU,NULL);
	pthread_join(hiloPCP,NULL);

	CU_ASSERT_EQUAL(compartidaEnviada,LEER_COMPARTIDAINEXISTENTE);
	CU_ASSERT_EQUAL(list_size(colaCPUsEnUso->cola),0);

	LiberarEstructurasGlobales
}

void testImprimir(void){

	CrearEstructurasGlobales

	sem_t* sem=malloc(sizeof(sem_t));
	pthread_t hiloPCP, hiloCPU,hiloPP;
	int* socket = malloc(sizeof(int));
	*socket = crearSocketEscuchaSobre("7017",2);
	tipoProceso* proceso1=malloc(sizeof(tipoProceso));
	t_cliente* cpu1;
	int* pp=malloc(sizeof(int));
	int valor =5;
	char* rta = malloc(2*TAMANIO_INT);

	sem_init(sem,0,0);

	void _imprimir(void){

			*pp = accept(*socket,NULL,NULL);
			proceso1->socketPP=*pp;
			sem_post(sem);
			cpu1 = newCliente(accept(*socket,NULL,NULL));
			char* key = string_itoa(cpu1->socket);
			dictionary_put(procesosEnEjecucion,key,proceso1);
			free(key);

			imprimir(cpu1);
		}

	pthread_create(&hiloPCP,NULL,(void*(*)(void*))_imprimir,NULL);
	sleep(1);
	pthread_create(&hiloPP,NULL,(void*(*)(void*))stubImprimirPP,(void*)rta);
	sem_wait(sem);
	pthread_create(&hiloCPU,NULL,(void*(*)(void*))stubImprimirCPU,(void*) &valor);

	pthread_join(hiloCPU,NULL);
	pthread_join(hiloPP,NULL);
	pthread_join(hiloPCP,NULL);

	CU_ASSERT_EQUAL(*(int*)rta,0);
	CU_ASSERT_EQUAL(*(rta+TAMANIO_INT),valor);
	t_cliente* cpuReencolado = list_get(colaCPUsEnUso->cola,0);
	CU_ASSERT_EQUAL(cpuReencolado->socket,cpu1->socket);


	LiberarEstructurasGlobales
}

void testImprimirTexto(void){

	CrearEstructurasGlobales

	sem_t* sem=malloc(sizeof(sem_t));
	pthread_t hiloPCP, hiloCPU,hiloPP;
	int* socket = malloc(sizeof(int));
	*socket = crearSocketEscuchaSobre("7020",2);
	tipoProceso* proceso1=malloc(sizeof(tipoProceso));
	t_cliente* cpu1;
	int* pp=malloc(sizeof(int));
	char* valor ="jose";
	char* rta = malloc(TAMANIO_INT+5);

	sem_init(sem,0,0);

	void _imprimirTexto(void){

			*pp = accept(*socket,NULL,NULL);
			proceso1->socketPP=*pp;
			sem_post(sem);
			cpu1 = newCliente(accept(*socket,NULL,NULL));
			char* key = string_itoa(cpu1->socket);
			dictionary_put(procesosEnEjecucion,key,proceso1);
			free(key);

			imprimirTexto(cpu1);

		}

	pthread_create(&hiloPCP,NULL,(void*(*)(void*))_imprimirTexto,NULL);
	sleep(1);
	pthread_create(&hiloPP,NULL,(void*(*)(void*))stubImprimirTextoPP,(void*)rta);
	sem_wait(sem);
	pthread_create(&hiloCPU,NULL,(void*(*)(void*))stubImprimirTextoCPU,(void*) valor);

	pthread_join(hiloCPU,NULL);
	pthread_join(hiloPP,NULL);
	pthread_join(hiloPCP,NULL);

	CU_ASSERT_EQUAL(*(int*)rta,1);
	CU_ASSERT(string_equals_ignore_case(rta+TAMANIO_INT,valor));
	t_cliente* cpuReencolado = list_get(colaCPUsEnUso->cola,0);
	CU_ASSERT_EQUAL(cpuReencolado->socket,cpu1->socket);

	LiberarEstructurasGlobales
}

void testDespachar(void){

	CrearEstructurasGlobales

	pthread_t hiloPCP, hiloCPU;
	int* socketCPU = malloc(sizeof(int));
	*socketCPU = crearSocketEscuchaSobre("7011",2);
	tipoProceso* proceso1=malloc(sizeof(tipoProceso));
	tipoProceso* procesoEnviado=malloc(sizeof(tipoProceso));
	tipoPCB* pcb1= (tipoPCB*) newPCB(0,1,2,3,4,5,6,7,8);
	t_cliente* cpu1;

	proceso1->PCB=*pcb1;

	encolar(colaProcesosListos,proceso1);

	void _despachar(void){

		void _salir(int a){
		pthread_exit(0);
		}
		signal(SIGUSR1,_salir);

		cpu1 = newCliente(accept(*socketCPU,NULL,NULL));

		encolar(colaCPUsListos,cpu1);

		despacharProcesos();
	}

	pthread_create(&hiloPCP,NULL,(void*(*)(void*))_despachar,NULL);
	sleep(1);
	pthread_create(&hiloCPU,NULL,(void*(*)(void*))stubDespachar,(void*) procesoEnviado);

	pthread_join(hiloCPU,NULL);
	pthread_kill(hiloPCP,SIGUSR1);
	pthread_join(hiloPCP,NULL);

	CU_ASSERT(list_is_empty(colaProcesosListos->cola));
	CU_ASSERT(list_is_empty(colaCPUsListos->cola));
	CU_ASSERT_EQUAL(procesoEnviado->PCB.identificador,0);
	CU_ASSERT_EQUAL((procesoEnviado->PCB).segmentoDeCodigo,1);
	CU_ASSERT_EQUAL(procesoEnviado->PCB.segmentoDeStack,2);
	CU_ASSERT_EQUAL(procesoEnviado->PCB.cursorDelStack,3);
	CU_ASSERT_EQUAL(procesoEnviado->PCB.indiceDeCodigo,4);
	CU_ASSERT_EQUAL(procesoEnviado->PCB.indiceEtiquetas,5);
	CU_ASSERT_EQUAL(procesoEnviado->PCB.programCounter,6);
	CU_ASSERT_EQUAL(procesoEnviado->PCB.tamanioContextoActual,7);
	CU_ASSERT_EQUAL(procesoEnviado->PCB.tamanioIndiceEtiquetas,8);

	char* scpu=string_itoa(cpu1->socket);
	tipoProceso* procesoEjecutado = dictionary_get(procesosEnEjecucion,scpu);
	free(scpu);

	CU_ASSERT_EQUAL(procesoEjecutado->PCB.identificador,0);
	CU_ASSERT_EQUAL((procesoEjecutado->PCB).segmentoDeCodigo,1);
	CU_ASSERT_EQUAL(procesoEjecutado->PCB.segmentoDeStack,2);
	CU_ASSERT_EQUAL(procesoEjecutado->PCB.cursorDelStack,3);
	CU_ASSERT_EQUAL(procesoEjecutado->PCB.indiceDeCodigo,4);
	CU_ASSERT_EQUAL(procesoEjecutado->PCB.indiceEtiquetas,5);
	CU_ASSERT_EQUAL(procesoEjecutado->PCB.programCounter,6);
	CU_ASSERT_EQUAL(procesoEjecutado->PCB.tamanioContextoActual,7);
	CU_ASSERT_EQUAL(procesoEjecutado->PCB.tamanioIndiceEtiquetas,8);
	CU_ASSERT_EQUAL(list_size(colaCPUsEnUso->cola),1);
	t_cliente* cpuEjecucion = list_get(colaCPUsEnUso->cola,0);
	CU_ASSERT_EQUAL(cpuEjecucion->socket,cpu1->socket);


	LiberarEstructurasGlobales
}

void testsDespacharYRecibir(void){

	CrearEstructurasGlobales

	sem_t* sem = malloc(sizeof(sem_t));
	sem_t* semCPU1 = malloc(sizeof(sem_t));
	pthread_t hiloPCP, hiloCPU1,hiloCPU2;
	int* socketCPU = malloc(sizeof(int));
	*socketCPU = crearSocketEscuchaSobre("6015",5);
	tipoProceso* proceso1=malloc(sizeof(tipoProceso));
	tipoProceso* proceso2=malloc(sizeof(tipoProceso));
	tipoPCB* pcb1= (tipoPCB*) newPCB(0,1,2,3,4,5,6,7,8);
	tipoPCB* pcb2= (tipoPCB*) newPCB(10,11,12,13,14,15,16,17,18);
	t_cliente* cpu1;
	t_cliente* cpu2;

	sem_init(sem,0,0);
	sem_init(semCPU1,0,0);

	proceso1->PCB=*pcb1;
	proceso2->PCB=*pcb2;

	void _escucharPedidosCPU(void){

		void _salir(int a){
			pthread_exit(0);
		}
		signal(SIGUSR1,_salir);

		char* streamPCB;

		cpu1 = newCliente(accept(*socketCPU,NULL,NULL));
		sem_post(sem);
		cpu2 = newCliente(accept(*socketCPU,NULL,NULL));

		//-----------------------------------[DISPATCHER>----------------------------------------//
		streamPCB = serializarPCB(proceso1->PCB);
		enviar(cpu1->socket,streamPCB,sizeof(int)*10);
		encolar(colaCPUsEnUso, cpu1);
		char* cpu=string_itoa(cpu1->socket);
		//wait mutexprintf("enviados pcbs");
		dictionary_put(procesosEnEjecucion,cpu,proceso1);
		//signal mutex
		free(cpu);

		streamPCB = serializarPCB(proceso2->PCB);
		enviar(cpu2->socket,streamPCB,sizeof(int)*10);
		encolar(colaCPUsEnUso, cpu2);
		cpu=string_itoa(cpu2->socket);
		//wait mutex
		dictionary_put(procesosEnEjecucion,cpu,proceso2);
		//signal mutex
		free(cpu);
		//-----------------------------------<DISPATCHER]----------------------------------------//

		escucharPedidosCPU();
	}

	pthread_create(&hiloPCP,NULL,(void*(*)(void*))_escucharPedidosCPU,NULL);
	sleep(1);
	pthread_create(&hiloCPU1,NULL,(void*(*)(void*))stubEscucharPedidosNoVuelve,semCPU1);
	sem_wait(sem);
	pthread_create(&hiloCPU2,NULL,(void*(*)(void*))stubEscucharPedidosVuelve,NULL);

	pthread_join(hiloCPU2,NULL);
	t_cliente* CPUDevuelto = retirar(colaCPUsListos);
	pthread_kill(hiloPCP,SIGUSR1);
	pthread_join(hiloPCP,NULL);
	sem_post(semCPU1);
	pthread_join(hiloCPU1,NULL);

	close(*socketCPU);
	CU_ASSERT_EQUAL(CPUDevuelto->socket,cpu2->socket);
	CU_ASSERT_EQUAL(list_size(colaCPUsListos->cola),0);//Si saque 1 y quedan 0 es porque solo estaba el que saque

	LiberarEstructurasGlobales
}

void testSeleccionar(void){

	CrearEstructurasGlobales

	sem_t* sem1=malloc(sizeof(sem_t));
	sem_t* sem2=malloc(sizeof(sem_t));
	sem_t* sem3=malloc(sizeof(sem_t));
	sem_t* sem4=malloc(sizeof(sem_t));
	pthread_t hiloPCP, hiloCPU1,hiloCPU2,hiloCPU3;
	int* socketCPU = malloc(sizeof(int));
	*socketCPU = crearSocketEscuchaSobre("7009",3);
	t_cliente* cpu1;
	t_cliente* cpu2;
	t_cliente* cpu3;
	t_list* CPUsDespachados = list_create();
	t_list* CPUsListos;

	sem_init(sem1,0,0);
	sem_init(sem2,0,0);
	sem_init(sem3,0,0);
	sem_init(sem4,0,0);

	void _elegir(void){
		cpu1 = newCliente(accept(*socketCPU,NULL,NULL));
		sem_post(sem1);
		cpu2 = newCliente(accept(*socketCPU,NULL,NULL));
		sem_post(sem2);
		cpu3 = newCliente(accept(*socketCPU,NULL,NULL));

		log_debug(logDebug,"stubCPUs se conectaron");

		list_add(CPUsDespachados,cpu1);
		list_add(CPUsDespachados,cpu2);
		list_add(CPUsDespachados,cpu3);

		sem_wait(sem3);
		sem_wait(sem4);

		CPUsListos = seleccionarListosLectura(CPUsDespachados);log_debug(logDebug,"hay %d CPUs listos",list_size(CPUsListos));

		int termina =1;
		enviar(cpu2->socket,&termina,sizeof(termina));
	}

	pthread_create(&hiloPCP,NULL,(void*(*)(void*))_elegir,NULL);
	sleep(1);
	pthread_create(&hiloCPU1,NULL,(void*(*)(void*))stubSelectVuelve,(void*) sem4);
	sem_wait(sem1);
	pthread_create(&hiloCPU2,NULL,(void*(*)(void*))stubSelectNoVuelve,NULL);
	sem_wait(sem2);
	pthread_create(&hiloCPU3,NULL,(void*(*)(void*))stubSelectVuelve,(void*)sem3);

	pthread_join(hiloPCP,NULL);
	pthread_join(hiloCPU1,NULL);
	pthread_join(hiloCPU2,NULL);
	pthread_join(hiloCPU3,NULL);

	CU_ASSERT_EQUAL(list_size(CPUsDespachados),3);
	CU_ASSERT_EQUAL(list_size(CPUsListos),2);
	t_cliente* cpuListoA = list_get(CPUsListos,0);
	t_cliente* cpuListoB = list_get(CPUsListos,1);
	CU_ASSERT_EQUAL(cpuListoA->socket,cpu1->socket);
	CU_ASSERT_EQUAL(cpuListoB->socket,cpu3->socket);

	LiberarEstructurasGlobales
}

/* TODO test de que bloquee-- igual lo probe y anda, pero para q quede
void testSeleccionarBloquea(void){

	CrearEstructurasGlobales

	pthread_t hiloPCP, hiloCPU1,hiloCPU2,hiloCPU3;
	int* socketCPU = malloc(sizeof(int));
	*socketCPU = crearSocketEscuchaSobre("7001",3);
	int* cpu1=malloc(sizeof(int));
	int* cpu2=malloc(sizeof(int));
	int* cpu3=malloc(sizeof(int));
	t_list* CPUsDespachados = list_create();
	t_list* CPUsListos;

	void _elegir(void){

		printf("hola");

		*cpu1 = accept(*socketCPU,NULL,NULL);
		*cpu2 = accept(*socketCPU,NULL,NULL);
		*cpu3 = accept(*socketCPU,NULL,NULL);

		printf("Se conectaron");

		list_add(CPUsDespachados,cpu1);
		list_add(CPUsDespachados,cpu2);
		list_add(CPUsDespachados,cpu3);


		printf("me bloqueare");

		CPUsListos = seleccionarListosLectura(CPUsDespachados);

		printf("Si aparezco yo, algo anda mal");

		int termina =1;
		send(*cpu2,&termina,sizeof(termina),0);
	}

	pthread_create(&hiloPCP,NULL,(void*(*)(void*))_elegir,NULL);
	sleep(1);
	pthread_create(&hiloCPU1,NULL,(void*(*)(void*))stubSelectNoVuelve,NULL);
	sleep(1);
	pthread_create(&hiloCPU2,NULL,(void*(*)(void*))stubSelectNoVuelve,NULL);
	sleep(1);
	pthread_create(&hiloCPU3,NULL,(void*(*)(void*))stubSelectNoVuelve,NULL);

	pthread_join(hiloPCP,NULL);
	pthread_join(hiloCPU1,NULL);
	pthread_join(hiloCPU2,NULL);
	pthread_join(hiloCPU3,NULL);


	LiberarEstructurasGlobales
}*/

void testReencolarCPSEnUso(void){

	CrearEstructurasGlobales

	int cpu1=1,cpu2=2,cpu3=3;
	t_list* cpus=list_create();

	list_add(cpus,&cpu1);
	list_add(cpus,&cpu2);
	list_add(cpus,&cpu3);

	reencolarCPUsEnUso(cpus);

	int *desencolado1,*desencolado2,*desencolado3;

	desencolado1=list_get(colaCPUsEnUso->cola,0);
	desencolado2=list_get(colaCPUsEnUso->cola,1);
	desencolado3=list_get(colaCPUsEnUso->cola,2);

	CU_ASSERT_EQUAL(list_size(colaCPUsEnUso->cola),3);
	CU_ASSERT_EQUAL(*desencolado1,1);
	CU_ASSERT_EQUAL(*desencolado2,2);
	CU_ASSERT_EQUAL(*desencolado3,3);

	LiberarEstructurasGlobales
}

void testDesencolarCPSEnUso(void){

	CrearEstructurasGlobales

	int cpu1=1,cpu2=2,cpu3=3;

	encolar(colaCPUsEnUso,&cpu1);
	encolar(colaCPUsEnUso,&cpu2);
	encolar(colaCPUsEnUso,&cpu3);

	t_list* desencolados = desencolarCPUsEnUso();

	int *desencolado1,*desencolado2,*desencolado3;

	desencolado1=list_get(desencolados,0);
	desencolado2=list_get(desencolados,1);
	desencolado3=list_get(desencolados,2);

	CU_ASSERT_EQUAL(list_size(desencolados),3);
	CU_ASSERT_EQUAL(*desencolado1,1);
	CU_ASSERT_EQUAL(*desencolado2,2);
	CU_ASSERT_EQUAL(*desencolado3,3);

	LiberarEstructurasGlobales
}

/* TODO test de que bloquee-- igual lo probe y anda, pero para q quede
void testDesencolarCPSEnUsoBloquea(void){

	CrearEstructurasGlobales

	int cpu1=1,cpu2=2,cpu3=3;

	printf("me bloquerare");

	t_list* desencolados = desencolarCPUsEnUso();

	LiberarEstructurasGlobales
}*/

void testRecuperarProceso(void){

	CrearEstructurasGlobales

	pthread_t hiloPCP, hiloCPU;
	int* socketCPU = malloc(sizeof(int));
	*socketCPU = crearSocketEscuchaSobre("7008",2);
	tipoProceso* proceso=malloc(sizeof(tipoProceso));
	t_cliente* cpu1;

	void _recuperarProceso(int* socket){

		cpu1 = newCliente(accept(*socket,NULL,NULL));

		char* scpu=string_itoa(cpu1->socket);
		dictionary_put(procesosEnEjecucion,scpu,proceso);
		free(scpu);
		recuperarProcesoListo(cpu1);
	}

	pthread_create(&hiloPCP,NULL,(void*(*)(void*))_recuperarProceso,socketCPU);
	sleep(1);
	pthread_create(&hiloCPU,NULL,(void*(*)(void*))stubRecuperarPCB,NULL);
	pthread_join(hiloPCP,NULL);
	pthread_join(hiloCPU,NULL);

	t_cliente* CPUDevuelto= retirar(colaCPUsListos);

	CU_ASSERT_EQUAL(cpu1->socket,CPUDevuelto->socket);
	CU_ASSERT_EQUAL(proceso->PCB.identificador,0);
	CU_ASSERT_EQUAL((proceso->PCB).segmentoDeCodigo,1);
	CU_ASSERT_EQUAL(proceso->PCB.segmentoDeStack,2);
	CU_ASSERT_EQUAL(proceso->PCB.cursorDelStack,3);
	CU_ASSERT_EQUAL(proceso->PCB.indiceDeCodigo,4);
	CU_ASSERT_EQUAL(proceso->PCB.indiceEtiquetas,5);
	CU_ASSERT_EQUAL(proceso->PCB.programCounter,6);
	CU_ASSERT_EQUAL(proceso->PCB.tamanioContextoActual,7);
	CU_ASSERT_EQUAL(proceso->PCB.tamanioIndiceEtiquetas,8);

	LiberarEstructurasGlobales
}

void testColasSincronizadas(void){

	int i;
	pthread_t hiloConsumidor, hiloProductor;
	t_colaSincronizada* colaCPUsListos=newColaSincronizada();
	t_list* lista=list_create();

	void _producir(void){

		int i;

		for(i=0;i<5;i++){
			int* cpu = malloc(sizeof(int));
			*cpu=i;
			encolar(colaCPUsListos,cpu);
		}
	}

	void _consumir(void){

		int i;

		for(i=0;i<5;i++)
			list_add(lista,retirar(colaCPUsListos));
	}

		pthread_create(&hiloConsumidor,NULL,(void*(*)(void*))_consumir,NULL);
		sleep(1);
		pthread_create(&hiloProductor,NULL,(void*(*)(void*))_producir,NULL);

		pthread_join(hiloConsumidor,NULL);
		pthread_join(hiloProductor,NULL);

		for(i=0;i<5;i++){
			int* a=list_remove(lista,0);
			CU_ASSERT_EQUAL(*a,i)
		}

}

void testSerializarPCB(void){


	tipoPCB PCB;
	int *streamPCB,i;

	PCB.identificador=0;
	PCB.segmentoDeCodigo=1;
	PCB.segmentoDeStack=2;
	PCB.cursorDelStack=3;
	PCB.indiceDeCodigo=4;
	PCB.indiceEtiquetas=5;
	PCB.programCounter=6;
	PCB.tamanioContextoActual=7;
	PCB.tamanioIndiceEtiquetas=8;

	streamPCB = (int*) serializarPCB(PCB);

	for(i=0;i<9;i++)
		CU_ASSERT_EQUAL(streamPCB[i],i);

	CU_ASSERT_EQUAL(streamPCB[9],quantum);
}

void testListComplement(void){

	t_list *original= list_create(), *complemento, *sublista = list_create();
	int *originales=malloc(5*sizeof(int));
	int *cero,*dos,*cuatro,i;

	for(i=0;i<5;i++)
		originales[i]=i;

	list_add(original,originales);
	list_add(original,originales+1);
	list_add(original,originales+2);
	list_add(original,originales+3);
	list_add(original,originales+4);

	list_add(sublista,originales+1);
	list_add(sublista,originales+3);

	complemento = list_complement(original,sublista);

	CU_ASSERT_EQUAL(list_size(complemento),3)

	cero = list_get(complemento,0);
	dos = list_get(complemento,1);
	cuatro = list_get(complemento,2);

	CU_ASSERT_EQUAL(*cero, 0)
	CU_ASSERT_EQUAL(*dos,2)
	CU_ASSERT_EQUAL(*cuatro,4)
}

int runTests(void){

	CU_initialize_registry();

	CU_pSuite suiteTestsPCP = CU_add_suite("Suite de Testeo : Planificador Corto Plazo",NULL,NULL);
	CU_add_test(suiteTestsPCP,"List_complement",testListComplement);
	CU_add_test(suiteTestsPCP,"Generar stream de PCB",testSerializarPCB);
	CU_add_test(suiteTestsPCP,"Colas sincronizadas",testColasSincronizadas);
	CU_add_test(suiteTestsPCP,"Recuperar proceso",testRecuperarProceso);
	CU_add_test(suiteTestsPCP,"Seleccionar Listos",testSeleccionar);
	CU_add_test(suiteTestsPCP,"Desencolar CPUs en uso",testDesencolarCPSEnUso);
	CU_add_test(suiteTestsPCP,"Reencolar CPUs en uso",testReencolarCPSEnUso);
	CU_add_test(suiteTestsPCP,"Despachar y recibir",testsDespacharYRecibir);
	CU_add_test(suiteTestsPCP,"Despachar Proceso",testDespachar);
	CU_add_test(suiteTestsPCP,"Enviar Compartida",testEnviarCompartida);
	CU_add_test(suiteTestsPCP,"Enviar Compartida Inexistente Falla",testEnviarCompartidaInexistenteFalla);
	CU_add_test(suiteTestsPCP,"Escribir Compartida",testEscribirCompartida);
	CU_add_test(suiteTestsPCP,"Escribir Compartida Inexistente Falla",testEscribirCompartidaInexistenteFalla);
	CU_add_test(suiteTestsPCP,"Imprimir",testImprimir);
	CU_add_test(suiteTestsPCP,"Imprimir texto",testImprimirTexto);

	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();

	return CU_get_error();
}
