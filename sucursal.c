#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "sala.h"
#include <sys/wait.h>
#include <signal.h>

#define MAX_ROOMS 10

pid_t pidSucursales[10] = {0};
char names[MAX_ROOMS][50];
int status;

int num_salas = 0;

void crea_sucursal(const char* ciudad, int capacidad) {
    char capacity[10];
    sprintf(capacity, "%d", capacidad);
    pid_t pid = fork();

    if (pid == 0) {
        printf("Sucursal de %s creada\n", ciudad);

        const char *execute_cmd[] = {"xterm", "-e", "./sala", capacity, ciudad, NULL};
        execvp(execute_cmd[0], (char *const *)execute_cmd);
        exit(1);  
    } else {
        pidSucursales[num_salas] = pid;
        sprintf(names[num_salas++], ciudad);
    }
}

int main() {
    char nombresala[100];
    int capacidad;

    pid_t pid = fork();

    if (pid == 0) {
        char *compile_cmd[] = {"gcc", "sala.c", "-o", "sala", NULL};
        execvp("gcc", compile_cmd);
    }
    
    while (1) {
        int available = 1;
        capacidad = -1;
        printf("Introduzca el nombre de la sala o 'salir' para terminar: \n");
        fgets(nombresala, 100, stdin);

        int i = 0;

        for (i = 0; nombresala[i] != '\0'; i++);

        nombresala[i - 1] = '\0'; 

        if (!strcmp("salir", nombresala)) break;

        for (int i = 0; i < num_salas; i++) {
            if (num_salas >= 10) {
                printf("Se ha alcanzado el número máximo de sucursales permitidas\n");
                available = 0;
                break;
            }
            if (!strcmp(names[i], nombresala)) {
                printf("* ERROR * El nombre introducido ya pertenece a una sucursal\n");
                available = 0;
                break;
            }
        }

        if (available) {
            printf("Introduzca la capacidad de la sala: \n");
again:
            scanf("%d", &capacidad);

            if (capacidad == -1) {
                printf("Introduzca un número como capacidad de la sala:\n");
                while (getchar() != '\n');
                goto again;
            } else {
                crea_sucursal(nombresala, capacidad);
            }

            while (getchar() != '\n');
        }

        for (int i = 0; i < num_salas; i++) {
            pid_t pid = waitpid(pidSucursales[i], &status, WNOHANG);

            if (pid != 0) {
                printf("Se ha cerrado la sucursal de %s\n", names[i]);
                pidSucursales[i] = 0;
                for (int j = 0; names[i][j] != '\0'; j++) {
                    names[i][j] = '\0';
                }
                num_salas--;
            }
        }
    }

    for (int i = 0; i < num_salas; i++) {
        kill(pidSucursales[i], SIGINT);
    }

    exit(1);
}