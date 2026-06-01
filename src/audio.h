#ifndef AUDIO_H
#define AUDIO_H
 
#include <Arduino.h>
 
/* Indices de archivos en la SD del DFPlayer (carpeta /01/) */
#define SND_HIT        1
#define SND_MISS       2
#define SND_COMBO      3
#define SND_WIN        4
#define SND_GAMEOVER   5
#define SND_COUNTDOWN  6
#define SND_PAD0       7   /* PAD0..5 = 7..12 */
 
void audioInit(void);
void reproducir(int sonido);
void reproducirPad(int pad);
void setVolumen(int vol);
 
#endif