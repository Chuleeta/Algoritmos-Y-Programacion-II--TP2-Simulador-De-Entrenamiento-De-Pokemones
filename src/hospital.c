#include "hospital.h"
#include "split.h"
#include "lista.h"
#include <string.h>
#include <stdio.h>

#define FIN '\0'
#define SALTO_DE_LINEA '\n'

struct _hospital_pkm_t{
    size_t cantidad_pokemon;
    lista_t* vector_pokemones;

    lista_t* entrenadores;
    size_t cantidad_entrenadores;
};

struct _pkm_t{
    char* nombre;
    entrenador_t* entrenador;
    size_t nivel;
};

struct _entrenador_t{
    char* nombre;
    size_t id;
};


/*
*   Pre condiciones: recibe el nombre de un archivo.
*
*   Post condiciones: lee una linea y la devuelve.
*
*/
char* leer_linea_de_archivo(FILE* archivo){
    size_t caracteres_leidos = 0;
    size_t tope_dinamico = 200;
    char* linea_leida = malloc(tope_dinamico*sizeof(char));

    if (!linea_leida)
        return NULL;

    while(fgets(linea_leida + caracteres_leidos, (int)(tope_dinamico - caracteres_leidos), archivo)){
        size_t leido = strlen(linea_leida + caracteres_leidos);
        
        if (leido > 0 && *(linea_leida + caracteres_leidos + leido - 1) == SALTO_DE_LINEA){
            *(linea_leida + caracteres_leidos + leido - 1) = FIN;
            return linea_leida;
        }
        else{
            char* auxiliar = realloc(linea_leida, tope_dinamico*2);
            
            if (!auxiliar){
                free(auxiliar);
                return NULL;
            }
            
            linea_leida = auxiliar;
            tope_dinamico *= 2;
        }
        caracteres_leidos += leido;
    }

    if (caracteres_leidos == 0){
        free(linea_leida);
        return NULL;
    }
    return linea_leida;
}


/*
*   Pre condiciones: recibe un string y un separador en especifico.
*
*   Post condiciones: cuenta la cantidad de veces que aparece el separador en el string dado.
*
*/
size_t  contar_separadores(char* string, char separador){
    size_t  cantidad_separadores = 0;
    size_t i = 0;
    
    while(string[i] != FIN){
        if(string[i] == separador)
            cantidad_separadores++;

        i++;
    }

    return cantidad_separadores;
}


/*
*   Pre condiciones: debe recibir un puntero a hospital_t y un vector de strings.
*   en la linea leida.
*
*   Post condiciones: actualiza los datos de los pokemones en el hospital_t.
*
*/
bool actualizar_pkmnes_en_hospital(hospital_t* hospital, char** datos_separados, size_t cantidad_pokemones_en_archivo){
    int j = 2;
    int nivel_pokemon;
    bool inserto_entrenador = false;
    for (size_t i = 0; i < cantidad_pokemones_en_archivo; ++i){
        entrenador_t* entrenador = calloc(1, sizeof(entrenador_t));
        entrenador->id = (size_t)atoi(datos_separados[0]);

        size_t largo_nombre_entrenador = (strlen(datos_separados[1]) + 1);
        char* nombre_entrenador = calloc(largo_nombre_entrenador, sizeof(char));
        if(!entrenador || !nombre_entrenador){
            free(entrenador);
            free(nombre_entrenador);
            return true;
        }

        strcpy(nombre_entrenador, datos_separados[1]);
        entrenador->nombre = nombre_entrenador;

        pokemon_t* pokemon = calloc(1, sizeof(pokemon_t));
        size_t largo_nombre_pokemon = (strlen(datos_separados[j]) + 1);
        char* nombre = calloc(largo_nombre_pokemon, sizeof(char));
        
        if (!nombre || !pokemon){
            free(pokemon);
            free(nombre);
            return true;
        }
        pokemon->nombre = nombre;

        strcpy(pokemon->nombre, datos_separados[j]);

        nivel_pokemon = atoi(datos_separados[j + 1]);
        pokemon->nivel = (size_t)nivel_pokemon;
        pokemon->entrenador = entrenador;
        void* elemento = pokemon;

        if(lista_tamanio(hospital->vector_pokemones) > 1){
            lista_iterador_t* iterador;
            void* elemento_a_comparar;
            size_t posicion = 0;
            bool actualizo = false;
            for(iterador = lista_iterador_crear(hospital->vector_pokemones);
                lista_iterador_tiene_siguiente(iterador);
                lista_iterador_avanzar(iterador)){

                elemento_a_comparar = lista_iterador_elemento_actual(iterador);

                if(strcmp(((pokemon_t*)elemento_a_comparar)->nombre, pokemon->nombre) > 0 && !actualizo){
                    actualizo = true;
                    lista_insertar_en_posicion(hospital->vector_pokemones, elemento, posicion);
                }
                posicion++;
            }
            
            if(!actualizo)
                lista_insertar(hospital->vector_pokemones, elemento);

            lista_iterador_destruir(iterador);

        }else if (lista_tamanio(hospital->vector_pokemones) == 1){
            void* elemento_a_comparar = lista_primero(hospital->vector_pokemones);
            if(strcmp(((pokemon_t*)elemento_a_comparar)->nombre, pokemon->nombre) > 0){
                lista_insertar_en_posicion(hospital->vector_pokemones, elemento, 0);
            }
            else
                lista_insertar(hospital->vector_pokemones, elemento);
        }else{
            lista_insertar(hospital->vector_pokemones, elemento);
        }

        if(!inserto_entrenador){
            lista_insertar(hospital->entrenadores, &(entrenador->id));
            inserto_entrenador = true;
        }

        j += 2;
    }

    return false;
}


