#include "pa2mm.h"
#include "src/hospital.h"
#include "src/heap.h"
#include "src/simulador.h"

#include <string.h>
#include <stdbool.h>

bool ignorar_pokemon(pokemon_t* p){
    p = p;
    return true;
}

/* No intenten esto en sus casas */
/* Ya vamos a ver como evitar esto en el TDA Lista */
struct{
    pokemon_t* pokemon[500];
    size_t cantidad;
} acumulados;

void resetear_acumulados(){
    acumulados.cantidad = 0;
}

bool acumular_pokemon(pokemon_t* p){
    acumulados.pokemon[acumulados.cantidad] = p;
    acumulados.cantidad++;
    return true;
}

bool acumular_pokemon_hasta_miltank(pokemon_t* p){
    acumulados.pokemon[acumulados.cantidad] = p;
    acumulados.cantidad++;
    return strcmp(pokemon_nombre(p), "miltank");
}

bool acumulados_en_orden_correcto(){
    if(acumulados.cantidad < 2)
        return true;
    pokemon_t* anterior = acumulados.pokemon[0];
    for(int i=1;i<acumulados.cantidad;i++){
        pokemon_t* actual =  acumulados.pokemon[i];
        if(strcmp(pokemon_nombre(anterior), pokemon_nombre(actual)) > 0)
            return false;
    }
    return true;
}

/* Pruebas del hospital*/

void puedoCrearYDestruirUnHospital(){
    hospital_t* h=NULL;

    pa2m_afirmar((h=hospital_crear()), "Crear un hospital devuelve un hospital");

    pa2m_afirmar(hospital_cantidad_entrenadores(h)==0, "Un hospital se crea con cero entrenadores");
    pa2m_afirmar(hospital_cantidad_pokemon(h)==0, "Un hospital se crea con cero pokemon");

    pa2m_afirmar(hospital_a_cada_pokemon(h, ignorar_pokemon)==0, "Recorrer los pokemon resulta en 0 pokemon recorridos");

    hospital_destruir(h);
}

void dadoUnHospitalNULL_lasPuedoAplicarLasOperacionesDelHospitalSinProblema(){
    hospital_t* h=NULL;

    pa2m_afirmar(hospital_cantidad_entrenadores(h)==0, "Un hospital NULL tiene cero entrenadores");
    pa2m_afirmar(hospital_cantidad_pokemon(h)==0, "Un hospital NULL tiene cero pokemon");

    pa2m_afirmar(hospital_a_cada_pokemon(h, ignorar_pokemon)==0, "Recorrer los pokemon de un hospital NULL resulta en 0 pokemon recorridos");

    hospital_destruir(h);
}

void dadoUnArchivoVacio_NoSeAgreganPokemonAlHospital(){
    hospital_t* h=hospital_crear();

    pa2m_afirmar(hospital_leer_archivo(h, "ejemplos/archivo_vacio.hospital"), "Puedo leer un archivo vacío");

    pa2m_afirmar(hospital_cantidad_entrenadores(h)==0, "Un hospital vacío tiene cero entrenadores");
    pa2m_afirmar(hospital_cantidad_pokemon(h)==0, "Un hospital vacío tiene tiene cero pokemon");

    pa2m_afirmar(hospital_a_cada_pokemon(h, ignorar_pokemon)==0, "Recorrer los pokemon resulta en 0 pokemon recorridos");

    hospital_destruir(h);
}

void dadoUnArchivoConUnEntrenador_SeAgregaElEntrenadorYSusPokemonAlHospital(){
    hospital_t* h=hospital_crear();

    pa2m_afirmar(hospital_leer_archivo(h, "ejemplos/un_entrenador.hospital"), "Puedo leer un archivo con un entrenador");

    pa2m_afirmar(hospital_cantidad_entrenadores(h)==1, "El hospital tiene 1 entrenador");
    pa2m_afirmar(hospital_cantidad_pokemon(h)==3, "El hospital tiene 3 pokemon");

    resetear_acumulados();
    pa2m_afirmar(hospital_a_cada_pokemon(h, acumular_pokemon)==3, "Recorrer los pokemon resulta en 3 pokemon recorridos");
    pa2m_afirmar(acumulados_en_orden_correcto(), "Los pokemon se recorrieron en orden alfabetico");

    hospital_destruir(h);
}

