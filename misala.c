#include <stdio.h>
#include <fcntl.h>
#include "sala.h"
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/wait.h>
#include <bits/getopt_core.h>

int crea(char* route, int capacidad, int mode) {
    if (!mode) {
        int fd = open(route, O_RDONLY | O_EXCL, 0644);

        if (fd = -1) {
            fprintf(stderr, "Error, el archivo ya existe: %s\n", route);
            exit(-1);
        }

        close(fd);
    }

    crea_sala(capacidad);
    guarda_estado_sala(route);
    printf("Estado guardado en: %s\n", route);
    return 0;
}

int reserva(char* route, char** argv, int idsNumber) {
    if (capacidad_sala() == asientos_ocupados()) {
        return 0;
    }

    if (recupera_estado_sala(route) == -1) {
        fprintf(stderr, "Error al abrir el archivo de la sala: %s\n", route);
        exit(-1);
    }

    for (int i = 0; i < idsNumber; i++) {
        if (reserva_asiento(atoi(argv[optind++])) == -1) {
            fprintf(stderr, "Error al reservar asiento: %s\n", argv[--optind]);
            exit(-1);
        }
    }
    
    if (guarda_estado_sala(route) == -1) {
        fprintf(stderr, "Error al guardar la sala: %s\n", route);
        exit(-1);
    }
    
    return 0;
}

int anula(char* route, char** argv, int idsNumber) {
    if (capacidad_sala() == 0) {
        return 0;
    }

    if (recupera_estado_sala(route) == -1) {
        fprintf(stderr, "Error al recuperar la sala: %s\n", route);
        exit(-1);
    }

    for (int i = 0; i < idsNumber; i++) {
        libera_asiento(atoi(argv[optind++]));
    }

    if (guarda_estado_sala(route) == -1) {
        fprintf(stderr, "Error al guardar la sala: %s\n", route);
        exit(-1);
    }

    return 0;
}

int anula_personas(char* route, char** argv, int idsNumber) {
    if (recupera_estado_sala(route) == -1) {
        fprintf(stderr, "Error al recuperar la sala: %s\n", route);
        exit(-1);
    }

    if (capacidad_sala() == 0) {
        return 0;
    }

    for (int i = 0; i < idsNumber; i++) {
        for (int j = 0; j < capacidad_sala(); j++) {
            if (estado_asiento(j) == atoi(argv[optind])) {
                libera_asiento(j);
            }
        }
        optind++;
    }

    if (guarda_estado_sala(route) == -1) {
        fprintf(stderr, "Error al guardar la sala: %s\n", route);
        exit(-1);
    }

    return 0;
}

int estado(char* route) {
    if (recupera_estado_sala(route) == -1) {
        fprintf(stderr, "Error al recuperar la sala: %s\n", route);
        exit(-1);
    }
    
    printf("Capacidad de la sala: %d\nAsientos libres: %d\n", capacidad_sala(), asientos_libres());

    return 0;
}

int main(int argc, char** argv) {

    if (argc < 3) {
        printf("Atgumentos no suficientes");
        exit(-1);
    }

    char* command = argv[1];
    char* route;
    int fileMode;
    int capacidad;
    int lastArg = -1;

    int opt;
    while ((opt = getopt(argc, argv, ":f:c:a:p:")) != -1) {
        switch (opt)
        {
        case 'f':
            if (!strcmp(optarg, "[o]")) {
                if (strcmp(command, "crea")) {
                    fprintf(stderr, "Argumento no válido con esta orden: %c\n", opt);
                    exit(-1);
                }
                fileMode = 1;
                route = argv[optind];
            } else {
                fileMode = 0;
                route = optarg;
            }
            break;
        case 'c':
            capacidad = atoi(optarg);
            break;
        case 'a':
            if (!strcmp(optarg, "sientos")) {
                lastArg = 0;
            } else {
                fprintf(stderr, "Argumento no encontrado: %c\n", optopt);
                exit(-1);
            }
            break;
        case 'p':
            if (!strcmp(optarg, "ersonas")) {
                lastArg = 1;
            } else {
                fprintf(stderr, "Argumento no encontrado: %c\n", optopt);
                exit(-1);
            }
            break;
        case '?':
            fprintf(stderr, "Argumento no encontrado: %c\n", optopt);
            exit(-1);
        }
    }

    if (!strcmp(command, "crea")) {
        crea(route, capacidad, fileMode);
    } else if (!strcmp(command, "reserva")) {
        optind++;
        reserva(route, argv, (argc - 4));
    } else if (!strcmp(command, "anula")) {
        optind++;
        switch (lastArg) 
        {
        case 0:
            anula(route, argv, (argc - 5));
            break;
        case 1:
            anula_personas(route, argv, (argc - 5));
            break;
        case -1:
            fprintf(stderr, "Argumento no encontrado: %c\n", optopt);
            exit(-1);
        }
    } else if (!strcmp(command, "estado")) {
        estado(route);
    } else {
        fprintf(stderr, "Orden no reconocida: %s\n", command);
        exit(-1);
    }

    exit(0);
}
