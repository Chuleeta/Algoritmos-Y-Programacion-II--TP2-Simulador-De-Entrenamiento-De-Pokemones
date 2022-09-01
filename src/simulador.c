#include "simulador.h"
#include "lista.h"
#include "cola.h"
#include "heap.h"

#include <string.h>

struct _heap_t{
	void** vector;
	size_t cantidad_de_nodos;
	size_t capacidad;
	funcion_comparador_t funcion;
};

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

struct _simulador_t{
	cola_t* sala_de_espera_entrenadores;
	heap_t* pokemones_recepcionados;
	pokemon_t* pokemon_siendo_atendido;
	unsigned pokemones_atendidos;

	EstadisticasSimulacion* estadisticas;
	lista_t* dificultades;
	lista_t* funciones_dificultades;
	unsigned cantidad_intentos;
	hospital_t* hospital;

	bool simulacion_finalizada;
};


/*
*	Pre Condiciones: Recibe dos pokemones
*
*	Post Condiciones: Devuelve cual de los dos tiene el menor nivel
*
*/
bool pokemon_de_menor_nivel(void* pokemon1, void* pokemon2){
	size_t nivel_pokemon_1 = pokemon_nivel((pokemon_t*)pokemon1);
	size_t nivel_pokemon_2 = pokemon_nivel((pokemon_t*)pokemon2);

	if(nivel_pokemon_1 <= nivel_pokemon_2)
		return true;
	

	return false;
}

/*
*	Pre Condiciones: Recibe un hosital y la sala de espera
*
*	Post Condiciones: Agrega los entrenadores a la sala de espera
*
*/
void agregar_entrenadores_a_cola(hospital_t* hospital, cola_t* sala_de_espera){
	lista_iterador_t* iterador;
    void* elemento;
    if(lista_tamanio(hospital->entrenadores) > 1){
		for(iterador = lista_iterador_crear(hospital->entrenadores);
	        lista_iterador_tiene_siguiente(iterador);
	        lista_iterador_avanzar(iterador)){

	        elemento = lista_iterador_elemento_actual(iterador);
	    	cola_encolar(sala_de_espera, elemento);
	    }

	    lista_iterador_destruir(iterador);
    }else if(lista_tamanio(hospital->entrenadores) == 1){
    	elemento = lista_primero(hospital->entrenadores);
    	cola_encolar(sala_de_espera, elemento);
    }
}

/*
*	Pre Condiciones: Recibe la lista de dificultades
*
*	Post Condiciones: Agrega las 3 dificultades base a la lista
*
*/
void agregar_dificultades_a_lista(lista_t* dificultades){
	int id_facil = 0;
	int id_normal = 1;
	int id_dificil = 2;
	InformacionDificultad* dificultad_facil = calloc(1, sizeof(InformacionDificultad));
	InformacionDificultad* dificultad_normal = calloc(1, sizeof(InformacionDificultad));
	InformacionDificultad* dificultad_dificil = calloc(1, sizeof(InformacionDificultad));

	char* nombre_facil = calloc(6, sizeof(char));
	char* nombre_normal = calloc(7, sizeof(char));
	char* nombre_dificil = calloc(8, sizeof(char));

	if(!dificultad_facil || !dificultad_normal || !dificultad_dificil || !nombre_facil ||
	 !nombre_normal || !nombre_dificil){
	 	free(nombre_facil);
	 	free(nombre_normal);
	 	free(nombre_dificil);
		free(dificultad_facil);
		free(dificultad_normal);
		free(dificultad_dificil);
		return;
	}

	strcpy(nombre_facil, "Facil");
	strcpy(nombre_normal, "Normal");
	strcpy(nombre_dificil, "Dificil");

	dificultad_facil->id = id_facil;
	dificultad_facil->en_uso = false;
	dificultad_facil->nombre_dificultad = nombre_facil;

	dificultad_normal->id = id_normal;
	dificultad_normal->en_uso = true;
	dificultad_normal->nombre_dificultad = nombre_normal;

	dificultad_dificil->id = id_dificil;
	dificultad_dificil->en_uso = false;
	dificultad_dificil->nombre_dificultad = nombre_dificil;

	lista_insertar(dificultades, dificultad_facil);
	lista_insertar(dificultades, dificultad_normal);
	lista_insertar(dificultades, dificultad_dificil);
}


/*
*	Pre Condiciones: Recibe un numero
*
*	Post Condiciones: Devuelve el modulo de este
*
*/
int valor_absoluto(int resultado){
	if(resultado < 0)
		return (resultado*(-1));

	return resultado;
}


