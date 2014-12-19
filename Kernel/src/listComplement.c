/*
 * listComplement.c
 *
 *  Created on: 06/06/2014
 *      Author: utnso
 */

#include <stdbool.h>
#include <commons/collections/list.h>

t_list* list_complement(t_list* original, t_list* sublist){

	bool _isNotInSublist(void* anElement){

		bool _equalsElement(void* anotherElement){
			return anElement == anotherElement;
		}
		return !list_any_satisfy(sublist,_equalsElement);
	}
	return list_filter(original, _isNotInSublist);
}
