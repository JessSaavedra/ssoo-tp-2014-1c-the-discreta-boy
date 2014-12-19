/*
 * milispleep.c
 *
 *  Created on: 22/06/2014
 *      Author: utnso
 */

#include "kernel.h"

void milisleep(unsigned int milisegundos){
	usleep(1000*milisegundos);
	return;
}
