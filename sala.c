#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define MAX_CAPACITY 2000000 // Capacidas máxima permitida.
#define MAX_LENGTH 1000
int capacity = -1;           // Capacidad de la sala actual. -1 indica que no existe sala.
int occupied = 0;            // Número de asientos ocupados.
int* room;                   // Representación de la sala.
char jump = '\n';

int getCapacity() {
    return capacity;
}

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

int guarda_estado_sala(const char* filename) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd == -1) {
        return -1;
    }
    char p[10];  
    sprintf(p, "%d", capacity);
    write(fd, p, strlen(p));
    write(fd, &jump, 1);

    for (int i = 0; i < capacity; i++) {
        char p[10];  
        sprintf(p, "%d", i);
        write(fd, p, strlen(p));
        sprintf(p, " %d", room[i]);
        write(fd, p, strlen(p));
        write(fd, &jump, 1);
    }

    close(fd);
    return 0;
}
int recupera_estado_sala(const char* ruta_fichero) {
    int fd = open(ruta_fichero, O_RDONLY);
    if (fd == -1) {
        return -1;
    }

    char leeByte[10];
    char acumStr[10];
    memset(acumStr, 0, sizeof(acumStr));

    while (read(fd, leeByte, 1) == 1) {
        if (leeByte[0] == jump) {
            break;
        }
        strncat(acumStr, leeByte, 1); 
    }

    elimina_sala();
    crea_sala(atoi(acumStr));
    int flaggi = 0;

    for (int i=0; i < capacity; i++){
        memset(acumStr, 0, sizeof(acumStr)); 
        while (read(fd, leeByte, 1) == 1) {
            if (leeByte[0] == ' ') {
                flaggi = 1;
                continue;
            }

            if (leeByte[0] == jump) {
                flaggi = 0;
                break;
            }

            if (flaggi == 1) {
               strncat(acumStr, leeByte, 1);
            }
        }

        room[i] = atoi(acumStr);

        if (room[i] != -1){
            occupied++;
        }
    }
     
    close(fd);
    return 0;
}


int guarda_estadoparcial_sala(const char* ruta_fichero, size_t num_asientos, int* id_asientos) {
    if (guarda_estado_sala("aux") == -1) return -1;

    int* old = malloc(capacity*sizeof(int));

    for (int i = 0; i < capacity; i++) {
        old[i] = room[i];
    }

    if (recupera_estado_sala(ruta_fichero) == -1) return -1;

    for (int i = 0; i < num_asientos; i++) {
        libera_asiento(id_asientos[i]);
        reserva_asiento(old[id_asientos[i]]);
    }

    if (guarda_estado_sala(ruta_fichero) == -1) return -1;

    if (recupera_estado_sala("aux") == -1) return -1;

    if (fork() == 0) {
        execlp("rm", "rm", "aux");
    } else {
        wait(NULL);
    }

    return 0;
}

static int contains(int* asientos, int size, int id) {
    for (int i = 0; i < size; i++) {
        if (asientos[i] == id) return 1;
    }

    return 0;
}

int recupera_estadoparcial_sala(const char* ruta_fichero, size_t num_asientos, int* id_asientos) {
    if (guarda_estado_sala("aux") == -1) return -1;

    int* old = malloc(capacity*sizeof(int));
    int oldCapacity;

    for (int i = 0; i < capacity; i++) {
        old[i] = room[i];
    }

    if (recupera_estado_sala(ruta_fichero) == -1) return -1;

    for (int i = 0; i < num_asientos; i++) {
        old[id_asientos[i]] = estado_asiento(id_asientos[i]);
    }

    if (recupera_estado_sala("aux") == -1) return -1;

    for (int i = 0; i < num_asientos; i++) {
        libera_asiento(id_asientos[i]);
        room[id_asientos[i]] = old[id_asientos[i]];
    }

    if (fork() == 0) {
        execlp("rm", "rm", "aux");
    } else {
        wait(NULL);
    }

    return 0;
}