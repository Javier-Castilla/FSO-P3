#include <stdio.h>
#include <fcntl.h>
#include "sala.h"
#include <string.h>

int main(int argc, char** argv) {
    if (argc >= 5) return -1;
    char* command = argv[1];
    char* op1 = argv[2];
    char* route = argv[3];
    char* op2 = argv[4];
    if (!strcmp(op2, "-c")) {
        int capacity = atoi(argv[5]);
        crea_sala(capacity);
    }

    if (!strcmp(command, "crea")) {
        if (!strcmp(op1, "-f[o]")) {
            guarda_estado_sala(route);
        } else {
            guarda_estado_sala_unico(route);
        }
    } else if (!strcmp(command, "reserva")) {
        recupera_estado_sala(route);

        for (int i = 4; i < argc; i++) {
            reserva_asiento(atoi(argv[i]));
        }
    } else if (!strcmp(command, "anula")) {
        
    }
}