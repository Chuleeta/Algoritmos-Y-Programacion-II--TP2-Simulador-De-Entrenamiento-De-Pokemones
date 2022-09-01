#include "heap.h"

#include <stdio.h>
/*
Pre Condiciones: Recibe una funcion bool la cual compara dos elementos los
cuales se quieras insertar en el heap, debe devolver true si el primero es mas chico
que el segundo (si es que se quiere hacer un heap minimal)
Post Condiciones: Devuelve un heap vacio
*/
heap_t* crear_heap(bool (*funcion_para_comparar_mas_chico)(void*, void*)){
	heap_t* heap = calloc(1, sizeof(heap_t));
	void** vector = calloc(5, sizeof(void*));
	if(!heap || !vector || !funcion_para_comparar_mas_chico){
		free(heap);
		free(vector);
		return NULL;
	}
	heap->vector = vector;
	heap->capacidad = 5;
	heap->funcion = funcion_para_comparar_mas_chico;

	return heap;
}


/*
Pre Condiciones: Recibe un heap
Post Condiciones: Muestra el nodo mas chico, de estar vacio el heap
				  devuelve NULL
*/
void* ver_nodo_mas_chico(heap_t* heap){
	if(!heap)
		return NULL;
	
	return heap->vector[0];
}


void sift_down(heap_t* heap, size_t posicion){
	if(!heap || (heap->cantidad_de_nodos == 0))
		return;
	if((2*posicion)+1 <= (heap->cantidad_de_nodos - 1) &&
		!(heap->funcion(heap->vector[posicion], heap->vector[(2*posicion)+1])) &&
		!(heap->funcion(heap->vector[(2*posicion)+2], heap->vector[(2*posicion)+1]))){
		
		void* aux = heap->vector[posicion];
		heap->vector[posicion] = heap->vector[(2*posicion)+1];
		heap->vector[(2*posicion)+1] = aux;
		sift_down(heap, (2*posicion)+1);
	}
	else if((2*posicion)+2 <= (heap->cantidad_de_nodos - 1) &&
		!(heap->funcion(heap->vector[posicion], heap->vector[(2*posicion)+2])) &&
		(heap->funcion(heap->vector[(2*posicion)+2], heap->vector[(2*posicion)+1]))){
		
		void* aux = heap->vector[posicion];
		heap->vector[posicion] = heap->vector[(2*posicion)+2];
		heap->vector[(2*posicion)+2] = aux;
		sift_down(heap, (2*posicion)+2);
	}
}


/*
Pre Condiciones: Recibe un heap
Post Condiciones: Elimina el nodo mas chico, de estar vacio el heap
				  devuelve NULL
*/
void* eliminar_raiz(heap_t* heap){
	if(!heap || heap->cantidad_de_nodos == 0)
		return NULL;

	void* raiz = heap->vector[0];
	heap->vector[0] = heap->vector[(heap->cantidad_de_nodos) - 1];
	heap->vector[(heap->cantidad_de_nodos) - 1] = NULL;
	heap->cantidad_de_nodos--;
	sift_down(heap, 0);

	return raiz;
}




void sift_up(heap_t* heap, size_t index_actual){
	if(!heap || (heap->cantidad_de_nodos < 1))
		return;
	size_t index_padre = 0;
	if(index_actual == 0)
		return;
	else if(index_actual%2 == 0)
		index_padre = ((index_actual)-2)/2;
	else
		index_padre = ((index_actual)-1)/2;

	if(heap->funcion(heap->vector[index_actual], heap->vector[index_padre])){
		void* aux = heap->vector[index_actual];
		heap->vector[index_actual] = heap->vector[index_padre];
		heap->vector[index_padre] = aux;
		sift_up(heap, index_padre);
	}
}


/*
Pre Condiciones: Recibe un heap y el elemento a insertar
Post Condiciones: Agrega el elemento en el heap y este se acomoda en
				  el heap. Devuelve true si se agrego con exito o false
				  de no ser asi
*/
bool agregar_nodo(heap_t* heap, void* elemento){
	if(!heap)
		return false;
	if(((100*(heap->cantidad_de_nodos))/heap->capacidad) >= 85){
		heap->capacidad *= 2;
		heap->vector = realloc(heap->vector, (heap->capacidad)*sizeof(void*));
	}

	heap->vector[(heap->cantidad_de_nodos)] = elemento;
	sift_up(heap, heap->cantidad_de_nodos);
	heap->cantidad_de_nodos++;
	
	return true;
}


/*
Pre Condiciones: Recibe un heap
Post Condiciones: Libera toda la memoria reservada por el heap
*/
void destruir_heap(heap_t* heap){
	if(!heap)
		return;
	free(heap->vector);
	free(heap);
}