void dadoUnArchivoConVariosEntrenadores_SeAgreganLosEntrenadoresYSusPokemonAlHospital(){
    hospital_t* h=hospital_crear();

    pa2m_afirmar(hospital_leer_archivo(h, "ejemplos/varios_entrenadores.hospital"), "Puedo leer un archivo con varios entrenadores");

    pa2m_afirmar(hospital_cantidad_entrenadores(h)==5, "El hospital tiene 5 entrenadores");
    pa2m_afirmar(hospital_cantidad_pokemon(h)==24, "El hospital tiene 24 pokemon");

    resetear_acumulados();
    pa2m_afirmar(hospital_a_cada_pokemon(h, acumular_pokemon)==24, "Recorrer los pokemon resulta en 24 pokemon recorridos");
    pa2m_afirmar(acumulados_en_orden_correcto(), "Los pokemon se recorrieron en orden alfabetico");

    hospital_destruir(h);
}


void dadosVariosArchivos_puedoAgregarlosTodosAlMismoHospital(){
    hospital_t* h=hospital_crear();

    pa2m_afirmar(hospital_leer_archivo(h, "ejemplos/varios_entrenadores.hospital"), "Puedo leer un archivo con varios entrenadores");
    pa2m_afirmar(hospital_leer_archivo(h, "ejemplos/varios_entrenadores.hospital"), "Puedo leer otro archivo con varios entrenadores");
    pa2m_afirmar(hospital_leer_archivo(h, "ejemplos/varios_entrenadores.hospital"), "Puedo leer un tercer archivo con varios entrenadores");

    pa2m_afirmar(hospital_cantidad_entrenadores(h)==15, "El hospital tiene 15 entrenadores");
    pa2m_afirmar(hospital_cantidad_pokemon(h)==72, "El hospital tiene 72 pokemon");

    resetear_acumulados();
    pa2m_afirmar(hospital_a_cada_pokemon(h, acumular_pokemon)==72, "Recorrer los pokemon resulta en 72 pokemon recorridos");
    pa2m_afirmar(acumulados_en_orden_correcto(), "Los pokemon se recorrieron en orden alfabetico");

    hospital_destruir(h);
}


void puedoGuardarUnArchivoDeUnHospital(){
    hospital_t* h = hospital_crear();

    hospital_leer_archivo(h, "ejemplos/varios_entrenadores.hospital");
    pa2m_afirmar(hospital_guardar_archivo(h, "archivo_guardado.hospital"), "Puedo guardar un archivo con un hospital");


    hospital_destruir(h);
}


// Pruebas del heap
bool funcion_comparador(void* elemento1, void* elemento2){
    if(!elemento1 || !elemento2)
        return false;
    if((*(size_t*)elemento1) < (*(size_t*)elemento2))
        return true;
    return false;
}

void prueboCrearUnHeap_y_Destruirlo(){
    heap_t* h = NULL;
    pa2m_afirmar((h = crear_heap(funcion_comparador)), "Creo un heap correctamente");
    pa2m_afirmar((h->cantidad_de_nodos == 0), "La cantidad de nodos es 0");
    destruir_heap(h);
}




void prueboAgregarUnElementoAlHeap(){
    heap_t* h = crear_heap(funcion_comparador);

    size_t numero1 = 888;
    size_t numero2 = 345;
    size_t numero3 = 3543;
    size_t numero4 = 697;
    size_t numero5 = 44;
    size_t numero6 = 14;
    size_t numero7 = 2;
    size_t numero8 = 877;
    size_t numero9 = 25;
    size_t numero10 = 336;
    size_t numero11 = 47;



    pa2m_afirmar(agregar_nodo(h, &numero1), "Agrego un nodo a un heap vacio");
    void* numero_minimo = ver_nodo_mas_chico(h);
    pa2m_afirmar(numero_minimo == &numero1, "El numero minimo del heap es el correcto");

    pa2m_afirmar(agregar_nodo(h, &numero2),"Agrego dos nodos a un heap");
    pa2m_afirmar(agregar_nodo(h, &numero3),"Agrego tres nodos a un heap");
    pa2m_afirmar(agregar_nodo(h, &numero4),"Agrego cuatro nodos a un heap");

    numero_minimo = ver_nodo_mas_chico(h);
    pa2m_afirmar(numero_minimo == &numero2, "El numero minimo del heap es el correcto");
    agregar_nodo(h, &numero5);
    agregar_nodo(h, &numero6);
    agregar_nodo(h, &numero7);
    agregar_nodo(h, &numero8);
    agregar_nodo(h, &numero9);
    agregar_nodo(h, &numero10);
    agregar_nodo(h, &numero11);

    numero_minimo = ver_nodo_mas_chico(h);
    pa2m_afirmar(numero_minimo == &numero7, "Luego de insertar 11 elementos el minimo es el correcto");

    destruir_heap(h);
}


