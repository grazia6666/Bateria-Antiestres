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
   0001.mp3 = Billie Jean          (activa)
   0002.mp3 = Camisa Negra         (pendiente)
   0003.mp3 = Center of Mass       (pendiente)
   0004.mp3 = Overcompensate       (pendiente)
   0005.mp3 = Seven Nation Army    (pendiente)
   Confirmado: dfPista.play(id) reproduce directo desde raiz
──────────────────────────────────────────────────────────────*/
#define CANCION_BILLIE_JEAN    1
#define CANCION_CAMISA_NEGRA   5
#define CANCION_CENTER_MASS    3
#define CANCION_OVERCOMPENSATE 4
#define CANCION_SEVEN_NATION   2

/* ── API publica ──────────────────────────────────────────── */
void audioInit(void);
void reproducirPad(int pad);        /* DFPlayer 1 — sonido del pad */
void reproducirCancion(int id);     /* DFPlayer 2 — pista de fondo */
void detenerCancion(void);          /* DFPlayer 2 — detener pista  */
void setVolumenPads(int vol);       /* 0-30 */
void setVolumenPista(int vol);      /* 0-30 */

#endif