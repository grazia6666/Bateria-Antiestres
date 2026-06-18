#ifndef MODO_CANCION_H
#define MODO_CANCION_H

#include "tipos_juego.h"

/* IDs de canciones */
#define CANCION_BILLIE_JEAN      0
#define CANCION_CAMISA_NEGRA     1
#define CANCION_CENTER_MASS      2
#define CANCION_OVERCOMPENSATE   3
#define CANCION_SEVEN_NATION     4
#define TOTAL_CANCIONES          5

void cancionStart(int idCancion, const char* jugador);
void cancionStop(void);
void cancionTick(void);
int  cancionActiva(void);

#endif