void prueboEliminarUnElementoAlHeap(){
    heap_t* h = crear_heap(funcion_comparador);
    heap_t* h2 = crear_heap(funcion_comparador);

    size_t numero1 = 888;
    size_t numero2 = 345;
    size_t numero3 = 3543;
    size_t numero4 = 697;
    size_t numero5 = 44;
    size_t numero6 = 14;
    size_t numero7 = 2;
    size_t numero8 = 877;
    size_t numero9 = 25;
    size_t numero10 = 336;
    size_t numero11 = 47;

    pa2m_afirmar(eliminar_raiz(h) == NULL, "Al eliminar la raiz de un heap vacio devuelve NULL");
    pa2m_afirmar(eliminar_raiz(NULL) == NULL, "Al eliminar la raiz de un heap inexistente devuelve NULL");

    agregar_nodo(h2, &numero1);
    agregar_nodo(h2, &numero2);
    void* elemento_eliminado = eliminar_raiz(h2);
    pa2m_afirmar(elemento_eliminado == &numero2, "Elimino la raiz de un heap con 2 elementos");
    elemento_eliminado = eliminar_raiz(h2);
    pa2m_afirmar(elemento_eliminado == &numero1, "Elimino la raiz de un heap con un elemento");
    elemento_eliminado = eliminar_raiz(h2);
    pa2m_afirmar(elemento_eliminado == NULL, "Elimino la raiz de un heap vacio");

    agregar_nodo(h, &numero1);
    agregar_nodo(h, &numero2);
    agregar_nodo(h, &numero3);
    agregar_nodo(h, &numero4);
    agregar_nodo(h, &numero5);
    agregar_nodo(h, &numero6);
    agregar_nodo(h, &numero7);
    agregar_nodo(h, &numero8);
    agregar_nodo(h, &numero9);
    agregar_nodo(h, &numero10);
    agregar_nodo(h, &numero11);

    void* numero_minimo = ver_nodo_mas_chico(h);
    pa2m_afirmar(numero_minimo == &numero7, "El numero chico es el correcto");
    
    elemento_eliminado = eliminar_raiz(h);
    pa2m_afirmar(elemento_eliminado == &numero7, "La raiz eliminada es la correcta");
    
    numero_minimo = ver_nodo_mas_chico(h);
    pa2m_afirmar(numero_minimo == &numero6, "Luego de eliminar la raiz, el numero mas chico es el correcto");
    
    elemento_eliminado = eliminar_raiz(h);
    pa2m_afirmar(elemento_eliminado == &numero6, "La raiz eliminada es la correcta");
    
    elemento_eliminado = eliminar_raiz(h);
    pa2m_afirmar(elemento_eliminado == &numero9, "La raiz eliminada es la correcta");

    elemento_eliminado = eliminar_raiz(h);
    pa2m_afirmar(elemento_eliminado == &numero5, "La raiz eliminada es la correcta");
    
    numero_minimo = ver_nodo_mas_chico(h);
    pa2m_afirmar(numero_minimo == &numero11, "Luego de eliminar la raiz varias veces, el numero mas chico es el correcto");


    destruir_heap(h);
    destruir_heap(h2);
}


// Pruebas del simulador
void prueboCrearUnSimuladorYDestruirlo(){
    simulador_t* s_invalido = simulador_crear(NULL);
    pa2m_afirmar(s_invalido == NULL,"Si le paso un hospital invalido devuelve NULL");

    hospital_t* h_vacio = hospital_crear();
    hospital_t* h_vacio_2 = hospital_crear();
    hospital_leer_archivo(h_vacio_2, "ejemplos/archivo_vacio.hospital");
    
    simulador_t* s_vacio = simulador_crear(h_vacio);
    simulador_t* s_vacio_2 = simulador_crear(h_vacio_2);


    pa2m_afirmar(s_vacio != NULL, "Puedo crear un simulador con un hospital vacio");
    pa2m_afirmar(s_vacio_2 != NULL, "Puedo crear un simulador con un hospital que leyo un archivo vacio");

    hospital_t* h = hospital_crear();
    hospital_leer_archivo(h, "ejemplos/varios_entrenadores.hospital");

    simulador_t* simulador = simulador_crear(h);
    pa2m_afirmar(simulador != NULL, "Puedo crear un simulador correctamente");

    simulador_destruir(s_vacio);
    simulador_destruir(s_vacio_2);
    simulador_destruir(simulador);
}