/*
*	Pre Condiciones: Recibe el nivel adivinado y el nivel del pokemon
*
*	Post Condiciones: Devuelve un resultado segun la diferencia entre el nivel adivinado y el nivel del pokemon.
* 					  0 es que el nivel adivinado es el correcto.
*
*/
int verificar_nivel_facil(unsigned nivel_adivinado, unsigned nivel_pokemon){
	int resultado = (int)(nivel_adivinado - nivel_pokemon);
	
	resultado = valor_absoluto(resultado);

	if(resultado == 0){
		return 0;
	}
	else if(resultado < 5)
		return 5;
	else if(resultado < 10 && resultado >= 5)
		return 10;
	else if(resultado < 15 && resultado >= 10)
		return 15;

	return -1;
}


/*
*	Pre Condiciones: Recibe la diferencia entre el numero adivinado y el del pokemon
*
*	Post Condiciones: Segun el numero recibido devuelve un string diferente
*
*/
const char* verificacion_a_string_facil(int resultado_verificacion){
	if(resultado_verificacion == 0){
		return "Resultado Correcto";
	}
	else if(resultado_verificacion == 5)
		return "Estas a menos de 5 niveles";
	else if(resultado_verificacion == 10)
		return "Estas a menos de 10 niveles";
	else if(resultado_verificacion == 15)
		return "Estas a menos de 15 niveles";
	
	return "Estas a mas de 15 niveles";
}


/*
*	Pre Condiciones: Recibe la cantidad de intentos
*
*	Post Condiciones: Devuelve los puntos indicados segun la cantidad de intentos
*
*/
unsigned calcular_puntaje_facil(unsigned cantidad_intentos){
	if(cantidad_intentos == 1)
		return 10;
	else if(cantidad_intentos <= 5 && cantidad_intentos > 1)
		return 7;
	else if(cantidad_intentos <= 10 && cantidad_intentos > 5)
		return 4;

	return 1;
}


/*
*	Pre Condiciones: Recibe el nivel adivinado y el nivel del pokemon
*
*	Post Condiciones: Devuelve 0 si el nivel adivinado es el correcto, si el nivel adivinado
*					  es menor que el del pokemon se devuelve 1, si es mayor se devuelve -1
*
*/
int verificar_nivel_normal(unsigned nivel_adivinado, unsigned nivel_pokemon){
	int resultado = (int)(nivel_adivinado - nivel_pokemon);

	if(resultado == 0){
		return 0;
	}
	else if(resultado < 0)
		return 1;

	return -1;
}


/*
*	Pre Condiciones: Recibe la diferencia entre el numero adivinado y el del pokemon
*
*	Post Condiciones: Segun el numero devuelve un string indicando si el resultado es correcto, mas chico o mas grande.
*
*/
const char* verificacion_a_string_normal(int resultado_verificacion){
	if(resultado_verificacion == 0){
		return "Resultado Correcto";
	}
	else if(resultado_verificacion == 1)
		return "Es un nivel mas alto";
	
	return "Es un nivel mas bajo";
}


/*
*	Pre Condiciones: Recibe la cantidad de intentos
*
*	Post Condiciones: Devuelve los puntos indicados segun la cantidad de intentos
*
*/
unsigned calcular_puntaje_normal(unsigned cantidad_intentos){
	if(cantidad_intentos == 1)
		return 20;
	else if(cantidad_intentos <= 5 && cantidad_intentos > 1)
		return 10;
	else if(cantidad_intentos <= 10 && cantidad_intentos > 5)
		return 5;

	return 1;
}


/*
*	Pre Condiciones: Recibe el nivel adivinado y el nivel del pokemon
*
*	Post Condiciones: Devuelve la diferencia entre el nivel adivinado y el del pokemon
*
*/
int verificar_nivel_dificil(unsigned nivel_adivinado, unsigned nivel_pokemon){
	return ((int)(nivel_adivinado - nivel_pokemon));
}


/*
*	Pre Condiciones: Recibe la diferencia entre el numero adivinado y el del pokemon
*
*	Post Condiciones: Segun el numero devuelve un string indicando si el resultado es correcto o no
*
*/
const char* verificacion_a_string_dificil(int resultado_verificacion){
	if(resultado_verificacion == 0){
		return "Resultado Correcto";
	}
	
	return "Resultado Incorrecto";
}


