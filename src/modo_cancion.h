#ifndef MODO_CANCION_H
#define MODO_CANCION_H

#include "tipos_juego.h"
#include "audio.h"   /* IDs CANCION_* definidos aqui */

#define TOTAL_CANCIONES  5

void cancionStart(int idCancion, const char* jugador);
void cancionStop(void);
void cancionTick(void);
int  cancionActiva(void);

#endif