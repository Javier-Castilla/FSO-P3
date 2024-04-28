// test_sala.c
// ===============
// Batería de pruebas de la biblioteca "sala.h/sala.c"
//

#include <assert.h>
#include <stdio.h>
#include "sala.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define DebeSerCierto(x)	assert(x)
#define DebeSerFalso(x)		assert(!(x))

/// FUNCIONES PARA PODER HACER TESTS
static int compare(char* route1, char* route2) {
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

void crea_ficheros(){
	crea_sala(100);
    reserva_asiento(1);
    reserva_asiento(5);
    reserva_asiento(11);
    reserva_asiento(22);
    reserva_asiento(33);
    reserva_asiento(44);
    guarda_estado_sala("salon.txt");
    elimina_sala();
    
    crea_sala(100);
    guarda_estado_sala("test_sala_parcial.txt");
    elimina_sala();
}

/// INICIO DE LOS TESTS


void INICIO_TEST (const char* titulo_test)
{
  printf("********** batería de pruebas para %s: ", titulo_test); 
 	// fflush fuerza que se imprima el mensaje anterior
	// sin necesidad de utilizar un salto de línea
	fflush(stdout);
}

void FIN_TEST (const char* titulo_test)
{
  printf ("********** hecho\n");
}

void test_guarda_estado_sala() {
    INICIO_TEST("guarda_estado_sala");

    crea_sala(10);
    reserva_asiento(1);
    reserva_asiento(2);
    DebeSerCierto(guarda_estado_sala("test_sala.txt") == 0);
	elimina_sala();

    FIN_TEST("guarda_estado_sala");
}

void test_recupera_estado_sala() {
    INICIO_TEST("recupera_estado_sala");
    
    DebeSerCierto(recupera_estado_sala("test_sala.txt") == 0);
    DebeSerCierto(asientos_ocupados() == 2);
    elimina_sala();

    FIN_TEST("recupera_estado_sala");
}

void test_guarda_estadoparcial_sala() {
    INICIO_TEST("guarda_estadoparcial_sala");

	crea_sala(150);
    reserva_asiento(14);
    reserva_asiento(24);
    reserva_asiento(34);
    reserva_asiento(44);
    reserva_asiento(54);
    int asientos[3] = {0, 3, 4};
    DebeSerCierto(guarda_estadoparcial_sala("test_sala_parcial.txt", 3, asientos) == 0);
    elimina_sala();

    FIN_TEST("guarda_estadoparcial_sala");
}

void test_recupera_estadoparcial_sala() {
    INICIO_TEST("recupera_estadoparcial_sala");

	crea_sala(70);
    reserva_asiento(11);
    reserva_asiento(24);
    int asientos[2] = {3, 4};
    DebeSerCierto(recupera_estadoparcial_sala("test_sala_parcial.txt", 2, asientos) == 0);
	DebeSerCierto(estado_asiento(3) == 44);
    DebeSerCierto(estado_asiento(4) == 54);    
    DebeSerCierto(asientos_ocupados() == 4);
	elimina_sala();

    FIN_TEST("recupera_estadoparcial_sala");
}

void test_compara_salas() {
    INICIO_TEST("compara_sala");
	crea_sala(15);
	DebeSerCierto(recupera_estado_sala("test_sala.txt") == 0);
	DebeSerCierto(guarda_estado_sala("test2_sala.txt") == 0);
	elimina_sala();
	DebeSerCierto(compare("test_sala.txt", "test2_sala.txt") == 0);
	FIN_TEST("compara_sala");
}


void test_compara_salas_parcial() {
    INICIO_TEST("compara_parcial_sala");
	crea_sala(100);
    int asientos[2] = {3, 4};
    DebeSerCierto(recupera_estadoparcial_sala("salon.txt", 2, asientos) == 0);
	DebeSerCierto(guarda_estado_sala("test4_sala.txt") == 0);
	elimina_sala();
	
	crea_sala(180);
	int asientos2[3] = {0, 1, 4};
	DebeSerCierto(recupera_estadoparcial_sala("salon.txt", 3, asientos2) == 0);
	DebeSerCierto(guarda_estado_sala("test5_sala.txt") == 0);
	elimina_sala();

	DebeSerCierto(compare("test4_sala.txt", "test5_sala.txt") == 1);
	
	FIN_TEST("compara_parcial_sala");
}

void test_misala_crea() {
    	INICIO_TEST("misala_crea");
    	if (fork() == 0) {
		execlp("./misala", "misala", "crea", "-fo", "salon", "-c", "25", NULL);
	} else {
		wait(NULL);
	}
    	
	DebeSerCierto(recupera_estado_sala("salon") == 0);
	DebeSerCierto(getCapacity() == 25);
	DebeSerCierto(asientos_libres() == 25);
	DebeSerCierto(asientos_ocupados() == 0);
	FIN_TEST("misala_crea");
}

void test_misala_reserva() {
    	INICIO_TEST("misala_reserva");
    	if (fork() == 0) {
		execlp("./misala", "misala", "reserva", "-f", "salon", "2", "10", NULL);
	} else {
		wait(NULL);
	}
    	
	DebeSerCierto(recupera_estado_sala("salon") == 0);
	DebeSerCierto(getCapacity() == 25);
	DebeSerCierto(asientos_libres() == 23);
	DebeSerCierto(asientos_ocupados() == 2);
	DebeSerCierto(estado_asiento(0) == 2);
	DebeSerCierto(estado_asiento(1) == 10);
	FIN_TEST("misala_reserva");
}

void test_misala_anula_asientos() {
    	INICIO_TEST("misala_anula_asientos");
    	if (fork() == 0) {
		execlp("./misala", "misala", "anula", "-f", "salon", "-asientos", "0", NULL);
	} else {
		wait(NULL);
	}
    	
	DebeSerCierto(recupera_estado_sala("salon") == 0);
	DebeSerCierto(getCapacity() == 25);
	DebeSerCierto(asientos_libres() == 24);
	DebeSerCierto(asientos_ocupados() == 1);
	DebeSerCierto(estado_asiento(0) == 0);
	FIN_TEST("misala_anula_asientos");
}

void test_misala_anula_personas() {
    	INICIO_TEST("misala_anula_personas");
    	if (fork() == 0) {
		execlp("./misala", "misala", "anula", "-f", "salon", "-personas", "10", NULL);
	} else {
		wait(NULL);
	}
    	
	DebeSerCierto(recupera_estado_sala("salon") == 0);
	DebeSerCierto(getCapacity() == 25);
	DebeSerCierto(asientos_libres() == 25);
	DebeSerCierto(asientos_ocupados() == 0);
	DebeSerCierto(estado_asiento(1) == 0);
	FIN_TEST("misala_anula_personas");
}

void ejecuta_tests ()
{
	if (fork() == 0) {
		execlp("gcc", "gcc", "misala.c", "sala.c", "-o", "misala", NULL);
	} else {
		wait(NULL);
	}

	crea_ficheros();
    test_guarda_estado_sala();
    test_recupera_estado_sala();
    test_guarda_estadoparcial_sala();
    test_recupera_estadoparcial_sala();
    test_compara_salas();
    test_compara_salas_parcial();
    test_misala_crea();
    test_misala_reserva();
    test_misala_anula_asientos();
    test_misala_anula_personas();
}

int main()
{
	puts("Iniciando tests...");
	
	ejecuta_tests();
	
	if (fork() == 0) {
    		execlp("rm", "rm", "salon.txt", "test_sala_parcial.txt", "test_sala.txt", "test2_sala.txt", "test4_sala.txt", "test5_sala.txt", NULL);
    	} else {
    		wait(NULL);
    	}
	
	puts("Batería de test completa.");
}