void prueboObtenerEstadisticasDelSimulador(){
    hospital_t* h = hospital_crear();
    hospital_leer_archivo(h, "ejemplos/varios_entrenadores.hospital");

    simulador_t* simulador = simulador_crear(h);


    EstadisticasSimulacion* estadisticas = calloc(1, sizeof(EstadisticasSimulacion));
    if(!estadisticas){
        simulador_destruir(simulador);
        free(estadisticas);
        return;
    }

    pa2m_afirmar(simulador_simular_evento(simulador, ObtenerEstadisticas, estadisticas) == ExitoSimulacion, "Puedo obtener las estadisticas del simulador");

    pa2m_afirmar(estadisticas->entrenadores_atendidos == 0, "Los entrenadores atendidos son los correctos");
    pa2m_afirmar(estadisticas->entrenadores_totales == 5, "Los entrenadores totales son los correctos");
    pa2m_afirmar(estadisticas->pokemon_atendidos == 0, "Los pokemones atendidos son los correctos");
    pa2m_afirmar(estadisticas->pokemon_en_espera == 0, "Los pokemones en espera son los correctos");
    pa2m_afirmar(estadisticas->pokemon_totales == 24, "Los pokemones totales son los correctos");
    pa2m_afirmar(estadisticas->puntos == 0, "Los puntos son los correctos");
    pa2m_afirmar(estadisticas->cantidad_eventos_simulados == 1, "La cantidad de eventos simulados son los correctos");


    simulador_destruir(simulador);
    free(estadisticas);
}


void prueboAtenderAlSiguientePokemonDelSimulador(){
    hospital_t* h = hospital_crear();
    hospital_leer_archivo(h, "ejemplos/varios_entrenadores.hospital");

    simulador_t* s = simulador_crear(h);
    EstadisticasSimulacion* estadisticas = calloc(1, sizeof(EstadisticasSimulacion));

    if(!estadisticas || !estadisticas){
        simulador_destruir(s);
        free(estadisticas);
        return;
    }

    pa2m_afirmar(simulador_simular_evento(s, AtenderProximoEntrenador, NULL) == ExitoSimulacion, "Puedo atender al siguiente Pokemon");
    simulador_simular_evento(s, ObtenerEstadisticas, estadisticas);
    
    pa2m_afirmar(estadisticas->entrenadores_atendidos == 1, "Los entrenadores atendidos son los correctos");
    pa2m_afirmar(estadisticas->entrenadores_totales == 5, "Los entrenadores totales son los correctos");
    pa2m_afirmar(estadisticas->pokemon_atendidos == 0, "Los pokemones atendidos son los correctos");
    pa2m_afirmar(estadisticas->pokemon_en_espera == 3, "Los pokemones en espera son los correctos");
    pa2m_afirmar(estadisticas->pokemon_totales == 24, "Los pokemones totales son los correctos");
    pa2m_afirmar(estadisticas->puntos == 0, "Los puntos son los correctos");
    pa2m_afirmar(estadisticas->cantidad_eventos_simulados == 2, "La cantidad de eventos simulados son los correctos");

    simulador_destruir(s);
    free(estadisticas);
}


void prueboObtenerInformacionDelPokemonEnTratamientoDelSimulador(){
    hospital_t* h = hospital_crear();
    hospital_leer_archivo(h, "ejemplos/varios_entrenadores.hospital");

    simulador_t* s = simulador_crear(h);

    InformacionPokemon* info_pokemon = calloc(1, sizeof(InformacionPokemon));
    EstadisticasSimulacion* estadisticas = calloc(1, sizeof(EstadisticasSimulacion));
    if(!info_pokemon || !estadisticas){
        simulador_destruir(s);
        free(estadisticas);
        free(info_pokemon);
        return;    
    }
    
    simulador_simular_evento(s, AtenderProximoEntrenador, NULL);
    pa2m_afirmar(simulador_simular_evento(s, ObtenerInformacionPokemonEnTratamiento, info_pokemon) == ExitoSimulacion, "Puedo obtener la informacion del pokemon en tratamiento");
    pa2m_afirmar((strcmp(info_pokemon->nombre_pokemon, "rampardos") == 0), "El nombre del pokemon es el correcto");
    pa2m_afirmar((strcmp(info_pokemon->nombre_entrenador, "lucas") == 0), "El nombre del entrenador es el correcto");
    
    simulador_simular_evento(s, ObtenerEstadisticas, estadisticas);
    pa2m_afirmar(estadisticas->cantidad_eventos_simulados == 3, "La cantidad de eventos simulados son los correctos");

    simulador_destruir(s);
    free(info_pokemon);
    free(estadisticas);
}


