#pragma once
#include <Arduino.h>

void   scoresInit();
void   guardarScore(const char* nombre, const char* modo, int puntos);
String obtenerScoresJSON();