/*
*   Pre condiciones: --------------------------------------------------------
*
*   Post condiciones: Crea un hospital_t y inicializa la cantidad de entrenadores en 0.
*
*/
hospital_t* hospital_crear(){
    hospital_t* hospital_nuevo = calloc(1, sizeof(hospital_t));
    lista_t* lista_pokemones = lista_crear();
    lista_t* lista_entrenadores = lista_crear();
    if (!hospital_nuevo || !lista_pokemones || !lista_entrenadores){
        lista_destruir(lista_pokemones);
        lista_destruir(lista_entrenadores);
        hospital_destruir(hospital_nuevo);
        return NULL;
    }
    hospital_nuevo->vector_pokemones = lista_pokemones;
    hospital_nuevo->entrenadores = lista_entrenadores;
    hospital_nuevo->cantidad_entrenadores = 0;

    return hospital_nuevo;
}


/*
*   Pre condiciones: recibe un puntero a hospital_t y el nombre del archivo a leer.
*
*   Post condiciones: lee todas las lineas del archivo, agrega todos estos datos al hospital
*   y devuelve true si todo funciono correctamente. De no ser asi devuelve false.
*
*/
bool hospital_leer_archivo(hospital_t* hospital, const char* nombre_archivo){    
    FILE* archivo_leido = fopen(nombre_archivo, "r");
    
    if(!archivo_leido)
        return NULL;

    bool fallo_subir_datos = false;
    size_t cantidad_palabras;

    char* datos_leidos = leer_linea_de_archivo(archivo_leido);
    while(!fallo_subir_datos && datos_leidos != NULL){
        char** datos_separados = split(datos_leidos, ';');
        if (!datos_leidos){
            return false;
        }
        cantidad_palabras = (contar_separadores(datos_leidos, ';') + 1);
        hospital->cantidad_pokemon += ((cantidad_palabras - 2) / 2);
        hospital->cantidad_entrenadores++;
        fallo_subir_datos = actualizar_pkmnes_en_hospital(hospital, datos_separados, ((cantidad_palabras - 2) / 2));
        
        free(datos_leidos);
        datos_leidos = leer_linea_de_archivo(archivo_leido);
        
        for (int n = 0; n < cantidad_palabras; ++n){
            free(datos_separados[n]);
        }
        free(datos_separados);
    }
    
    if(fallo_subir_datos){
        return false;    
    }
    
    fclose(archivo_leido);
    return true;
}


/*
*   Pre condiciones: recibe un puntero a hospital_t.
*
*   Post condiciones: devuelve la cantidad de pokemones que se encuentran en el hospital.
*   Si el hospital es NULL devuelve 0.
*
*/
size_t hospital_cantidad_pokemon(hospital_t* hospital){
    if (!hospital){
        return 0;
    }
    return hospital->cantidad_pokemon;
}


/*
*   Pre condiciones: recibe un puntero a hospital_t.
*
*   Post condiciones: devuelve la cantidad de entrenadores que se encuentran en el hospital.
*   Si el hospital es NULL devuelve 0.
*
*/
size_t hospital_cantidad_entrenadores(hospital_t* hospital){
    if (!hospital){
        return 0;
    }
    return hospital->cantidad_entrenadores;
}


/*
*   Pre condiciones: recibe un puntero a hospital_t y una funcion que debe devolver un booleano y su parametro
*   es un puntero a un pokemon_t.
*
*   Post condiciones: aplica la funcion pasada por parametro a todos los pokemones del hospital
*   en orden alfabetico y devuelve a cuantos de estos al aplicarle la funcion devuelve true 
*   (al primer false corta la iteracion y devuelve la cantidad de pokemones a los que se le aplico
*   la funcion hasta ese momento, contando el que devolvio false). Si el hospital es NULL devuelve 0.
*
*/
size_t hospital_a_cada_pokemon(hospital_t* hospital, bool (*funcion)(pokemon_t* p)){
    if (!hospital || !funcion)
        return 0;

    bool funciona = true;
    size_t pokemones_a_los_que_se_aplico_funcion = 1;

    void* elemento_a_comparar;
    if(lista_tamanio(hospital->vector_pokemones) > 1){
            lista_iterador_t* iterador;
            for(iterador = lista_iterador_crear(hospital->vector_pokemones);
                lista_iterador_tiene_siguiente(iterador);
                lista_iterador_avanzar(iterador)){

                elemento_a_comparar = lista_iterador_elemento_actual(iterador);
                funciona = funcion((pokemon_t*)elemento_a_comparar);
                if (!funciona){
                    lista_iterador_destruir(iterador);
                    return (pokemones_a_los_que_se_aplico_funcion);
                }
                pokemones_a_los_que_se_aplico_funcion++;
            }

            lista_iterador_destruir(iterador);
        }else if(lista_tamanio(hospital->vector_pokemones)  == 1){
            elemento_a_comparar = lista_primero(hospital->vector_pokemones);
            funciona = funcion((pokemon_t*)elemento_a_comparar);
            if (!funciona){
                return (pokemones_a_los_que_se_aplico_funcion);
            }
            pokemones_a_los_que_se_aplico_funcion++;
        }



    return hospital->cantidad_pokemon;
}