void prueboAdivinarElNivelDelPokemon(){
    hospital_t* h = hospital_crear();
    hospital_leer_archivo(h, "ejemplos/varios_entrenadores.hospital");

    simulador_t* s = simulador_crear(h);

    Intento* intento_pokemon = calloc(1, sizeof(Intento));
    EstadisticasSimulacion* estadisticas = calloc(1, sizeof(EstadisticasSimulacion));
    if(!intento_pokemon || !estadisticas){
        simulador_destruir(s);
        free(estadisticas);
        free(intento_pokemon);
        return;    
    }

    simulador_simular_evento(s, AtenderProximoEntrenador, NULL);

    intento_pokemon->nivel_adivinado = 9;
    pa2m_afirmar(simulador_simular_evento(s, AdivinarNivelPokemon, intento_pokemon) == ExitoSimulacion, "Puedo adivinar el nivel del pokemon");
    pa2m_afirmar(intento_pokemon->es_correcto == false, "Al adivinar un nivel incorrecto, este devuelve que te equivocaste");
    pa2m_afirmar(strcmp("Es un nivel mas alto", intento_pokemon->resultado_string) == 0, "El feedback que da la dificultad es el correcto");

    intento_pokemon->nivel_adivinado = 10;
    pa2m_afirmar(simulador_simular_evento(s, AdivinarNivelPokemon, intento_pokemon) == ExitoSimulacion, "Puedo adivinar el nivel del pokemon");
    pa2m_afirmar(intento_pokemon->es_correcto == true, "Puedo adivinar el nivel correcto");
    pa2m_afirmar(strcmp("Resultado Correcto", intento_pokemon->resultado_string) == 0, "El feedback que da la dificultad es el correcto");

    simulador_simular_evento(s, ObtenerEstadisticas, estadisticas);
    pa2m_afirmar(estadisticas->puntos == 10, "La cantidad de puntos conseguidos es correcta");
    pa2m_afirmar(estadisticas->pokemon_atendidos == 1, "La cantidad de pokemones atendidos es correcta");
    pa2m_afirmar(estadisticas->pokemon_en_espera == 2, "La cantidad de pokemones en espera es correcta");
    pa2m_afirmar(estadisticas->cantidad_eventos_simulados == 4, "La cantidad de eventos simulados son los correctos");

    simulador_destruir(s);
    free(estadisticas);
    free(intento_pokemon);
}


void prueboObtenerInformacionDeUnaDificultad(){
    hospital_t* h = hospital_crear();
    hospital_leer_archivo(h, "ejemplos/varios_entrenadores.hospital");

    simulador_t* s = simulador_crear(h);

    InformacionDificultad* info_dificultad = calloc(1, sizeof(InformacionDificultad));
    EstadisticasSimulacion* estadisticas = calloc(1, sizeof(EstadisticasSimulacion));
    if(!info_dificultad || !estadisticas){
        simulador_destruir(s);
        free(estadisticas);
        free(info_dificultad);
        return;    
    }

    info_dificultad->id = 0;
    
    pa2m_afirmar(simulador_simular_evento(s, ObtenerInformacionDificultad, info_dificultad) == ExitoSimulacion, "Puedo obtener la informacion de una dificultad");
    pa2m_afirmar(info_dificultad->en_uso == false, "La dificultad no esta en uso");
    pa2m_afirmar(strcmp(info_dificultad->nombre_dificultad, "Facil") == 0, "El nombre de la dificultad buscada es el correcto");

    simulador_simular_evento(s, ObtenerEstadisticas, estadisticas);
    pa2m_afirmar(estadisticas->cantidad_eventos_simulados == 2, "La cantidad de eventos simulados son los correctos");

    info_dificultad->id = 8;
    pa2m_afirmar(simulador_simular_evento(s, ObtenerInformacionDificultad, info_dificultad) == ErrorSimulacion, "No puedo encontrar una dificultad que no esta en el simulador");

    simulador_destruir(s);
    free(estadisticas);
    free(info_dificultad);
}


