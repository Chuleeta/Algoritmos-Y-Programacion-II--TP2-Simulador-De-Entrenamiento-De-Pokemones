#include "lista.h"
#include <stddef.h>
#include <stdlib.h>

#include <stdio.h>


lista_t* lista_crear(){
    lista_t* lista = calloc(1, sizeof(lista_t));
    if (!lista)
        return NULL;
    
    nodo_t* nodo_principio_aux = calloc(1, sizeof(nodo_t));
    if (!nodo_principio_aux)
        return NULL;
    lista->nodo_inicio = nodo_principio_aux;

    nodo_t* nodo_fin_aux = calloc(1, sizeof(nodo_t));
    if (!nodo_fin_aux)
        return NULL;
    lista->nodo_fin = nodo_fin_aux;

    lista->nodo_inicio = NULL;
    lista->nodo_fin = NULL;
    
    free(nodo_principio_aux);
    free(nodo_fin_aux);
    return lista;
}


lista_t* lista_insertar(lista_t* lista, void* elemento){
    if (!lista)
        return NULL;
    
    nodo_t* nodo_aux = malloc(sizeof(nodo_t));
    if (!nodo_aux)
        return NULL;
    nodo_aux->elemento = elemento;
    nodo_aux->siguiente = NULL;
    
    if (lista->cantidad == 0){
        lista->nodo_inicio = nodo_aux;
    }
    else{
        lista->nodo_fin->siguiente = nodo_aux;
    }
    
    lista->nodo_fin = nodo_aux;
    
    lista->cantidad++;
    return lista;
}


lista_t* lista_insertar_en_posicion(lista_t* lista, void* elemento, size_t posicion){
    if (!lista)
        return NULL;

    nodo_t* nodo_aux = malloc(sizeof(nodo_t));
    if (!nodo_aux)
        return NULL;
    nodo_aux->elemento = elemento;
    nodo_aux->siguiente = NULL;

    if (posicion == 0){
        nodo_aux->siguiente = lista->nodo_inicio;
        lista->nodo_inicio = nodo_aux;
        if (lista->cantidad == 0){
            lista->nodo_fin = nodo_aux;
        }
    }else if(posicion < lista->cantidad){
        nodo_t* actual = lista->nodo_inicio;
        if (posicion > 1){
            for (int i = 1; i < posicion; ++i){
                actual = actual->siguiente;
            }
        }
        nodo_aux->siguiente = actual->siguiente;
        actual->siguiente = nodo_aux;
    }else{
        if (lista->cantidad == 0){
            lista->nodo_inicio = nodo_aux;
            lista->nodo_fin = nodo_aux;
        }else{
            lista->nodo_fin->siguiente = nodo_aux;
            lista->nodo_fin = nodo_aux;
        }
    }

    lista->cantidad++;
    return lista;
}


void* lista_quitar(lista_t* lista){
    if (!lista || lista_vacia(lista))
        return NULL;

    nodo_t* actual = lista->nodo_inicio;
    
    void* resultado;
    if (lista_tamanio(lista) > 1){
        while (actual->siguiente->siguiente != NULL){
            actual = actual->siguiente;
        }
        resultado = actual->siguiente->elemento;
        lista->nodo_fin = actual;
        free(actual->siguiente);
        actual->siguiente = NULL;
    }else{
        resultado = actual->elemento;
        lista->nodo_fin = NULL;
        lista->nodo_inicio = NULL;
        free(actual);
        actual = NULL;
    } 

    lista->cantidad--;
    return resultado;
}

void* lista_quitar_de_posicion(lista_t* lista, size_t posicion){
    if (!lista || lista->cantidad == 0)
        return NULL;
    


    void* resultado;
    if (posicion == 0){
        nodo_t* actual = lista->nodo_inicio;
        lista->nodo_inicio = lista->nodo_inicio->siguiente;
        resultado = actual->elemento;
        free(actual);
        lista->cantidad--;
    }
    else if (posicion >= lista->cantidad){
        resultado = lista_quitar(lista);
    }
    else {
        nodo_t* actual = lista->nodo_inicio;
        if (posicion > 1){
            for (int i = 1; i < posicion; i++){
                actual = actual->siguiente;
            }
        }
    
        nodo_t* proximo_al_actual = actual->siguiente->siguiente;
        resultado = actual->siguiente->elemento;
        free(actual->siguiente);
        actual->siguiente = proximo_al_actual;
        lista->cantidad--;
    }


    return resultado;
}

void* lista_elemento_en_posicion(lista_t* lista, size_t posicion){
    if (!lista || (posicion >= lista->cantidad) || lista->cantidad == 0)
        return NULL;

    if (posicion == 0){
        return lista->nodo_inicio->elemento;
    }
    
    nodo_t* actual = lista->nodo_inicio;
    for (int i = 0; i < posicion; ++i){
        actual = actual->siguiente;
    }
    return actual->elemento;
}

void* lista_primero(lista_t* lista){
    if (!lista || lista->cantidad == 0)
        return NULL;

    return lista->nodo_inicio->elemento;
}

void* lista_ultimo(lista_t* lista){
    if (!lista || lista->cantidad == 0)
        return NULL;

    return lista->nodo_fin->elemento;
}

bool lista_vacia(lista_t* lista){
    if (!lista || lista->cantidad == 0)
        return true;
    return false;
}

size_t lista_tamanio(lista_t* lista){
    if (!lista)
        return 0;

    return lista->cantidad;
}


void liberar_nodos(nodo_t* nodo){
    if (nodo){
    liberar_nodos(nodo->siguiente);
    free(nodo);
    }
}


void lista_destruir(lista_t* lista){
    if (lista){
        liberar_nodos(lista->nodo_inicio);
        free(lista);
    }
}

lista_iterador_t* lista_iterador_crear(lista_t* lista){
    if (!lista)
        return NULL;

    lista_iterador_t* lista_iterador = malloc(sizeof(lista_iterador_t));
    if (!lista_iterador)
        return NULL;

    lista_iterador->lista = lista;
    lista_iterador->corriente = lista->nodo_inicio;
    
    return lista_iterador;
}

bool lista_iterador_tiene_siguiente(lista_iterador_t* iterador){
    if (!iterador || iterador->lista->cantidad < 1)
        return false;
    if((iterador->corriente == iterador->lista->nodo_fin->siguiente))
        return false;
    
    return true;
}


bool lista_iterador_avanzar(lista_iterador_t* iterador){
    if (!lista_iterador_tiene_siguiente(iterador) || !iterador){
        return false;
    }
    if ((iterador->corriente = iterador->corriente->siguiente))
        return true;
    return false;
}

void* lista_iterador_elemento_actual(lista_iterador_t* iterador){
    if (!iterador || !iterador->corriente)
        return NULL;
    return iterador->corriente->elemento;
}

void lista_iterador_destruir(lista_iterador_t* iterador){
    if (iterador){
        free(iterador);
    }
}

size_t lista_con_cada_elemento(lista_t* lista, bool (*funcion)(void*, void*), void *contexto){
    if(!lista || lista->cantidad == 0 || !funcion)
        return 0;

    size_t cantidad_elemenetos_validos = 0;
    nodo_t* actual = lista->nodo_inicio;
    while(actual && funcion(actual->elemento, contexto)){
        actual = actual->siguiente;
        cantidad_elemenetos_validos++;
    }
    return cantidad_elemenetos_validos;
}
