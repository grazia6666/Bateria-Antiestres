#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include <Arduino.h>

void oledInit(void);
void oledEspera(const char* ip);
void oledContdown(int num);
void oledPadObjetivo(int pad, int barraProgreso);
void oledMostrarSecuencia(int* seq, int len, int ronda);
void oledTurnoJugador(int paso, int total, int ronda);
void oledHitCorrecto(int pad, int puntos);
void oledHitIncorrecto(int pad, int vidas);
void oledScore(int puntos, int vidas, int combo);
void oledModoLibre(int puntos);
void oledFinJuego(int puntos, const char* jugador);

#endif