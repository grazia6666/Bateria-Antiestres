#pragma once
#include <Arduino.h>

void oledInit();

// Pantallas del juego
void oledEspera(const char* ip);
void oledPadObjetivo(int pad, int barraProgreso);   // barraProgreso: 0-100
void oledMostrarSecuencia(int* seq, int len, int ronda);
void oledTurnoJugador(int paso, int total, int ronda);
void oledHitCorrecto(int pad, int puntos);
void oledHitIncorrecto(int pad, int vidas);
void oledScore(int puntos, int vidas, int combo);
void oledFinJuego(int puntos, const char* jugador);
void oledModoLibre(int puntos);
void oledContdown(int num);                          // 3, 2, 1, 0 = ¡YA!