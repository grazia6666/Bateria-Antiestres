#pragma once
#include <Arduino.h>

// Índices de archivos en la SD del DFPlayer (carpeta /01/)
//   0001.mp3 → SND_HIT       golpe correcto
//   0002.mp3 → SND_MISS      golpe incorrecto
//   0003.mp3 → SND_COMBO     combo conseguido
//   0004.mp3 → SND_WIN       ronda completada
//   0005.mp3 → SND_GAMEOVER  fin de juego
//   0006.mp3 → SND_COUNTDOWN cuenta regresiva (tick)
//   0007.mp3 → SND_PAD0..5   sonidos de batería por pad (7-12)
enum Sonido {
    SND_HIT       = 1,
    SND_MISS      = 2,
    SND_COMBO     = 3,
    SND_WIN       = 4,
    SND_GAMEOVER  = 5,
    SND_COUNTDOWN = 6,
    SND_PAD0      = 7,
    SND_PAD1      = 8,
    SND_PAD2      = 9,
    SND_PAD3      = 10,
    SND_PAD4      = 11,
    SND_PAD5      = 12,
};

void audioInit();
void reproducir(Sonido s);
void reproducirPad(int pad);     // sonido específico del pad
void setVolumen(int vol);        // 0-30