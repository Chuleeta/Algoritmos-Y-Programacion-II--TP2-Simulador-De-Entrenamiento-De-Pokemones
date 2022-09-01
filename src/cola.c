#include "cola.h"
#include "lista.h"

#include <stdio.h>


struct _cola_t{
    lista_t* lista;
};

cola_t* cola_crear(){
    lista_t* lista = lista_crear();
    cola_t* cola = malloc(sizeof(cola_t));
    if(!cola)
        return NULL;
    cola->lista = lista;
    return cola;
}

cola_t* cola_encolar(cola_t* cola, void* elemento){
    if(!cola)
        return NULL;
    cola->lista = lista_insertar(cola->lista, elemento);
    return cola;
}

void* cola_desencolar(cola_t* cola){
    if (!cola || cola_tamanio(cola) == 0){
        return NULL;
    }
    
    return lista_quitar_de_posicion(cola->lista, 0);
}

void* cola_frente(cola_t* cola){
    if(!cola || cola->lista->cantidad == 0)
        return NULL;

    return cola->lista->nodo_inicio->elemento;
}

size_t cola_tamanio(cola_t* cola){
    if(!cola || cola->lista->cantidad == 0)
        return 0;
    return cola->lista->cantidad;
}

bool cola_vacia(cola_t* cola){
    return (cola_tamanio(cola) == 0);
}

void cola_destruir(cola_t* cola){
    if(cola){
        lista_destruir(cola->lista);
        free(cola);
    }
}
