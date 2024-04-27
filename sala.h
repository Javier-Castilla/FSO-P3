#ifndef SALA_H
#define SALA_H

extern int crea_sala(int capacidad);
extern int reserva_asiento(int id_persona);
extern int libera_asiento(int id_asiento);
extern int estado_asiento(int id_asiento);
extern int asientos_libres();
extern int asientos_ocupados();
extern int capacidad_sala();
extern int elimina_sala();
extern int guarda_estado_sala(const char* filename);
extern int recupera_estado_sala();
extern int guarda_estadoparcial_sala();
extern int recupera_estadoparcial_sala();
extern int getCapacity();

#endif