#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_CAPACITY 2000000 // Capacidas máxima permitida.
#define MAX_LENGTH 34
int capacity = -1;           // Capacidad de la sala actual. -1 indica que no existe sala.
int occupied = 0;            // Número de asientos ocupados.
int* room;                   // Representación de la sala.

/**
 * Se crea una sala con la capacidad indicada.
 * 
 * @param capacidad de la nueva sala
 * @return -1 si ha ocurrido algún error.
 * Se retorna la capacidad si se ha creado correctamente
*/
int crea_sala(int capacidad)
{
    if (capacidad < 1 || capacidad > MAX_CAPACITY || capacity != -1) {
        return -1;
    }

    room = malloc(capacidad*(sizeof(int)));

    for (int i=0; i < capacidad; i++){
        room[i] = -1;
    }

    capacity = capacidad;
    return capacity;

}

/**
 * Se elimina la sala actual.
 * 
 * @return -1 si ha ocurrido algún error.
 * Se retorna 0 si se ha eliminado correctamente
*/
int elimina_sala()
{
    if (capacity == -1) return -1;
    free(room);
    capacity = -1;
    occupied = 0;
    return 0;

}

/**
 * Reserva el asiento y le asigna la id de la persona indicada.
 * 
 * @param id_persona que reserva el asiento
 * @return -1 si ha ocurrido algún error.
 * Se retorna el id del asiento que se ha podido reservar
 * si no ha ocurrido ningún error
*/
int reserva_asiento(int id_persona)
{

    if (capacity - occupied <= 0 || id_persona < 1) return -1;

    for (int i = 0; i < capacity; i++) {
        if (room[i] == -1) {
            room[i] = id_persona;
            occupied++;
            return i;
        }    
    }

    return -1;
}

/**
 * Función que retorna el id de la persona que reservó el asiento
 * 
 * @param id_asiento a liberar
 * @return -1 si ha ocurrido algún error.
 * Se retorna la id de la persona que estaba sentada
 * en el asiento si no hay errores
*/ 
int libera_asiento(int id_asiento)
{
    if (room[id_asiento] != -1 && occupied != 0){
        int temp = room[id_asiento];
        room[id_asiento] = -1;
        occupied--;
        return temp;
    }

    return -1;
}

/**
 * Función que retorna el estado de un asiento 
 * 
 * @param id_asiento a comprobar
 * @return -1 si ha ocurrido algún error.
 * Se retorna 0 si el asiento está libre, y la id
 * de la persona si no lo está
*/ 
int estado_asiento(int id_asiento) 
{
    if (room[id_asiento] > 0){
        return room[id_asiento];
    } else if (room[id_asiento] == -1) {
        return 0;
    }
    return -1;
}

/**
 * Función que retorna la cantidad de asientos libres 
 *
 * @return la cantidad de asientos libres en la sala.
 * Se retorna -1 si no existe la sala
*/ 
int asientos_libres()
{
    return capacity - occupied;
}

/** 
 * Función que retorna la cantidad de asientos ocupados
 *
 * @return el número de asientos ocupados
*/ 
int asientos_ocupados()
{
    return occupied;
}

/** 
 * Función que retorna la capacidad actual de la sala
 *
 * @return la capacidad de la sala.
*/ 
int capacidad_sala()
{
    return capacity;
}

void clear()
{
    while (getchar() != '\n');
}

int guardar_estado_sala(const char* filename) {
    int fd = open(filename, O_WRONLY | O_CREAT, 0666);

    if (fd == -1) {
        return -1;
    }
    write(fd, &capacity, sizeof(int));
    write(fd, "\n", sizeof(char));

    for (int i = 0; i < capacity; i++) {
        write(fd, &i, sizeof(int));
        write(fd, "-", sizeof(char));
        write(fd, &room[i], sizeof(int));
        write(fd, "\n", sizeof(char));
    }

    close(fd);
    // Set file permissions to allow read access for all users
    //chmod(filename, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    return 0;
}

/*    int recupera_estado_sala(const char* ruta_fichero) {
        FILE* file = fopen(ruta_fichero, "r");
        if (file == NULL) {
            return -1;
        }

        // Read the capacity and occupied variables
        fscanf(file, "%d %d\n", &capacity, &occupied);

        // Read the state of each seat
        for (int i = 0; i < capacity; i++) {
            fscanf(file, "%d ", &room[i]);
        }

        fclose(file);
        return 0;
    }

    int guarda_estadoparcial_sala(const char* ruta_fichero, size_t num_asientos, int* id_asientos) {
        FILE* file = fopen(ruta_fichero, "w");
        if (file == NULL) {
            return -1;
        }

        // Write the state of each seat in id_asientos
        for (int i = 0; i < num_asientos; i++) {
            fprintf(file, "%d ", id_asientos[i]);
        }
        fprintf(file, "\n");

        fclose(file);
        return 0;
    }

    int recupera_estadoparcial_sala(const char* ruta_fichero, size_t num_asientos, int* id_asientos) {
        FILE* file = fopen(ruta_fichero, "r");
        if (file == NULL) {
            return -1;
        }

        // Read the state of each seat into id_asientos
        for (int i = 0; i < num_asientos; i++) {
            fscanf(file, "%d ", &id_asientos[i]);
        }

        fclose(file);
        return 0;
    }
*/

int main(int argc, char *argv[]) {
    crea_sala(atoi(argv[1]));
    char *roomName = argv[2];
    printf("======================\nROOM %s\n======================\n", roomName);
    char commands[11][100] = {"reserva <id-persona>\n", "libera <id_asiento>\n", "estado_asiento <id-asiento>\n", "estado_sala\n", "cerrar_sala\n", "clear\n", "quit\n", "guardar_estado <ruta_fichero>\n", 
    "recuperar_estado <ruta_fichero>\n", "guardar_estadoparcial <ruta_fichero> <n_asientos> <id_asientos>\n", "recuperar_estadoparcial <ruta_fichero> <n_asientos> <id_asientos>\n"};
    int commandsLength = 11;
    char command[100];
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
            char* ruta_fichero = arg;
            if (guardar_estado_sala("estado_sala.txt") == -1) {
                printf("Error al guardar el estado de la sala\n");
            }
        } else if (!strcmp(commandStr, "recuperar_estado")) {
            char* ruta_fichero = arg;
            /*if (recupera_estado_sala("estado_sala.txt") == -1) {
                printf("Error al recuperar el estado de la sala\n");
            }*/

        } else if (!strcmp(commandStr, "guardar_estadoparcial")) {
            char* ruta_fichero = arg;
            int n_asientos = atoi(strtok(NULL, " "));
            for (int i=0; i < n_asientos; i++) {
                int id_asientos = atoi(strtok(NULL, " "));
            }
            int id_asientos[n_asientos];
            for (int i = 0; i < n_asientos; i++) {
                id_asientos[i] = atoi(strtok(NULL, " "));
            }
            /*if (guarda_estadoparcial_sala("estado_parcial.txt", n_asientos, id_asientos) == -1) {
                printf("Error al guardar el estado parcial de la sala\n");
            }*/

        } else if (!strcmp(commandStr, "recuperar_estadoparcial")) {
            
            printf("Recuperar estado parcial de la sala\n");

        }
        else if (!strcmp(commandStr, "help\n")) {
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