/*
*	Pre Condiciones: Recibe la cantidad de intentos
*
*	Post Condiciones: Devuelve los puntos indicados segun la cantidad de intentos
*
*/
unsigned calcular_puntaje_dificil(unsigned cantidad_intentos){
	if(cantidad_intentos == 1)
		return 50;
	else if(cantidad_intentos <= 5 && cantidad_intentos > 1)
		return 30;
	else if(cantidad_intentos <= 10 && cantidad_intentos > 5)
		return 20;

	return 10;
}


/*
*	Pre Condiciones: Recibe una lista con las funciones de cada dificultad vacia
*
*	Post Condiciones: Agrega las funciones de las 3 dificultades basicas a la lista
*
*/
void agregar_funciones_dificultad_a_lista(lista_t* funciones_dificultades){
	DatosDificultad* dificultad_facil = calloc(1, sizeof(DatosDificultad));
	DatosDificultad* dificultad_normal = calloc(1, sizeof(DatosDificultad));
	DatosDificultad* dificultad_dificil = calloc(1, sizeof(DatosDificultad));

	char* nombre_facil = calloc(6, sizeof(char));
	char* nombre_normal = calloc(7, sizeof(char));
	char* nombre_dificil = calloc(8, sizeof(char));

	if(!dificultad_facil || !dificultad_normal || !dificultad_dificil || !nombre_facil || 
		!nombre_normal || !nombre_dificil){
		free(dificultad_facil);
		free(dificultad_normal);
		free(dificultad_dificil);
		free(nombre_facil);
	 	free(nombre_normal);
	 	free(nombre_dificil);
		return;
	}

	strcpy(nombre_facil, "Facil");
	strcpy(nombre_normal, "Normal");
	strcpy(nombre_dificil, "Dificil");

	dificultad_facil->nombre = nombre_facil;
	dificultad_facil->calcular_puntaje = calcular_puntaje_facil;
	dificultad_facil->verificar_nivel = verificar_nivel_facil;
	dificultad_facil->verificacion_a_string = verificacion_a_string_facil;

	dificultad_normal->nombre = nombre_normal;
	dificultad_normal->calcular_puntaje = calcular_puntaje_normal;
	dificultad_normal->verificar_nivel = verificar_nivel_normal;
	dificultad_normal->verificacion_a_string = verificacion_a_string_normal;

	dificultad_dificil->nombre = nombre_dificil;
	dificultad_dificil->calcular_puntaje = calcular_puntaje_dificil;
	dificultad_dificil->verificar_nivel = verificar_nivel_dificil;
	dificultad_dificil->verificacion_a_string = verificacion_a_string_dificil;

	lista_insertar(funciones_dificultades, dificultad_facil);
	lista_insertar(funciones_dificultades, dificultad_normal);
	lista_insertar(funciones_dificultades, dificultad_dificil);
}


/**
 * Crea un simulador para un hospital. El simulador toma control del hospital y
 * el mismo no debe ser modificado ni liberado por fuera del simulador.
 *
 * Devuelve el simulador creado o NULL en caso de error.
 */
simulador_t* simulador_crear(hospital_t* hospital){
	if(!hospital)
		return NULL;

	cola_t* sala_de_espera = cola_crear();
	heap_t* pokemones_recepcionados = crear_heap(pokemon_de_menor_nivel);//heap minimal
	lista_t* dificultades = lista_crear();
	lista_t* funciones_dificultades = lista_crear();

	EstadisticasSimulacion* estadisticas = calloc(1, sizeof(EstadisticasSimulacion));

	simulador_t* simulador = calloc(1, sizeof(simulador_t));

	if(!simulador || !estadisticas || !sala_de_espera || !pokemones_recepcionados || !dificultades ||
	 !funciones_dificultades){
		cola_destruir(sala_de_espera);
		destruir_heap(pokemones_recepcionados);
		lista_destruir(dificultades);
		lista_destruir(funciones_dificultades);
		free(estadisticas);
		free(simulador);
		return NULL;
	}

	agregar_entrenadores_a_cola(hospital, sala_de_espera);
	agregar_dificultades_a_lista(dificultades);
	agregar_funciones_dificultad_a_lista(funciones_dificultades);

	simulador->sala_de_espera_entrenadores = sala_de_espera;
	simulador->pokemones_recepcionados = pokemones_recepcionados;
	simulador->estadisticas = estadisticas;
	simulador->dificultades = dificultades;
	simulador->funciones_dificultades = funciones_dificultades;
	simulador->hospital = hospital;

	simulador->estadisticas->entrenadores_totales = (unsigned)cola_tamanio(simulador->sala_de_espera_entrenadores);
	simulador->estadisticas->pokemon_totales = (unsigned)lista_tamanio(simulador->hospital->vector_pokemones);

	simulador->pokemon_siendo_atendido = NULL;
	simulador->simulacion_finalizada = false;

	return simulador;
}


