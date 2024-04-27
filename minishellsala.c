#include <stdio.h>
#include "sala.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_LENGTH 1000

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "%s\n", "Número de argumentos no válido");
        exit(-1);
    }

    crea_sala(atoi(argv[1]));
    char *roomName = argv[2];
    printf("======================\nROOM %s\n======================\n", roomName);
    char commands[11][300] = {"reserva <id-persona>\n", "libera <id_asiento>\n", "estado_asiento <id-asiento>\n", "estado_sala\n", "cerrar_sala\n", "clear\n", "quit\n", "guardar_estado <ruta_fichero>\n", 
    "recuperar_estado <ruta_fichero>\n", "guardar_estadoparcial <ruta_fichero> <n_asientos> <id_asientos>\n", "recuperar_estadoparcial <ruta_fichero> <n_asientos> <id_asientos>\n"};
    int commandsLength = 11;
    char command[3000];
    char *commandStr, *arg;

    while (1) {
        fgets(command, MAX_LENGTH, stdin);

        commandStr = strtok(command, " ");

        arg = strtok(NULL, " ");

        if (!strcmp(commandStr, "reserva")) {
            if (strtok(NULL, " ") != NULL) goto nonav; // si tiene más argumentos, que salte directamente a nonav
            printf("Reservar el asiento: %d\nResultado de la reserva: %d\n", atoi(arg), reserva_asiento(atoi(arg)));
        } else if (!strcmp(commandStr, "libera")) {
            if (strtok(NULL, " ") != NULL) goto nonav; // si tiene más argumentos, que salte directamente a nonav
            printf("Liberar el asiento: %d\nResultado de la liberación: %d\n", atoi(arg), libera_asiento(atoi(arg)));
        } else if (!strcmp(commandStr, "estado_asiento")) {
            printf(
                "Estado del asiento: %d\nResultado de la operación (0 es libre, ID de la persona si está ocupado): %d\n",
                atoi(arg), estado_asiento(atoi(arg))
            );
        } else if (!strcmp(commandStr, "estado_sala\n")) {
            printf(
                "Capacidad de la sala: %d\nAsientos ocupados: %d\nAsientos libres: %d\n",
                capacidad_sala(), asientos_ocupados(), asientos_libres()
            );
        } else if (!strcmp(commandStr, "cerrar_sala\n")) {
            printf("%s\n", "Cerrando sala...\n");
            elimina_sala();
            break;
        } else if (!strcmp(commandStr, "guardar_estado")) {
            char* ruta_fichero = strtok(arg, "\n");
            if (guarda_estado_sala(ruta_fichero) == -1) {
                printf("Error al guardar el estado de la sala\n");
            }
        } else if (!strcmp(commandStr, "recuperar_estado")) {
            char* ruta_fichero = strtok(arg, "\n");
            if (recupera_estado_sala(ruta_fichero) == -1) {
                printf("Error al recuperar el estado de la sala\n");
            }

        } else if (!strcmp(commandStr, "guardar_estadoparcial")) {
            int n_asientos = atoi(strtok(NULL, " "));
            /*if (n_asientos > capacity) {
                printf("El tamaño de la sala es inferior al número de IDs a recuperar\n");
                continue;
            }*/
            int id_asientos[n_asientos];
            for (int i=0; i < n_asientos - 1; i++) {
                id_asientos[i] = atoi(strtok(NULL, " ")); 
            }
            id_asientos[n_asientos - 1] = atoi(strtok(NULL, "\n"));
            if (guarda_estadoparcial_sala(arg, n_asientos, id_asientos) == -1) {
                printf("Error al guardar el estado parcial de la sala\n");
            }
        } else if (!strcmp(commandStr, "recuperar_estadoparcial")) {
            int n_asientos = atoi(strtok(NULL, " "));
            if (n_asientos > getCapacity()) {
                printf("El tamaño de la sala es inferior al número de IDs a recuperar\n");
                continue;
            }
            int id_asientos[n_asientos];
            for (int i=0; i < n_asientos - 1; i++) {
                id_asientos[i] = atoi(strtok(NULL, " ")); 
            }
            id_asientos[n_asientos - 1] = atoi(strtok(NULL, "\n"));
            if (recupera_estadoparcial_sala(arg, n_asientos, id_asientos) == -1){
                printf("Error al recuperar el estado parcial de la sala\n");
            }
        } else if (!strcmp(commandStr, "help\n")) {
            for (int i = 0; i < commandsLength; i++) {
                printf("----- %s", commands[i]);
            }
        } else if (!strcmp(commandStr, "clear\n")) {
            printf("\e[1;1H\e[2J");
            printf("======================\nROOM %s\n======================\n", roomName);
        } else if (!strcmp(commandStr, "quit\n")) {
            printf("%s\n", "Cerrando sala...\n");
            elimina_sala();
            break;
        } else {
            nonav:
            printf("Comando no reconocido\n");
        }
    }
    exit(1);
}