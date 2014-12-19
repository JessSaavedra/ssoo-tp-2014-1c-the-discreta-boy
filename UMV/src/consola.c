#include "stdUMV.h"

void consola(void){

	int errorEnParametros,largoMaximo=LARGOMAXIMOCOMANDO,longitudComando;
    char* comando = (char *)malloc(largoMaximo+1);
    char* parametros= (char*) malloc(1);

	printf("Bienvenido a la Unidad de Memoria Virtual.\n");

	while(1){

		errorEnParametros=0;
		printf("comando$ ");

		longitudComando=getline(&comando, (size_t*) &largoMaximo, stdin);
		comando[longitudComando-1]='\0';							//Borra el /n

		if (esElComando(comando,"cambiarAlgoritmo:",&parametros))
			errorEnParametros=cambiarAlgoritmoSegun(parametros);
		else if (esElComando(comando,"compactarMemoria",&parametros))
			compactarMemoria();
		else if (esElComando(comando,"cerrarConsola",&parametros)){
			printf("Saliendo... \nGracias por utilizar la consola\n(AVISO: El resto de los servicios de la UMV sigue en ejecución)\n");
			break;
			}
		else if (esElComando(comando,"salir",&parametros)){
			printf("Saliendo... \nGracias por utilizar la Unidad de Memoria Virtual\n");
			raise(SIGINT);
			}
		else if (esElComando(comando,"setRetardo:",&parametros))
			errorEnParametros=setRetardoEn(parametros);
		else if(esElComando(comando,"leerDe:",&parametros))
			errorEnParametros=operarMemoriaSegun(parametros,leerMemoriaPorTeclado);
		else if(esElComando(comando,"escribirEn:",&parametros))
			errorEnParametros=operarMemoriaSegun(parametros,escribirMemoriaPorTeclado);
		else if(esElComando(comando,"dump:",&parametros))
			errorEnParametros=dumpSegun(parametros);
		else
			fprintf(stderr,"\"%s\": Comando inexistente\n",comando);
		if(errorEnParametros)
			fprintf(stderr,"Parametros incorrectos\n");
		}
	free(comando);
	free(parametros);
	return;
}