bool liberar_nombres(pokemon_t* p){
    if(!p)
        return false;
    free(p->nombre);
    free(p->entrenador->nombre);
    free(p->entrenador);
    free(p);
    return true;
}


/*
*   Pre condiciones: recibe un puntero a hospital_t.
*
*   Post condiciones: libera toda la memoria que se habia pedido para cada variable que haya usado malloc
*   o calloc y finalmente libera la memoria del hospital entero.
*
*/
void hospital_destruir(hospital_t* hospital){
    if (!hospital)
        return;
    hospital_cantidad_pokemon(hospital);
    hospital_a_cada_pokemon(hospital, liberar_nombres);
    lista_destruir(hospital->vector_pokemones);
    lista_destruir(hospital->entrenadores);
    free(hospital);
}


/*
*   Pre condiciones: recibe un puntero a pokemon_t.
*
*   Post condiciones: devuelve el nivel del pokemon. Si el pokemon es NULL devuelve 0. 
*
*/
size_t pokemon_nivel(pokemon_t* pokemon){
    if (!pokemon)
        return 0;

    return pokemon->nivel;
}


/*
*   Pre condiciones: recibe un puntero a pokemon_t.
*
*   Post condiciones: devuelve el nombre del pokemon. Si el pokemon es NULL devuelve NULL.
*
*/
const char* pokemon_nombre(pokemon_t* pokemon){
    if (!pokemon)
        return NULL;
    
    return pokemon->nombre;
}

bool funcion_aplicar(pokemon_t* p){
    if(strcmp(p->nombre, "miltank") != 0){
        return true;    
    }
    return false;
}


entrenador_t* pokemon_entrenador(pokemon_t* pokemon){
    if(!pokemon || !pokemon->entrenador)
        return NULL;

    return pokemon->entrenador;
}


void escribir_linea(FILE* archivo, size_t posicion_entrenador_actual, hospital_t* h){
    void* elemento_entrenador = lista_elemento_en_posicion(h->entrenadores, posicion_entrenador_actual);
    //entrenador_t* entenador_actual = (entrenador_t*)elemento_entrenador;
    size_t id_entenador_actual = (*(size_t*)elemento_entrenador);
    fprintf(archivo, "%zu;", id_entenador_actual);

    size_t posicion_entrenador = 0;
    bool encontro_entrenador = false;
    while((posicion_entrenador < lista_tamanio(h->vector_pokemones)) && !encontro_entrenador){
        void* elemento_pokemon = lista_elemento_en_posicion(h->vector_pokemones, posicion_entrenador);
        pokemon_t* pokemon_actual = (pokemon_t*)elemento_pokemon;
        if(pokemon_actual->entrenador->id == id_entenador_actual){
            fprintf(archivo, "%s", pokemon_actual->entrenador->nombre);
            encontro_entrenador = true;
        }
        posicion_entrenador++;
    }

    size_t posicion_pokemon_actual = 0;
    while(posicion_pokemon_actual < lista_tamanio(h->vector_pokemones)){
        void* elemento_pokemon = lista_elemento_en_posicion(h->vector_pokemones, posicion_pokemon_actual);
        pokemon_t* pokemon_actual = (pokemon_t*)elemento_pokemon;
        if(pokemon_actual->entrenador->id == id_entenador_actual){
            fprintf(archivo, ";%s;%zu", pokemon_actual->nombre, pokemon_actual->nivel);
        }
        posicion_pokemon_actual++;
    }
    fprintf(archivo, "\n");
}


/**
 * Guarda un archivo con el nombre dado. El formato del archivo debe ser igual
 * al del caso de lectura.
 *
 * Devuelve true si pudo guardarlo o false en caso de error.
 */
bool hospital_guardar_archivo(hospital_t* hospital, const char* nombre_archivo){
    if(!hospital || !nombre_archivo)
        return false;
    
    FILE* archivo = fopen(nombre_archivo,"w");
    if(archivo == NULL)
        return false;

    for (size_t i = 0; i < hospital->cantidad_entrenadores; ++i){
        escribir_linea(archivo, i, hospital);
    }

    fclose(archivo);
    return true;
}