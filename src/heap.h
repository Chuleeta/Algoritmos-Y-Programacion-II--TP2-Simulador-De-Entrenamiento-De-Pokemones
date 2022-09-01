#ifndef __HEAP_H__
#define __HEAP_H__
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>


typedef bool (*funcion_comparador_t)(void*, void*);

//Este es un heap minimal
typedef struct heap{
	void** vector;
	size_t cantidad_de_nodos;
	size_t capacidad;
	funcion_comparador_t funcion;
}heap_t;

/*
Pre Condiciones: Recibe una funcion bool la cual compara dos elementos los
cuales se quieras insertar en el heap, debe devolver true si el primero es mas chico
que el segundo (si es que se quiere hacer un heap minimal)
Post Condiciones: Devuelve un heap vacio
*/
heap_t* crear_heap(bool (*funcion_para_comparar_mas_chico)(void*, void*));


/*
Pre Condiciones: Recibe un heap
Post Condiciones: Muestra el nodo mas chico, de estar vacio el heap
				  devuelve NULL
*/
void* ver_nodo_mas_chico(heap_t* heap);


/*
Pre Condiciones: Recibe un heap
Post Condiciones: Elimina el nodo mas chico, de estar vacio el heap
				  devuelve NULL
*/
void* eliminar_raiz(heap_t* heap);


/*
Pre Condiciones: Recibe un heap y el elemento a insertar
Post Condiciones: Agrega el elemento en el heap y este se acomoda en
				  el heap. Devuelve true si se agrego con exito o false
				  de no ser asi
*/
bool agregar_nodo(heap_t* heap, void* elemento);


/*
Pre Condiciones: Recibe un heap
Post Condiciones: Libera toda la memoria reservada por el heap
*/
void destruir_heap(heap_t* heap);


#endif /* __HEAP_H__ */