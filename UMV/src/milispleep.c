/*
 * milispleep.c
 *
 *  Created on: 22/06/2014
 *      Author: utnso
 */

#include "stdUMV.h"

void milisleep(unsigned int milisegundos){
	usleep(1000*milisegundos);
	return;
}