/*
*	Pre Condiciones: Recibe el simulador y las estadisticas que se deben rellenar con la informacion del simulador
*
*	Post Condiciones: Rellena la info necesaria y devuelve true si todo salio bien. Si no hay estadisticas las
*					  cuales deba rellenar devuelve false.
*
*/
/*
devuelve true si todo bien, sino devuelve false
*/
bool obtener_estadisticas(simulador_t* simulador, void* datos){
	simulador->estadisticas->cantidad_eventos_simulados++;
	if(!datos)
		return false;
	EstadisticasSimulacion* estadisticas = (EstadisticasSimulacion*)datos;

	estadisticas->entrenadores_atendidos = simulador->estadisticas->entrenadores_atendidos;
	estadisticas->entrenadores_totales = simulador->estadisticas->entrenadores_totales;
	estadisticas->pokemon_atendidos = simulador->estadisticas->pokemon_atendidos;
	estadisticas->pokemon_en_espera = simulador->estadisticas->pokemon_en_espera;
	estadisticas->pokemon_totales = simulador->estadisticas->pokemon_totales;
	estadisticas->puntos = simulador->estadisticas->puntos;
	estadisticas->cantidad_eventos_simulados = simulador->estadisticas->cantidad_eventos_simulados;

	return true;
}


/*
*	Pre Condiciones: Recibe un pokemon
*
*	Post Condiciones: Devuelve una copia de este
*
*/
pokemon_t* copiar_pokemon(pokemon_t* pokemon){
	pokemon_t* copia_pokemon = calloc(1,sizeof(pokemon_t));
	entrenador_t* copia_entrenador = calloc(1,sizeof(entrenador_t));
	
	size_t largo_nombre_pokemon_copia = strlen(pokemon->nombre)+1;
	char* copia_nombre_pokemon = calloc(largo_nombre_pokemon_copia, sizeof(char));

	size_t largo_nombre_entrenador_copia = strlen(pokemon->entrenador->nombre)+1;
	char* copia_nombre_entrenador = calloc(largo_nombre_entrenador_copia, sizeof(char));
	
	if(!copia_pokemon || !copia_entrenador || !copia_nombre_pokemon || !copia_nombre_entrenador){
		free(copia_pokemon);
		free(copia_entrenador);
		free(copia_nombre_pokemon);
		free(copia_nombre_entrenador);
		return NULL;
	}
	
	strcpy(copia_nombre_pokemon, pokemon->nombre);
	strcpy(copia_nombre_entrenador, pokemon->entrenador->nombre);

	copia_entrenador->nombre = copia_nombre_entrenador;
	copia_entrenador->id = pokemon->entrenador->id;

	copia_pokemon->nivel = pokemon->nivel;
	copia_pokemon->nombre = copia_nombre_pokemon;
	copia_pokemon->entrenador = copia_entrenador;

	return copia_pokemon;
}


/*
*	Pre Condiciones: Recibe el simulador y un id de entrenador buscado
*
*	Post Condiciones: Recorre el vector de pokemones del hospital, busca aquellos pokemones con el id
*					  buscado, hace una copia de este y lo agrega al heap. Si encontro algun pokemon
*					  con el id y lo agrego al heap pokemones_recepcionados, devuelve true. De lo contrario
*					  devuelve false.
*
*/
bool agregar_pokemones_de_entrenador_a_heap(simulador_t* simulador, size_t id_buscado){
	lista_iterador_t* iterador;
    void* elemento;
    bool encontro_pokemones = false;

	for(iterador = lista_iterador_crear(simulador->hospital->vector_pokemones);
        lista_iterador_tiene_siguiente(iterador);
        lista_iterador_avanzar(iterador)){

        elemento = lista_iterador_elemento_actual(iterador);
    	pokemon_t* pokemon = (pokemon_t*)elemento;

    	if(pokemon->entrenador->id == id_buscado){
	    	pokemon_t* pokemon_a_insertar = copiar_pokemon(pokemon);
	    	if(!pokemon_a_insertar){
	    		lista_iterador_destruir(iterador);
	    		return false;
	    	}
		    encontro_pokemones = true;
		    if(!agregar_nodo(simulador->pokemones_recepcionados, (pokemon_a_insertar))){
				lista_iterador_destruir(iterador);
				return false;
		    }
		   	simulador->estadisticas->pokemon_en_espera++;
    	}
    }

    lista_iterador_destruir(iterador);
    if(!encontro_pokemones)
    	return false;
    
    return true;
}


