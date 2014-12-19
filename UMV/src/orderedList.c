/*
 * orderedList.c
 *
 *  Created on: 08/05/2014
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include "stdUMV.h"
#include <commons/collections/node.h>
#include <commons/collections/list.h>

void list_add_order_by(t_list* list, void* data, int(*isBigger)(void*,void*)){

	int index=0;

	if(list_is_empty(list)){

		list_add_in_index(list,0,data);
		return;
	}
	while((index<list_size(list)))
		if ((*isBigger)((void*)list_get(list,index), data))
			index++;
		else
			break;
	list_add_in_index(list,index,data);
	return;
}


//Funcion para pruebas
int esMayor(void* actual, void* nuevo){
	return *(int*)nuevo>*(int*)actual;
}