void prueboSleccionarUnaDificultad(){
    hospital_t* h = hospital_crear();
    hospital_leer_archivo(h, "ejemplos/varios_entrenadores.hospital");

    simulador_t* s = simulador_crear(h);

    InformacionDificultad* info_dificultad = calloc(1, sizeof(InformacionDificultad));
    EstadisticasSimulacion* estadisticas = calloc(1, sizeof(EstadisticasSimulacion));
    if(!info_dificultad || !estadisticas){
        simulador_destruir(s);
        free(estadisticas);
        free(info_dificultad);
        return;    
    }

    info_dificultad->id = 2;
    int id_dificultad = 2;
    pa2m_afirmar(simulador_simular_evento(s, SeleccionarDificultad, &id_dificultad) == ExitoSimulacion, "Puedo seleccionar una dificultad");
    
    pa2m_afirmar(simulador_simular_evento(s, ObtenerInformacionDificultad, info_dificultad) == ExitoSimulacion, "Puedo obtener la informacion de una dificultad");
    pa2m_afirmar(info_dificultad->en_uso == true, "La dificultad dificil esta en uso");
    pa2m_afirmar(strcmp(info_dificultad->nombre_dificultad, "Dificil") == 0, "La dificultad en uso tiene el nombre correcto");

    simulador_simular_evento(s, ObtenerEstadisticas, estadisticas);
    pa2m_afirmar(estadisticas->cantidad_eventos_simulados == 3, "La cantidad de eventos simulados son los correctos");

    simulador_destruir(s);
    free(estadisticas);
    free(info_dificultad);
}


unsigned calcular_puntaje_dificultad_nueva(unsigned cantidad_intentos){
    if(cantidad_intentos == 1)
        return 322;
    return 100;
}

int verificar_nivel_dificultad_nueva(unsigned nivel_adivinado, unsigned nivel_pokemon){
    if(nivel_adivinado == nivel_pokemon)
        return 0;
    return 1;
}

const char* verificacion_a_string_dificultad_nueva(int resultado_verificacion){
    if(resultado_verificacion == 0)
        return "Es la respuesta";
    return "No es la respuesta";
}

void prueboAgregarUnaDificultadAlSimulador(){
    hospital_t* h = hospital_crear();
    hospital_leer_archivo(h, "ejemplos/varios_entrenadores.hospital");

    simulador_t* s = simulador_crear(h);

    DatosDificultad* dificultad_nueva = calloc(1, sizeof(DatosDificultad));
    EstadisticasSimulacion* estadisticas = calloc(1, sizeof(EstadisticasSimulacion));
    InformacionDificultad* info_dificultad = calloc(1, sizeof(InformacionDificultad));
    Intento* intento_pokemon = calloc(1, sizeof(Intento));
    if(!dificultad_nueva || !estadisticas || !info_dificultad || !intento_pokemon){
        simulador_destruir(s);
        free(estadisticas);
        free(dificultad_nueva);
        free(info_dificultad);
        free(intento_pokemon);
        return;    
    }

    dificultad_nueva->nombre = "Dificultad nueva";
    dificultad_nueva->calcular_puntaje = calcular_puntaje_dificultad_nueva;
    dificultad_nueva->verificar_nivel = verificar_nivel_dificultad_nueva;
    dificultad_nueva->verificacion_a_string = verificacion_a_string_dificultad_nueva;
    
    info_dificultad->id = 1;

    simulador_simular_evento(s, AtenderProximoEntrenador, NULL);
    simulador_simular_evento(s, ObtenerInformacionDificultad, info_dificultad);
    pa2m_afirmar(info_dificultad->en_uso,"La dificultad en uso es la normal");

    simulador_simular_evento(s, ObtenerEstadisticas, estadisticas);
    pa2m_afirmar(estadisticas->puntos == 0,"Tiene 0 puntos");

    pa2m_afirmar(simulador_simular_evento(s, AgregarDificultad, dificultad_nueva) == ExitoSimulacion, "Puedo agregar una nueva dificultad");
    int id_dificultad = 3;
    pa2m_afirmar(simulador_simular_evento(s, SeleccionarDificultad, &id_dificultad) == ExitoSimulacion, "Puedo seleccionar la nueva dificultad");
    info_dificultad->id = 3;
    simulador_simular_evento(s, ObtenerInformacionDificultad, info_dificultad);
    pa2m_afirmar(info_dificultad->en_uso,"La dificultad en uso es la nueva");
    pa2m_afirmar(strcmp(info_dificultad->nombre_dificultad, "Dificultad nueva") == 0, "La dificultad nueva tiene el nombre correcto");

    simulador_simular_evento(s, ObtenerEstadisticas, estadisticas);
    pa2m_afirmar(estadisticas->cantidad_eventos_simulados == 7,"La cantidad de eventos simulados es la correcta");

    intento_pokemon->nivel_adivinado = 10;
    simulador_simular_evento(s, AdivinarNivelPokemon, intento_pokemon);
    pa2m_afirmar(intento_pokemon->es_correcto, "El nivel adivinado es el correcto");
    simulador_simular_evento(s, ObtenerEstadisticas, estadisticas);
    pa2m_afirmar(estadisticas->puntos == 322, "La cantidad de puntos al adivinar en la nueva dificultad es la correcta");

    intento_pokemon->nivel_adivinado = 19;
    simulador_simular_evento(s, AdivinarNivelPokemon, intento_pokemon);
    pa2m_afirmar(!intento_pokemon->es_correcto, "El nivel adivinado es incorrecto");
    
    intento_pokemon->nivel_adivinado = 20;
    simulador_simular_evento(s, AdivinarNivelPokemon, intento_pokemon);
    pa2m_afirmar(intento_pokemon->es_correcto, "El nivel adivinado es correcto");
    simulador_simular_evento(s, ObtenerEstadisticas, estadisticas);
    pa2m_afirmar(estadisticas->puntos == 422, "La cantidad de puntos al adivinar se suman a los anteriores");

    simulador_destruir(s);
    free(estadisticas);
    free(dificultad_nueva);
    free(info_dificultad);
    free(intento_pokemon);
}