/*
*	Pre Condiciones: Recibe el simulador
*
*	Post Condiciones: Saca al proximo entrenador de la cola y agrega sus pokemones al heap.
*					  Si no hay mas entrenadores o si falla el agregar los pokemones al heap, devuelve false.
*					  De lo contrario devuelve true.
*
*/
bool atender_proximo_entrenador(simulador_t* simulador){
	simulador->estadisticas->cantidad_eventos_simulados++;
	void* elemento_entrenador = cola_desencolar(simulador->sala_de_espera_entrenadores);
	if(elemento_entrenador == NULL)
		return false;
	
	size_t id_entrenador = (*(size_t*)elemento_entrenador);
	if(!agregar_pokemones_de_entrenador_a_heap(simulador, id_entrenador))
		return false;
	simulador->estadisticas->entrenadores_atendidos++;

	if(simulador->pokemon_siendo_atendido == NULL){
		simulador->pokemon_siendo_atendido = (pokemon_t*)ver_nodo_mas_chico(simulador->pokemones_recepcionados);
		eliminar_raiz(simulador->pokemones_recepcionados);
		if(simulador->estadisticas->pokemon_en_espera > 0)
			simulador->estadisticas->pokemon_en_espera--;
	}

	return true;
}


/*
*	Pre Condiciones: Recibe el simulador y la informacion del pokemon que se debe rellenar.
*
*	Post Condiciones: Rellena los datos del pokemon en tratamiento. Si no hay pokemon siendo atendido
*					  o la informacion a rellenar es invalida, se devuelve false. Si todo sale bien
*					  se devuelve true.
*
*/
bool obtener_info_pokemon_en_tratamiento(simulador_t* simulador, void* datos){
	simulador->estadisticas->cantidad_eventos_simulados++;
	if(!datos)
		return false;

	InformacionPokemon* info_pokemon = (InformacionPokemon*)datos;
	info_pokemon->nombre_pokemon = NULL;
	info_pokemon->nombre_entrenador = NULL;
	if(!simulador->pokemon_siendo_atendido)
		return false;
	
	info_pokemon->nombre_pokemon = simulador->pokemon_siendo_atendido->nombre;
	info_pokemon->nombre_entrenador = simulador->pokemon_siendo_atendido->entrenador->nombre;

	return true;
}


/*
*	Pre Condiciones: Recibe la lista de dificultades.
*
*	Post Condiciones: La recorre y devuelve la dificultad que esta en uso. Si
*					  ninguna esta en uso, devuelve NULL.
*
*/
InformacionDificultad* encontrar_dificultad_en_uso(lista_t* lista_de_dificultades){
	lista_iterador_t* iterador;
    void* elemento;
	for(iterador = lista_iterador_crear(lista_de_dificultades);
        lista_iterador_tiene_siguiente(iterador);
        lista_iterador_avanzar(iterador)){

        elemento = lista_iterador_elemento_actual(iterador);
    	InformacionDificultad* dificultad_elemento = (InformacionDificultad*)elemento;
    	if(dificultad_elemento->en_uso){
    		lista_iterador_destruir(iterador);
    		return dificultad_elemento;
    	}
    }

    lista_iterador_destruir(iterador);

    return NULL;
}


/*
*	Pre Condiciones: Recibe el simulador, el intento de adivinar el nivel del pokemon y la posicion de la dificultad usada.
*
*	Post Condiciones: Aplica las funciones de la dificultad usada y rellena los campos necesarios en el intento_pokemon.
*
*/
void adivinar_nivel_dificultad(simulador_t* simulador, Intento* intento_pokemon, size_t posicion_dificultad){
	simulador->cantidad_intentos++;
	DatosDificultad* dificultad = (DatosDificultad*)lista_elemento_en_posicion(simulador->funciones_dificultades, posicion_dificultad);
	int resultado = dificultad->verificar_nivel(intento_pokemon->nivel_adivinado, (unsigned)simulador->pokemon_siendo_atendido->nivel);
	if(resultado == 0){
		simulador->estadisticas->puntos += dificultad->calcular_puntaje(simulador->cantidad_intentos);
		intento_pokemon->es_correcto = true;
		
		simulador->cantidad_intentos = 0;
		simulador->estadisticas->pokemon_atendidos++;

		free(simulador->pokemon_siendo_atendido->entrenador->nombre);
		free(simulador->pokemon_siendo_atendido->entrenador);
		free(simulador->pokemon_siendo_atendido->nombre);
		free(simulador->pokemon_siendo_atendido);

		simulador->pokemon_siendo_atendido = ver_nodo_mas_chico(simulador->pokemones_recepcionados);
		eliminar_raiz(simulador->pokemones_recepcionados);
		
		if(simulador->estadisticas->pokemon_en_espera > 0)
			simulador->estadisticas->pokemon_en_espera--;
	}
	else
		intento_pokemon->es_correcto = false;
	intento_pokemon->resultado_string = dificultad->verificacion_a_string(resultado);
}


