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
        int fd = open(route, O_RDONLY, 0644);

        if (fd != -1) {
            fprintf(stderr, "Error, el archivo ya existe: %s\n", route);
            exit(-1);
        }

        close(fd);
    }

    crea_sala(capacidad);
    guarda_estado_sala(route);
    return 0;
}

int reserva(char* route, char** argv, int idsNumber) {
    if (capacidad_sala() == asientos_ocupados()) {
        fprintf(stderr, "Error al reservar, no hay espacio libre: %s\n");
        exit(-1);
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

    int* noAnulados = malloc(idsNumber*sizeof(int));
    int index = 0;

    if (recupera_estado_sala(route) == -1) {
        fprintf(stderr, "Error al recuperar la sala: %s\n", route);
        exit(-1);
    }

    for (int i = 0; i < idsNumber; i++) {
        int id = atoi(argv[optind++]);
        if (id < 0 || id >= getCapacity()) {
            noAnulados[index++] = id;
            continue;
        }
        if (libera_asiento(id) == -1) {
            fprintf(stderr, "Error al liberar asiento: %d\n", id);
            exit(-1);
        }
    }

    if (guarda_estado_sala(route) == -1) {
        fprintf(stderr, "Error al guardar la sala: %s\n", route);
        exit(-1);
    }

    if (index != 0) {
        fprintf(stderr, "Error al anular algunas reservas con los siguientes IDs: ");
        for (int i = 0; i < index; i++) {
            fprintf(stderr, "%d ", noAnulados[i]);
        }
        fprintf(stderr, "\n");
        exit(-1);
    }

    free(noAnulados);
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

int compare(char* route1, char* route2) {
    int fd1 = open(route1, O_RDONLY, 0644);
    int fd2 = open(route2, O_RDONLY, 0644);
    char buffer1[10];
    char buffer2[10];

    if (fd1 == -1) {
        fprintf(stderr, "Error al abrir el archivo de la sala: %s\n", route1);
        exit(-1);
    }

    if (fd2 == -1) {
        fprintf(stderr, "Error al abrir el archivo de la sala: %s\n", route2);
        exit(-1);
    }

    while (1) {
        size_t size1 = read(fd1, buffer1, 10);
        size_t size2 = read(fd2, buffer2, 10);

        if (memcmp(buffer1, buffer2, sizeof(buffer1))) return 1;

        if (size1 != 10) break;
    }

    return 0;
}

int main(int argc, char** argv) {

    if (argc < 3) {
        printf("Atgumentos no suficientes");
        exit(-1);
    }

    char* command = argv[1];
    char* route;
    char* anotherRoute;
    int fileMode;
    int capacidad;
    int lastArg = -1;

    int opt;
    while ((opt = getopt(argc, argv, ":f:c:a:p:")) != -1) {
        switch (opt)
        {
        case 'f':
            if (!strcmp(optarg, "o")) {
                if (strcmp(command, "crea")) {
                    fprintf(stderr, "Argumento no válido con esta orden: -%c\n", opt);
                    exit(-1);
                }
                fileMode = 1;
                route = argv[optind];
            } else {
                if (strcmp(argv[2], "-f")) {
                    fprintf(stderr, "Argumento no válido con esta orden: %s\n", argv[2]);
                    exit(-1);
                }

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
    } else if (!strcmp(command, "compara")) {
        optind++;
        route = argv[optind++];
        anotherRoute = argv[optind++];
        int rs = compare(route, anotherRoute);
        printf("%d\n", rs);
    } else {
        fprintf(stderr, "Orden no reconocida: %s\n", command);
        exit(-1);
    }

    exit(0);
}