void prueboFinalizarElSimulador(){
    hospital_t* h = hospital_crear();
    hospital_leer_archivo(h, "ejemplos/varios_entrenadores.hospital");

    simulador_t* s = simulador_crear(h);

    pa2m_afirmar(simulador_simular_evento(s, FinalizarSimulacion, NULL) == ExitoSimulacion, "Puedo finalizar la simulacion");
    pa2m_afirmar(simulador_simular_evento(s, AtenderProximoEntrenador, NULL) == ErrorSimulacion, "Luego de finalizar la simulacion no puedo llamar a ningun evento");
    
    simulador_destruir(s);
}

void pruebasConUnEntrenador(){
    hospital_t* h_con_un_entrenador = hospital_crear();
    hospital_leer_archivo(h_con_un_entrenador, "ejemplos/un_entrenador.hospital");

    simulador_t* simulador_con_un_entrenador = simulador_crear(h_con_un_entrenador);
    InformacionPokemon* info_pokemon = calloc(1, sizeof(InformacionPokemon));
    EstadisticasSimulacion* estadisticas = calloc(1, sizeof(EstadisticasSimulacion));
    Intento* intento_pokemon = calloc(1, sizeof(Intento));

    if(!info_pokemon || !estadisticas || !intento_pokemon){
        simulador_destruir(simulador_con_un_entrenador);
        free(info_pokemon);
        free(estadisticas);
        free(intento_pokemon);
        return;
    }

    pa2m_afirmar(simulador_simular_evento(simulador_con_un_entrenador, ObtenerEstadisticas, estadisticas) == ExitoSimulacion, "Puedo obtener las estadisticas del simulador con un entrenador");
    pa2m_afirmar(estadisticas->entrenadores_totales == 1, "La cantidad de entrenadores totales es uno");
    pa2m_afirmar(simulador_simular_evento(simulador_con_un_entrenador, AtenderProximoEntrenador, NULL) == ExitoSimulacion, "Puedo atender al siguiente Entrenador");
    pa2m_afirmar(simulador_simular_evento(simulador_con_un_entrenador, ObtenerInformacionPokemonEnTratamiento, info_pokemon) == ExitoSimulacion, "Puedo obtener la informacion del pokemon en tratamiento");
    pa2m_afirmar(strcmp(info_pokemon->nombre_pokemon, "lapras") == 0, "El pokemon en tratamiento es el menor");
    simulador_simular_evento(simulador_con_un_entrenador, ObtenerEstadisticas, estadisticas);
    pa2m_afirmar(estadisticas->pokemon_en_espera == 2, "Son dos pokemones en espera");

    intento_pokemon->nivel_adivinado = 40;
    pa2m_afirmar(simulador_simular_evento(simulador_con_un_entrenador, AdivinarNivelPokemon, intento_pokemon) == ExitoSimulacion, "Puedo adivinar el nivel del pokemon");
    pa2m_afirmar(intento_pokemon->es_correcto == true, "Puedo adivinar el nivel");
    simulador_simular_evento(simulador_con_un_entrenador, ObtenerInformacionPokemonEnTratamiento, info_pokemon);
    intento_pokemon->nivel_adivinado = 50;
    pa2m_afirmar(simulador_simular_evento(simulador_con_un_entrenador, AdivinarNivelPokemon, intento_pokemon) == ExitoSimulacion, "Puedo adivinar el nivel del pokemon");
    pa2m_afirmar(intento_pokemon->es_correcto == true, "Puedo adivinar el nivel");
    simulador_simular_evento(simulador_con_un_entrenador, ObtenerInformacionPokemonEnTratamiento, info_pokemon);

    simulador_destruir(simulador_con_un_entrenador);
    free(info_pokemon);
    free(estadisticas);
    free(intento_pokemon);
}