/*
*	Pre Condiciones: Recibe el simulador y el intento de adivinar el nivel del pokemon.
*
*	Post Condiciones: Aplica la dificultad en uso al intento_pokemon y devuelve true si todo sale bien.
*					  Si el intento_pokemon es invalido o si no hay pokemon siendo atendido se devuelve false.
*
*/
bool adivinar_nivel_pokemon(simulador_t* simulador, void* datos){
	simulador->estadisticas->cantidad_eventos_simulados++;
	if(!datos || !simulador->pokemon_siendo_atendido)
		return false;
	

	InformacionDificultad* dificulatd_en_uso = encontrar_dificultad_en_uso(simulador->dificultades);
	Intento* intento_pokemon = (Intento*)datos;

	if(dificulatd_en_uso->id == 0){
		adivinar_nivel_dificultad(simulador, intento_pokemon, 0);
	}
	else if(dificulatd_en_uso->id == 1){
		adivinar_nivel_dificultad(simulador, intento_pokemon, 1);
	}
	else if(dificulatd_en_uso->id == 2){
		adivinar_nivel_dificultad(simulador, intento_pokemon, 2);
	}
	else if(dificulatd_en_uso->id == 3 && lista_tamanio(simulador->funciones_dificultades) >= 4){
		adivinar_nivel_dificultad(simulador, intento_pokemon, 3);
	}
	else if(dificulatd_en_uso->id == 4 && lista_tamanio(simulador->funciones_dificultades) >= 5){
		adivinar_nivel_dificultad(simulador, intento_pokemon, 4);
	}

	return true;
}


/*
*	Pre Condiciones: Recibe el simulador y la variable de informacion de la dificultad en uso para rellenar.
*
*	Post Condiciones: Rellena los datos necesarios de la dificultad buscada. Si info_dificultad es invalido o
*					  si no se encuentra la dificultad con el id buscado se devuelve false.
*
*/
bool obtener_info_dificultad(simulador_t* simulador, void* datos){
	simulador->estadisticas->cantidad_eventos_simulados++;
	if(!datos)
		return false;
	InformacionDificultad* info_dificultad = (InformacionDificultad*)datos;
	
	lista_iterador_t* iterador;
    void* elemento;
	for(iterador = lista_iterador_crear(simulador->dificultades);
        lista_iterador_tiene_siguiente(iterador);
        lista_iterador_avanzar(iterador)){

        elemento = lista_iterador_elemento_actual(iterador);
    	InformacionDificultad* dificultad_elemento = (InformacionDificultad*)elemento;
    	if(dificultad_elemento->id == info_dificultad->id){
    		info_dificultad->en_uso = dificultad_elemento->en_uso;
    		info_dificultad->nombre_dificultad = dificultad_elemento->nombre_dificultad;
    		lista_iterador_destruir(iterador);
    		return true;
    	}
    }

    lista_iterador_destruir(iterador);
    info_dificultad->id =-1;
    info_dificultad->nombre_dificultad = NULL;
    return false;
}


/*
*	Pre Condiciones: Recibe el simulador y un numero dado.
*
*	Post Condiciones: Pone la dificultad pedida en uso. Si todo sale bien devuelve true. Si el id de la dificultad pedida
*					  no existe o si se pasa un id invalido se devuelve false.
*
*/
bool seleccionar_dificultad(simulador_t* simulador, void* datos){
	simulador->estadisticas->cantidad_eventos_simulados++;
	if(!datos)
		return false;

	int numero_dado = (*(int*)datos);

	lista_iterador_t* iterador;
    void* elemento;
    bool encontro_id = false;
	for(iterador = lista_iterador_crear(simulador->dificultades);
        lista_iterador_tiene_siguiente(iterador);
        lista_iterador_avanzar(iterador)){

        elemento = lista_iterador_elemento_actual(iterador);
    	InformacionDificultad* dificultad_elemento = (InformacionDificultad*)elemento;
    	if(dificultad_elemento->id == (numero_dado)){
			encontro_id = true;
    		dificultad_elemento->en_uso = true;
    	}
    	if((dificultad_elemento->id != (numero_dado)) && (dificultad_elemento->en_uso))
    		dificultad_elemento->en_uso = false;
    }

    lista_iterador_destruir(iterador);
    if(encontro_id)
    	return true;
    return false;
}


