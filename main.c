#include "src/hospital.h"
#include "src/simulador.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct{
    simulador_t* simulador;
    bool se_esta_jugando;
}Juego;


char leer_comando(){
    char linea[100];
    char* leido;
    leido = fgets(linea, 100, stdin);
    if(!leido)
        return 0;
    while(*leido == ' ')
        leido++;
    return (char)tolower(*leido);
}


void mostrar_estadisticas(Juego* juego){
    EstadisticasSimulacion e;
    simulador_simular_evento(juego->simulador, ObtenerEstadisticas, &e);

    printf("Entrenadores atendidos: %u\n", e.entrenadores_atendidos);
    printf("Entrenadores totales: %u\n", e.entrenadores_totales);
    printf("Pokemones atendidos: %u\n", e.pokemon_atendidos);
    printf("Pokemones en espera: %u\n", e.pokemon_en_espera);
    printf("Pokemones totales: %u\n", e.pokemon_totales);
    printf("Puntos: %u\n", e.puntos);
    printf("Cantidad de eventos simulados: %u\n", e.cantidad_eventos_simulados);
}

void atender_siguiente_entrenador(Juego* juego){
    simulador_simular_evento(juego->simulador, AtenderProximoEntrenador, NULL);
    printf("Se atendio el proximo entrenador\n");
}

void info_pokemon_en_tratamiento(Juego* juego){
    InformacionPokemon info_pokemon;
    simulador_simular_evento(juego->simulador, ObtenerInformacionPokemonEnTratamiento, &info_pokemon);
    printf("El pokemon en tratamiento es: %s\n", info_pokemon.nombre_pokemon);
    printf("Su entrenador es: %s\n", info_pokemon.nombre_entrenador);
}


void adivinar_pokemon(Juego* juego){
    bool adivino_pokemon = false;
    Intento intento_pokemon;

    while(!adivino_pokemon){
        printf("\n");
        printf("Ingrese el nivel del pokemon\n");
        printf("> ");
        scanf("%i", &(intento_pokemon.nivel_adivinado));
        simulador_simular_evento(juego->simulador, AdivinarNivelPokemon, &intento_pokemon);
        if(intento_pokemon.es_correcto)
            adivino_pokemon = true;
        printf("%s\n", intento_pokemon.resultado_string);
        printf("\n");
    }
}


void seleccionar_otra_dificultad(Juego* juego){
    unsigned id;
    printf("Ingrese el id de la dificultad deseada\n");
    printf("> ");
    scanf("%u", &id);
    simulador_simular_evento(juego->simulador, SeleccionarDificultad, &id);
}


void obtener_info_dificultades(Juego* juego){
    InformacionDificultad info_dificultad;
    info_dificultad.id = 0;
    if(simulador_simular_evento(juego->simulador, ObtenerInformacionDificultad, &info_dificultad) == ExitoSimulacion){
        if(info_dificultad.en_uso){
            printf("0. %s (en uso)\n", info_dificultad.nombre_dificultad);
        }else{
            printf("0. %s\n", info_dificultad.nombre_dificultad);
        }
    }

    info_dificultad.id = 1;
    if(simulador_simular_evento(juego->simulador, ObtenerInformacionDificultad, &info_dificultad) == ExitoSimulacion){
        if(info_dificultad.en_uso){
            printf("1. %s (en uso)\n", info_dificultad.nombre_dificultad);
        }else{
            printf("1. %s\n", info_dificultad.nombre_dificultad);
        }
    }

    info_dificultad.id = 2;
    if(simulador_simular_evento(juego->simulador, ObtenerInformacionDificultad, &info_dificultad) == ExitoSimulacion){
        if(info_dificultad.en_uso){
            printf("2. %s (en uso)\n", info_dificultad.nombre_dificultad);
        }else{
            printf("2. %s\n", info_dificultad.nombre_dificultad);
        }
    }

    info_dificultad.id = 3;
    if(simulador_simular_evento(juego->simulador, ObtenerInformacionDificultad, &info_dificultad) == ExitoSimulacion){
        if(info_dificultad.en_uso){
            printf("3. %s (en uso)\n", info_dificultad.nombre_dificultad);
        }else{
            printf("3. %s\n", info_dificultad.nombre_dificultad);
        }
    }

    info_dificultad.id = 4;
    if(simulador_simular_evento(juego->simulador, ObtenerInformacionDificultad, &info_dificultad) == ExitoSimulacion){
        if(info_dificultad.en_uso){
            printf("4. %s (en uso)\n", info_dificultad.nombre_dificultad);
        }else{
            printf("4. %s\n", info_dificultad.nombre_dificultad);
        }
    }
}


void mostrar_comandos(){
    printf("(e) Mostrar estadisticas\n");
    printf("(p) Atender siguiente entrenador\n");
    printf("(i) Obtener informacion pokemon en tratamiento\n");
    printf("(a) Adivinar pokemon\n");
    printf("(d) Seleccionar dificultad\n");
    printf("(o) Ver dificultades\n");
    printf("(h) Mostrar comandos\n");
    printf("(q) Finalizar simulador\n");
}


