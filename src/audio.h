#ifndef AUDIO_H
#define AUDIO_H

#include <Arduino.h>

/* ── DFPlayer 1 (pads) — archivos en raiz SD ─────────────────
   0001.mp3 = PAD 1 bombo
   0002.mp3 = PAD 2 caja
   0003.mp3 = PAD 3 hi-hat
   0004.mp3 = PAD 4 tom grave
   0005.mp3 = PAD 5 tom agudo
   0006.mp3 = PAD 6 crash
   Formula: archivo = pad + 1
──────────────────────────────────────────────────────────────*/

/* ── DFPlayer 2 (pistas) — archivos en raiz SD ───────────────
   0001.mp3 = Billie Jean
   0002.mp3 = Camisa Negra
   0003.mp3 = Center of Mass
   0004.mp3 = Overcompensate
   0005.mp3 = Seven Nation Army
──────────────────────────────────────────────────────────────*/
#define CANCION_BILLIE_JEAN    1
#define CANCION_CAMISA_NEGRA   2
#define CANCION_CENTER_MASS    3
#define CANCION_OVERCOMPENSATE 4
#define CANCION_SEVEN_NATION   5

/* ── API publica ──────────────────────────────────────────── */
void audioInit(void);
void reproducirPad(int pad);        /* DFPlayer 1 — sonido del pad */
void reproducirCancion(int id);     /* DFPlayer 2 — pista de fondo */
void detenerCancion(void);          /* DFPlayer 2 — detener pista  */
void setVolumenPads(int vol);       /* 0-30 */
void setVolumenPista(int vol);      /* 0-30 */

#endif