/*
*	Pre Condiciones: Recibe el simulador y la nueva dificultad a agregar.
*
*	Post Condiciones: Agrega la nueva dificultad al resto de dificultades. Si ocurre algun error o la nueva dificultad en invalida
*					  se devuelve false. De lo contrario se devuelve true.
*
*/
bool agregar_dificultad(simulador_t* simulador, void* datos){
	simulador->estadisticas->cantidad_eventos_simulados++;
	if(!datos)
		return false;
	
	DatosDificultad* nueva_dificultad_dada = (DatosDificultad*)datos;

	InformacionDificultad* nueva_dificultad = calloc(1,sizeof(InformacionDificultad));
	DatosDificultad* nueva_funcion_dificultad= calloc(1, sizeof(DatosDificultad));
	
	size_t largo_nombre_dificultad = strlen(nueva_dificultad_dada->nombre)+1;

	char* nombre_dificultad = malloc(largo_nombre_dificultad*sizeof(char));
	char* nombre_dificultad_funcion = malloc(largo_nombre_dificultad*sizeof(char));
	
	if(!nueva_dificultad || !nueva_dificultad_dada->nombre ||
	 !nueva_dificultad_dada->calcular_puntaje || !nueva_dificultad_dada->verificar_nivel ||
	 !nueva_dificultad_dada->verificacion_a_string || !nueva_funcion_dificultad ||
	  !nombre_dificultad || !nombre_dificultad_funcion){
		free(nueva_dificultad);
		free(nueva_funcion_dificultad);
		free(nombre_dificultad);
		free(nombre_dificultad_funcion);
		return false;
	}

	strcpy(nombre_dificultad, nueva_dificultad_dada->nombre);
	strcpy(nombre_dificultad_funcion, nueva_dificultad_dada->nombre);

	nueva_dificultad->nombre_dificultad = nombre_dificultad;
	nueva_dificultad->en_uso = false;
	nueva_dificultad->id = (int)(lista_tamanio(simulador->dificultades));
	lista_insertar(simulador->dificultades, nueva_dificultad);

	nueva_funcion_dificultad->nombre = nombre_dificultad_funcion;
	nueva_funcion_dificultad->calcular_puntaje = nueva_dificultad_dada->calcular_puntaje;
	nueva_funcion_dificultad->verificar_nivel = nueva_dificultad_dada->verificar_nivel;
	nueva_funcion_dificultad->verificacion_a_string = nueva_dificultad_dada->verificacion_a_string;

	lista_insertar(simulador->funciones_dificultades, nueva_funcion_dificultad);
	return true;
}


/*
*	Pre Condiciones: Recibe un simulador.
*
*	Post Condiciones: Finaliza la simulacion. De pasarse un simulador invalido se devuelve false. De lo contrario se devuelve true.
*
*/
bool finalizar_simulacion(simulador_t* simulador){
	if(!simulador)
		return false;
	
	simulador->simulacion_finalizada = true;
	return true;
}


/*
*	Pre Condiciones: Recibe un evento.
*
*	Post Condiciones: Si el evento no es alguno de los eventos soportados se devuelve true. De lo contrario se devuelve false.
*
*/
bool evento_no_es_valido(EventoSimulacion evento){
	if(evento == ObtenerEstadisticas || evento == AtenderProximoEntrenador || evento == ObtenerInformacionPokemonEnTratamiento ||
	 evento == AdivinarNivelPokemon || evento == ObtenerInformacionDificultad || evento == SeleccionarDificultad ||
	 evento == AgregarDificultad || evento == FinalizarSimulacion)
		return false;
	return true;
}


/**
 * Simula un evento. Cada evento puede recibir un puntero a un dato que depende
 * de cada evento. En caso de no necesitarse, se debe pasar NULL.
 *
 * Ejecuta el evento correspondiente (ver la documentación) y en caso de ser
 * necesario modifica la información apuntada por dato.
 *
 * Devuelve ErrorSimulacion si no existe el simulador o existe algún error en el
 * manejo de memoria.
 *
 * Devuelve ExitoSimulacion o ErrorSimulacion según corresponda a cada evento.
 */
