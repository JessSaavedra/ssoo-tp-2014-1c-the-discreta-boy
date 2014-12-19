#include "stdPP.h"

t_log* logDebug;
struct addrinfo criteriosConfiguracion;
struct addrinfo* datosSocketKernel;
int socketKernel;
char* puerto;
char* ip;


int main(int argc, char** argv) {
	logDebug= log_create("debugPP.txt","ProcesoPrograma",false,LOG_LEVEL_DEBUG);

	if (argc!=2) {
		fprintf(stderr,"El proceso programa requiere como parametro el script a ejecutar\n");		log_debug(logDebug,"Fallo en ejecucion por parametros");
		return FALLOEJECUCION;
	}

	if (string_equals_ignore_case(argv[1],"runTests")){
		runTests();
		return 0;
	}

	if (cargarConfiguraciones()) {
		fprintf(stderr,"Error en las configuraciones\n");											log_debug(logDebug,"Fallo en ejecucion por configuraciones");
		return CONFIG_INVALIDA;
	}
																								log_debug(logDebug,"Cargadas configuraciones");
	socketKernel = newSocketServidor();


	connect(socketKernel, datosSocketKernel->ai_addr,datosSocketKernel->ai_addrlen);
	freeaddrinfo(datosSocketKernel);
																								log_debug(logDebug,"Conectado el socket kernel, con errno=%d",errno);

	int errores = enviarScript(argv[1]);
	if (errores){
		fprintf(stderr,"Error en la apertura del archivo");										log_debug(logDebug,"Fallo en apertura de archivo");
		return FALLOENLECTURA;
	}
																								log_debug(logDebug,"Enviado script, esperando header");

	t_header headerRecibido;

	recibirHeader(&headerRecibido);																log_debug(logDebug,"Recibido primer header");
	while (headerRecibido.codigoOperacion!=FINALIZAR){

			void* datosRecibidos = malloc(headerRecibido.tamanioTransmitido);
			recibirDatosCon(headerRecibido.tamanioTransmitido,datosRecibidos);					log_debug(logDebug,"Recibidos datos");

			switch(headerRecibido.codigoOperacion){
				case IMPRIMIRTEXTO:
					imprimirTexto(datosRecibidos);												log_debug(logDebug,"Entre a Imprimir Texto");
					break;
				case IMPRIMIR:
					imprimir(datosRecibidos);													log_debug(logDebug,"Entre a Imprimir Normal");
					break;
			}
			free(datosRecibidos);
			recibirHeader(&headerRecibido);														log_debug(logDebug,"Recibido un nuevo header con exito");
		}
	log_debug(logDebug,"Programa finalizado con exito");
	log_destroy(logDebug);
	liberarConfiguraciones();
	close(socketKernel);
	return 0;
}

