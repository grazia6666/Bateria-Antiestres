#ifndef SCORES_H
#define SCORES_H
 
#include <Arduino.h>
 
void scoresInit(void);
void guardarScore(const char* nombre, const char* modo, int puntos);
void obtenerScoresJSON(char* buf, int bufLen);
 
#endif
 