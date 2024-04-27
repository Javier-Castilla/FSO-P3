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

char SALTO = '\n';
char BACKSPACE = '\b';
char delete = '\0';

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void bubbleSort(int arr[], int n) {
    for (int i = 0; i < n-1; i++) {
        for (int j = 0; j < n-i-1; j++) {
            if (arr[j] > arr[j+1]) {
                swap(&arr[j], &arr[j+1]);
            }
        }
    }
}

int crea(char** argv) {
    char* route = argv[1];
    char* argument = argv[2];
    char* capacity = argv[3];

    if (strcmp(argument, "-c")) {
        fprintf(stderr, "Argumento no reconocido: %s\n", argument);
        exit(-1);
    }

    if (!strcmp(argv[0], "-f")) {
        int fd = open(argv[1], O_WRONLY | O_CREAT | O_EXCL, 0644);

        if (fd == -1) {
            fprintf(stderr, "No se ha podido crear el archivo: %s\n", route);
            exit(-1);
        }

        write(fd, capacity, strlen(capacity));
        write(fd, &SALTO, 1);
        write(fd, capacity, strlen(capacity));
        write(fd, &SALTO, 1);
        for (int i = 0; i < atoi(capacity); i++) {
            char num[20];
            sprintf(num, "%d", i);
            write(fd, num, strlen(num));
            write(fd, " ", 1);
            write(fd, "0", 1);
            write(fd, &SALTO, 1);
        }

        close(fd);
    } else if (!strcmp(argv[0], "-f[o]")) {
        crea_sala(atoi(capacity));
        guarda_estado_sala(route);

        /*
        int fd = open(route, O_WRONLY | O_CREAT | O_TRUNC, 0644);

        if (fd == -1) {
            fprintf(stderr, "%s\n", strerror(errno));
            exit(-1);
        }

        write(fd, capacity, strlen(capacity));
        write(fd, &SALTO, 1);
        write(fd, capacity, strlen(capacity));
        write(fd, &SALTO, 1);
        for (int i = 0; i < atoi(capacity); i++) {
            char num[20];
            sprintf(num, "%d", i);
            write(fd, num, strlen(num));
            write(fd, " ", 1);
            write(fd, "0", 1);
            write(fd, &SALTO, 1);
        }

        close(fd);
        */
    } else {
        fprintf(stderr, "Argumento no reconocido: %s\n", argv[0]);
        exit(-1);
    }
    
}

int reserva(char** argv, int idsNumber) {
    char buffer[10];
    char* route = argv[1];
    argv += 2;
    
    int capacity = 0;

    int fd = open(route, O_RDWR, 0644);

    if (fd == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(-1);
    }

    while (1) {
        size_t size = read(fd, buffer, 1);
        if (buffer[0] == SALTO) {
            break;
        } else {
            capacity = capacity * 10 + (buffer[0] - '0');
        }

        if (size == 0) break;
    }

    int free = 0;
    off_t lastOffset = lseek(fd, 0, SEEK_CUR);

    while (1) {
        size_t size = read(fd, buffer, 1);
        if (buffer[0] == SALTO) {
            break;
        } else {
            free = free * 10 + (buffer[0] - '0');
        }

        if (size == 0) break;
    }

    if (free == 0) {
        printf("No hay asientos libres\n");
        exit(-1);
    }

    free -= idsNumber;
    int aux = open("aux", O_RDWR | O_CREAT | O_TRUNC, 0644);

    if (aux == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(-1);
    }

    lseek(fd, -1, SEEK_CUR);

    while (1) {
        size_t size = read(fd, buffer, 10);

        for (int i = 0; i < size; i++) {
            write(aux, &buffer[i], 1);
        }

        if (size != 10) break;
    }

    lseek(fd, lastOffset, SEEK_SET);
    lseek(aux, 0, SEEK_SET);
    char libres[20];
    sprintf(libres, "%d", free);
    write(fd, libres, strlen(libres));

    while (1) {
        size_t size = read(aux, buffer, 10);

        for (int i = 0; i < size; i++) {
            write(fd, &buffer[i], 1);
        }

        if (size != 10) break;
    }

    int flag = 0;
    int num = 0;

    for (int argNum = 0; argNum < idsNumber; argNum++) {
        aux = open("aux", O_RDWR | O_CREAT | O_TRUNC, 0644);

        if (aux == -1) {
            fprintf(stderr, "%s\n", strerror(errno));
            exit(-1);
        }

        char* id = argv[argNum];
        lseek(fd, lastOffset, SEEK_SET);
        
        while (1) {
            size_t size = read(fd, buffer, 1);

            if (size == 0) break;
            if (buffer[0] == '\n') continue;
            if (buffer[0] == ' ') {
                flag = 1;
                continue;
            }
            if (flag && (buffer[0] - '0') == 0) {
                free--;

                lastOffset = lseek(fd, 0, SEEK_CUR);
                
                while (1) {
                    size_t leidos = read(fd, buffer, 10);
                
                    for (int i = 0; i < leidos; i++) {
                        write(aux, &buffer[i], 1);
                    }

                    if (leidos != 10) break;
                }
                
                lseek(fd, lastOffset - 1, SEEK_SET);
                write(fd, id, strlen(id));
                flag = 0;
                break;
            } else {
                flag = 0;
            }
        }
        
        lseek(aux, 0, SEEK_SET);
        lastOffset = lseek(fd, 0, SEEK_CUR);
        
        while (1) {
            size_t leidos = read(aux, buffer, 10);
                
            for (int i = 0; i < leidos; i++) {
                write(fd, &buffer[i], 1);
            }
            
            if (leidos != 10) break;
        }

        close(aux);
    }

    close(fd);

    if (fork() == 0) {
        execlp("rm", "rm", "aux");
    } else {
        wait(NULL);
    }

}

