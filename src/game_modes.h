#ifndef GAME_MODES_H
#define GAME_MODES_H

#include <Arduino.h>

void gameModeStart(const char* modo, const char* jugador);
void gameModeStop(void);
void gameModeTick(void);

#endif