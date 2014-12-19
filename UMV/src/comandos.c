#include "stdUMV.h"

//Funciones Auxiliares

void imprimirError(int error){
	if(error==PROCESOINEXISTENTE)
		fprintf(stderr,"El proceso es inexistente\n");
	else if(error==SEGMENTOINEXISTENTE)
		fprintf(stderr,"El segmento es inexistente\n");
	else if (error == SINESPACIO)
		fprintf(stderr,"No hay espacio suficiente para crear el segmento\n");
	else if(error == PROCESOYAEXISTE)
		fprintf(stderr, "El proceso que se intento crear ya existe en el sistema\n");
	else if (error == SEGMENTATIONFAULT)
		fprintf(stderr,"Violacion de segmento: se ha intentado acceder a una posicion fuera de los limites del segmento\n");
	else
		fprintf(stderr,"Se ha producido un error desconocido\n");
	return;
}

int stringANumero(char* cadena, long* numero){

	char*primerCaracterNoNumerico;

	errno=0;
	*numero = strtol(cadena,&primerCaracterNoNumerico,10);
	if (LONG_MIN==*numero || LONG_MAX==*numero || errno !=0|| *primerCaracterNoNumerico!='\0') //Chequeo de errores: overflow, underflow, caracteres no numericos
		return 1;
	return 0;
}

int obtenerParametrosNumericos(char* cadenaParametrosEntrada, long* parametrosNumericos, int cantidadParametrosEsperados){

	char** parametrosSeparados;
	int i=0,errores=0;

	parametrosSeparados=string_split(++cadenaParametrosEntrada, " ");
	while(parametrosSeparados[i]!=NULL)
		i++;
	if( i != cantidadParametrosEsperados){
		free(parametrosSeparados);
		return 1;
	}
	for(i=0;i<cantidadParametrosEsperados;i++){
		errores+=stringANumero(parametrosSeparados[i],&parametrosNumericos[i]);
		free(parametrosSeparados[i]);
	}
	free(parametrosSeparados);
	if(errores)
		return 1;
	return 0;

}

void escribirMemoriaPorTeclado(int proceso, int baseSegmento, int desplazamiento, int tamanioBloque){

	char* buffer = (char*) malloc(tamanioBloque);
	int largoLineaIngresada,largoLineaEsperado=tamanioBloque;
	int error;


	printf("Ingrese datos a escribir:\n");
	largoLineaIngresada=getline(&buffer,&largoLineaEsperado,stdin)-1; //-1 para no contar el '\n'

	if(largoLineaIngresada!=tamanioBloque){
		fprintf(stderr,"Los datos ingresados no coinciden con el tamaño de buffer solicitado\n");
		free(buffer);
		return;
	}
	error = escribirMemoria(proceso,baseSegmento,desplazamiento,tamanioBloque,(void*)buffer);
	if(error)
		imprimirError(error);
	free(buffer);
	return;
}

void leerMemoriaPorTeclado(int proceso, int baseSegmento, int desplazamiento, int tamanioBloque){

	char* datos;
	int error = leerMemoria(proceso, baseSegmento, desplazamiento,tamanioBloque,(void**) &datos);
	if(error){
		imprimirError(error);
		return;
	}
	int i;
	t_segmento* segmento;
pthread_rwlock_rdlock(&lockTablasDeSegmentos);
	getSegmentoActual(proceso, baseSegmento,&segmento);
pthread_rwlock_unlock(&lockTablasDeSegmentos);

	printf ("Direccion Virtual | Direccion Fisica | Contenido\n");
	for(i=0; i<tamanioBloque; i++){
	pthread_rwlock_rdlock(&segmento->lock);
		printf("%10u        | %10u       | Ox%X\n",segmento->base+desplazamiento+i,segmento->ubicacionFisica+desplazamiento+i,datos[i]);
	pthread_rwlock_unlock(&segmento->lock);
	}
	free(datos);
	return;
}

//Funciones que resuelven comandos

int dumpSegun(void* comandoConParametros){
	int erroresEnParametros;
	char* parametros= (char*) malloc(1);

	if(esElComando(comandoConParametros," contenido",&parametros)){
		erroresEnParametros=dumpContenido(parametros);
	}else if(esElComando(comandoConParametros," estructuras",&parametros)){
		if(string_is_empty(parametros))
			erroresEnParametros=dumpTodasLasEstructuras();
		else
			erroresEnParametros=dumpEstructurasDe(parametros);
	}
	else if(esElComando(comandoConParametros," memoria",&parametros))
		erroresEnParametros=dumpMemoriaCompleta();


	return erroresEnParametros;
}

int cambiarAlgoritmoSegun(char* parametro){
	if(string_equals_ignore_case(parametro," WF"))
		setAlgoritmo(posicionWorstFit);
	else if(string_equals_ignore_case(parametro," FF"))
		setAlgoritmo(posicionFirstFit);
	else
		return 1;
	return 0;
}

int setRetardoEn(char* parametros){

	int error;
	long retardoASetear;

	error= stringANumero(++parametros, &retardoASetear);
	retardo = retardoASetear;

	return error;
}

int operarMemoriaSegun(char* parametros,void (*operacion)(int, int, int, int)){

	long* parametrosNumericos = (long*)malloc(4*sizeof(long));

	if(obtenerParametrosNumericos(parametros,parametrosNumericos,4)!=0){
		free(parametrosNumericos);
		return 1;
	}
	(*operacion)(parametrosNumericos[0],parametrosNumericos[1],parametrosNumericos[2],parametrosNumericos[3]);
	free(parametrosNumericos);
	return 0;

	}