int anula(char** argv, int idsNumber) {
    char buffer[10];
    char* route = argv[1];
    argv += 2;

    int ids[idsNumber];

    for (int i = 0; i < idsNumber; i++) {
        ids[i] = atoi(argv[i]);
    }

    bubbleSort(ids, idsNumber);
    
    int capacity = 0;

    int fd = open(route, O_RDWR, 0644);

    if (fd == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(-1);
    }

    while (1) {
        size_t size = read(fd, buffer, 1);

        if (size == 0) {
            break;
        }

        if (buffer[0] == SALTO) {
            break;
        } else {
            capacity = capacity * 10 + (buffer[0] - '0');
        }
    }

    int free = 0;
    off_t freeOffset = lseek(fd, 0, SEEK_CUR);
    off_t lastOffset = 0;

    while (1) {
        size_t size = read(fd, buffer, 1);

        if (size == 0) {
            break;
        }

        if (buffer[0] == SALTO) {
            break;
        } else {
            free = free * 10 + (buffer[0] - '0');
        }
    }

    off_t nextFReeOffset = lseek(fd, 0, SEEK_CUR);

    if (free == capacity) {
        exit(0);
    }

    int flag = 0;
    int num = 0;

    off_t lastSeat = lseek(fd, 0, SEEK_CUR);

    for (int argNum = 0; argNum < idsNumber; argNum++) {
        int id = ids[argNum];
        int asiento = 0;
        
        // Busco el asiento
        while (1) {
            size_t size = read(fd, buffer, 1);

            if (size == 0) {
                break;
            }

            if (buffer[0] == SALTO) continue;
            if (buffer[0] == ' ') {
                if (asiento != id) {
                    asiento = 0;

                    while(1) {
                        read(fd, buffer, 1);
                        if (buffer[0] == SALTO) {
                            break;
                        }
                    }

                    continue;
                }
                //Aquí he encontrado el asiento y ahora elimino su ID asignada
                int aux = open("aux", O_RDWR | O_CREAT | O_TRUNC, 0644);

                if (aux == -1) {
                    fprintf(stderr, "%s\n", strerror(errno));
                    exit(-1);
                }

                lastOffset = lseek(fd, 0, SEEK_CUR); // Guardo el puntero de la posición antes de la ID
                asiento = 0;
                // Me posiciono al final de la ID y escribo en un fichero auxiliar un cero y el salto de línea que le sigue
                while(1) {
                    read(fd, buffer, 1);
                    if (buffer[0] == SALTO) {
                        write(aux, "0\n", 2);
                        break;
                    }
                }

                // Copio todo lo que viene después en otro fichero
                while (1) {
                    size_t newSize = read(fd, buffer, 10);
                    write(aux, buffer, newSize);
                    if (newSize != 10) break;
                }

                // Trunco eliminando todo lo que viene
                ftruncate(fd, lastOffset);

                lastSeat = lseek(fd, 0, SEEK_END) + 2; // Pongo el puntero a donde lo había guardado antes de la ID
                lseek(aux, 0, SEEK_SET); // Pongo el puntero de AUX al principio para rescribir sobre el otro

                // Empiezo a copiar todo lo de AUX
                while (1) {
                    size_t newSize = read(aux, buffer, 10);
                    write(fd, buffer, newSize);
                    if (newSize != 10) break;
                }

                // Cierro el fichero auxiliar y rompo el bucle para ir a otro asiento
                close(aux);

                if (fork() == 0) {
                    execlp("rm", "rm", "aux");
                } else {
                    wait(NULL);
                }

                break;
            } else if ((buffer[0] - '0') >= 0 && (buffer[0] - '0') <= 9) {
                asiento = asiento * 10 + (buffer[0] - '0');
            }
        }
        lseek(fd, lastSeat, SEEK_SET);
    }

    lseek(fd, nextFReeOffset - 1, SEEK_SET);

    int aux = open("aux", O_RDWR | O_CREAT | O_TRUNC, 0644);

    if (fd == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(-1);
    }

    while (1) {
        size_t size = read(fd, buffer, 10);
        write(aux, buffer, size);

        if (size != 10) break;
    }

    char newFree[20]; 
    sprintf(newFree, "%d", free);

    printf("FRee now: %d", free);

    lseek(fd, freeOffset, SEEK_SET);
    write(fd, newFree, strlen(newFree));

    lseek(aux, 0, SEEK_SET);
    while (1) {
        size_t size = read(aux, buffer, 10);
        write(fd, buffer, size);
        if (size != 10) break;
    }

    close(fd);
}

int estado() {

}

int main(int argc, char** argv) {

    if (argc < 3) {
        printf("Atgumentos no insuficientes");
        exit(-1);
    }

    if (!strcmp(argv[1], "crea")) {
        crea(argv + 2);
    } else if (!strcmp(argv[1], "reserva")) {
        reserva(argv + 2, (argc - 4));
    } else if (!strcmp(argv[1], "anula")) {
        anula(argv + 2, (argc - 4));
    } else if (!strcmp(argv[1], "estado")) {
        estado(argv + 2);
    } else {
        fprintf(stderr, "Orden no reconocida: %s\n", argv[1]);
        exit(-1);
    }

    exit(0);
}