ResultadoSimulacion simulador_simular_evento(simulador_t* simulador, EventoSimulacion evento, void* datos){
	if(!simulador || simulador->simulacion_finalizada)
		return ErrorSimulacion;

	bool paso_un_error = false;
	if(evento == ObtenerEstadisticas && !obtener_estadisticas(simulador, datos)){
		paso_un_error = true;
	}
	else if(evento == AtenderProximoEntrenador && !atender_proximo_entrenador(simulador)){
		paso_un_error = true;
	}
	else if(evento == ObtenerInformacionPokemonEnTratamiento && !obtener_info_pokemon_en_tratamiento(simulador, datos)){
		paso_un_error = true;
	}
	else if(evento == AdivinarNivelPokemon && !adivinar_nivel_pokemon(simulador, datos)){
		paso_un_error = true;
	}
	else if(evento == ObtenerInformacionDificultad && !obtener_info_dificultad(simulador, datos)){
		paso_un_error = true;
	}
	else if(evento == SeleccionarDificultad && !seleccionar_dificultad(simulador, datos)){
		paso_un_error = true;
	}
	else if(evento == AgregarDificultad && !agregar_dificultad(simulador, datos)){
		paso_un_error = true;
	}
	else if(evento == FinalizarSimulacion && !finalizar_simulacion(simulador)){
		paso_un_error = true;
	}

	if(paso_un_error || evento_no_es_valido(evento))
		return ErrorSimulacion;
	return ExitoSimulacion;
}


/*
*	Pre Condiciones: Recibe un heap.
*
*	Post Condiciones: Libera los pokemones que queden en este.
*
*/
void liberar_pokemones_heap(heap_t* heap){
	if(!heap)
		return;
	void* elemento_pokemon = ver_nodo_mas_chico(heap);
	while(elemento_pokemon != NULL){

		pokemon_t* pokemon = (pokemon_t*)elemento_pokemon;

		free(pokemon->entrenador->nombre);
		free(pokemon->entrenador);
		free(pokemon->nombre);
		free(pokemon);
		
		eliminar_raiz(heap);
		elemento_pokemon = ver_nodo_mas_chico(heap);
	}
}


/*
*	Pre Condiciones: Recibe la lista de dificultades disponibles.
*
*	Post Condiciones: Libera las dificultades de esta lista.
*
*/
void liberar_dificultades(lista_t* dificultades){
	lista_iterador_t* iterador;
    void* elemento;
	for(iterador = lista_iterador_crear(dificultades);
        lista_iterador_tiene_siguiente(iterador);
        lista_iterador_avanzar(iterador)){

        elemento = lista_iterador_elemento_actual(iterador);
    	InformacionDificultad* dificultad_elemento = (InformacionDificultad*)elemento;
    	
    	free((char*)dificultad_elemento->nombre_dificultad);
    	free(dificultad_elemento);
    }

    lista_iterador_destruir(iterador);
}


/*
*	Pre Condiciones: Recibe la lista de funciones de cada dificultad.
*
*	Post Condiciones: Las libera.
*
*/
void liberar_funciones_dificultades(lista_t* funciones_dificultades){
	lista_iterador_t* iterador;
    void* elemento;
	for(iterador = lista_iterador_crear(funciones_dificultades);
        lista_iterador_tiene_siguiente(iterador);
        lista_iterador_avanzar(iterador)){

        elemento = lista_iterador_elemento_actual(iterador);
    	DatosDificultad* funciones_dificultad_elemento = (DatosDificultad*)elemento;
    	
    	free((char*)funciones_dificultad_elemento->nombre);
    	free(funciones_dificultad_elemento);
    }

    lista_iterador_destruir(iterador);
}


/**
 * Destruye el simulador y libera la memoria asociada (incluida la del hospital).
 */
void simulador_destruir(simulador_t* simulador){
	if(!simulador)
		return;
	hospital_destruir(simulador->hospital);
	
	cola_destruir(simulador->sala_de_espera_entrenadores);
	

	liberar_pokemones_heap(simulador->pokemones_recepcionados);
	destruir_heap(simulador->pokemones_recepcionados);

	liberar_dificultades(simulador->dificultades);
	lista_destruir(simulador->dificultades);

	liberar_funciones_dificultades(simulador->funciones_dificultades);
	lista_destruir(simulador->funciones_dificultades);

	if(simulador->pokemon_siendo_atendido != NULL){
		free(simulador->pokemon_siendo_atendido->entrenador->nombre);
		free(simulador->pokemon_siendo_atendido->entrenador);
		free(simulador->pokemon_siendo_atendido->nombre);
		free(simulador->pokemon_siendo_atendido);
	}
	
	free(simulador->estadisticas);
	free(simulador);
}