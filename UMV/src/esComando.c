#include "stdUMV.h"

int esElComando(char* comandoIngresado, char* comandoEsperado,char** parametros){

	if(string_starts_with(comandoIngresado,comandoEsperado)){
			free(*parametros);
		*parametros=string_substring_from(comandoIngresado,strlen(comandoEsperado));
		return 1;
	}
	else
		return 0;
}