int main(){

    pa2m_nuevo_grupo("Pruebas con el hospital");

    pa2m_nuevo_grupo("Pruebas de  creación y destrucción");
    puedoCrearYDestruirUnHospital();

    pa2m_nuevo_grupo("Pruebas con NULL");
    dadoUnHospitalNULL_lasPuedoAplicarLasOperacionesDelHospitalSinProblema();

    pa2m_nuevo_grupo("Pruebas con un archivo vacío");
    dadoUnArchivoVacio_NoSeAgreganPokemonAlHospital();

    pa2m_nuevo_grupo("Pruebas con un archivo de un entrenador");
    dadoUnArchivoConUnEntrenador_SeAgregaElEntrenadorYSusPokemonAlHospital();

    pa2m_nuevo_grupo("Pruebas con un archivo de varios entrenadores");
    dadoUnArchivoConVariosEntrenadores_SeAgreganLosEntrenadoresYSusPokemonAlHospital();

    pa2m_nuevo_grupo("Pruebas con mas de un archivo");
    dadosVariosArchivos_puedoAgregarlosTodosAlMismoHospital();

    pa2m_nuevo_grupo("Pruebas con guardar un archivo");
    puedoGuardarUnArchivoDeUnHospital();

//-------------------------------------------------------------------------------------

    pa2m_nuevo_grupo("Pruebas con el Heap");
    
    pa2m_nuevo_grupo("Pruebas de crear un heap");
    prueboCrearUnHeap_y_Destruirlo();

    pa2m_nuevo_grupo("Pruebas de agregar un elemento al heap");
    prueboAgregarUnElementoAlHeap();

    pa2m_nuevo_grupo("Pruebas de eliminar un elemento al heap");
    prueboEliminarUnElementoAlHeap();

//-------------------------------------------------------------------------------------

    pa2m_nuevo_grupo("Pruebas del simulador");

    pa2m_nuevo_grupo("Pruebas de crear un simulador y destruirlo");
    prueboCrearUnSimuladorYDestruirlo();

    pa2m_nuevo_grupo("Pruebas de obtener estadisticas");
    prueboObtenerEstadisticasDelSimulador();

    pa2m_nuevo_grupo("Pruebas de atender siguiente pokemon");
    prueboAtenderAlSiguientePokemonDelSimulador();

    pa2m_nuevo_grupo("Pruebas de obtener informacion del pokemon en tratamiento");
    prueboObtenerInformacionDelPokemonEnTratamientoDelSimulador();

    pa2m_nuevo_grupo("Pruebas de adivinar nivel del pokemon");
    prueboAdivinarElNivelDelPokemon();

    pa2m_nuevo_grupo("Pruebas de obtener informacion de una dificultad");
    prueboObtenerInformacionDeUnaDificultad();

    pa2m_nuevo_grupo("Pruebas de seleccionar de una dificultad");
    prueboSleccionarUnaDificultad();

    pa2m_nuevo_grupo("Pruebas de agregar dificultad");
    prueboAgregarUnaDificultadAlSimulador();

    pa2m_nuevo_grupo("Pruebas de finalizar simulador");
    prueboFinalizarElSimulador();

    pa2m_nuevo_grupo("Pruebas con un entrenador");
    pruebasConUnEntrenador();

    return pa2m_mostrar_reporte();
}