void ejecutar_comando(Juego *juego, char comando){
    switch (comando) {
        case 'q':
            simulador_simular_evento(juego->simulador, FinalizarSimulacion, NULL);
            juego->se_esta_jugando = false;
            break;
        case 'e':
            mostrar_estadisticas(juego);
            break;
        case 'p':
            atender_siguiente_entrenador(juego);
            break;
        case 'i':
            info_pokemon_en_tratamiento(juego);
            break;
        case 'a':
            adivinar_pokemon(juego);
            break;
        case 'd':
            seleccionar_otra_dificultad(juego);
            break;
        case 'o':
            obtener_info_dificultades(juego);
            break;
        case 'h':
            mostrar_comandos();
            break;
    }
}


void inicializar_juego(Juego* juego, hospital_t* hospital){
    simulador_t* s = simulador_crear(hospital);
    juego->simulador = s;
    juego->se_esta_jugando = true;
}


void destruir_juego(Juego* juego){
    simulador_destruir(juego->simulador);
    free(juego);
}


unsigned calcular_puntaje_dificultad_temperatura(unsigned cantidad_intentos){
    if(cantidad_intentos == 1)
        return 22;
    else if(cantidad_intentos <= 5 && cantidad_intentos > 1)
        return 13;
    else if(cantidad_intentos <= 10 && cantidad_intentos > 5)
        return 5;

    return 2;
}

int verificar_nivel_dificultad_temperatura(unsigned nivel_adivinado, unsigned nivel_pokemon){
    int resultado = (int)(nivel_adivinado - nivel_pokemon);
    
    if(resultado < 0)
        resultado = (resultado*(-1));

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

const char* verificacion_a_string_dificultad_temperatura(int resultado_verificacion){
    if(resultado_verificacion == 0){
        return "Resultado Correcto";
    }
    else if(resultado_verificacion == 5)
        return "Caliente";
    else if(resultado_verificacion == 10)
        return "Tibio";
    else if(resultado_verificacion == 15)
        return "Frio";
    
    return "Congelado";
}


unsigned calcular_puntaje_dificultad_hardcore(unsigned cantidad_intentos){
    if(cantidad_intentos == 1)
        return 300;
    else if(cantidad_intentos <= 5 && cantidad_intentos > 1)
        return 50;
    else if(cantidad_intentos <= 10 && cantidad_intentos > 5)
        return 30;

    return 15;
}

int verificar_nivel_dificultad_hardcore(unsigned nivel_adivinado, unsigned nivel_pokemon){
    int resultado = (int)(nivel_adivinado - nivel_pokemon);
    
    if(resultado < 0)
        resultado = (resultado*(-1));

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

const char* verificacion_a_string_dificultad_hardcore(int resultado_verificacion){
    if(resultado_verificacion == 0){
        return "Resultado Correcto";
    }
    else if(resultado_verificacion == 5)
        return "Chee creo que estas re lejos";
    else if(resultado_verificacion == 10)
        return "MMmm tan lejos no estas";
    else if(resultado_verificacion == 15)
        return "Estas re cerca";
    
    return "Le erraste por uno";
}

void agregar_dos_dificultades(Juego* juego){
    DatosDificultad* dificultad_nueva_1 = calloc(1, sizeof(DatosDificultad));
    DatosDificultad* dificultad_nueva_2 = calloc(1, sizeof(DatosDificultad));

    if(!dificultad_nueva_1 || !dificultad_nueva_2){
        free(dificultad_nueva_1);
        free(dificultad_nueva_2);
        return;
    }

    dificultad_nueva_1->nombre = "Temperatura";
    dificultad_nueva_1->calcular_puntaje = calcular_puntaje_dificultad_temperatura;
    dificultad_nueva_1->verificar_nivel = verificar_nivel_dificultad_temperatura;
    dificultad_nueva_1->verificacion_a_string = verificacion_a_string_dificultad_temperatura;
    simulador_simular_evento(juego->simulador, AgregarDificultad, dificultad_nueva_1);

    dificultad_nueva_2->nombre = "Hardcore";
    dificultad_nueva_2->calcular_puntaje = calcular_puntaje_dificultad_hardcore;
    dificultad_nueva_2->verificar_nivel = verificar_nivel_dificultad_hardcore;
    dificultad_nueva_2->verificacion_a_string = verificacion_a_string_dificultad_hardcore;
    simulador_simular_evento(juego->simulador, AgregarDificultad, dificultad_nueva_2);
}


int main(int argc, char *argv[]) {
    hospital_t* hospital = hospital_crear();
    hospital_leer_archivo(hospital, "ejemplos/varios_entrenadores.hospital");

    Juego* juego = calloc(1, sizeof(Juego));
    if(!juego)
        return -1;
    inicializar_juego(juego, hospital);

    agregar_dos_dificultades(juego);


    mostrar_comandos();
    do{
        printf("> ");
        char c = leer_comando();
        ejecutar_comando(juego, c);
    }while(juego->se_esta_jugando);

    destruir_juego(juego);

    return 0;
}
