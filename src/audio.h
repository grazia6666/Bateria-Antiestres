#ifndef AUDIO_H
#define AUDIO_H

#include <Arduino.h>

/* ── Indices de archivos en la SD (carpeta /01/) ─────────────
   DFPlayer 1 (pads):   0007-0012.mp3  sonidos de bateria
   DFPlayer 2 (pista):  0001-0006.mp3  efectos + canciones
──────────────────────────────────────────────────────────────*/

/* Sonidos de efectos — DFPlayer 2 */
#define SND_HIT        1
#define SND_MISS       2
#define SND_COMBO      3
#define SND_WIN        4
#define SND_GAMEOVER   5
#define SND_COUNTDOWN  6

/* Sonidos de pads — DFPlayer 1 */
#define SND_PAD0       7   /* PAD0..5 = 7..12 */

/* IDs de canciones en DFPlayer 2 (archivos 0013.mp3 en adelante) */
#define SND_CANCION_BILLIE      13
#define SND_CANCION_CAMISA      14
#define SND_CANCION_CENTER      15
#define SND_CANCION_OVERCOMP    16
#define SND_CANCION_SEVEN       17

/* ── API publica ──────────────────────────────────────────── */
void audioInit(void);

/* DFPlayer 1 — sonidos de pads */
void reproducirPad(int pad);

/* DFPlayer 2 — efectos y pistas */
void reproducir(int sonido);
void reproducirCancion(int idCancion);
void detenerCancion(void);
void setVolumenPads(int vol);    /* 0-30 */
void setVolumenPista(int vol);   /* 0-30 */

#endif