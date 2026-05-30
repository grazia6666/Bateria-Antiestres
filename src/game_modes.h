#pragma once
#include <Arduino.h>

void gameModeStart(const char* modo, const char* jugador);
void gameModeStop();
void gameModeTick();    // llamar en cada iteración del loop()