#include "audio.h"
#include <DFRobotDFPlayerMini.h>
#include <HardwareSerial.h>

/* ── DFPlayer 1: sonidos de pads ─────────────────────────────
   UART2 — pines definidos en platformio.ini
   PIN_DFP1_RX = 16
   PIN_DFP1_TX = 17
──────────────────────────────────────────────────────────────*/
static HardwareSerial      dfSerial1(1);   /* UART1 — pines 25/26 */
static DFRobotDFPlayerMini dfPads;
static int                 dfPadsOk = 0;

/* ── DFPlayer 2: pista de fondo y efectos ────────────────────
   UART2 — pines definidos en platformio.ini
   PIN_DFP2_RX = 27
   PIN_DFP2_TX = 14
──────────────────────────────────────────────────────────────*/
static HardwareSerial      dfSerial2(2);   /* UART2 — pines 27/14 */
static DFRobotDFPlayerMini dfPista;
static int                 dfPistaOk = 0;

/* ── Init ambos DFPlayer ─────────────────────────────────── */
void audioInit(void) {
    /* DFPlayer 1 — pads */
    dfSerial1.begin(9600, SERIAL_8N1, PIN_DFP1_RX, PIN_DFP1_TX);
    delay(2000);   /* aumentado a 2000ms para clones lentos */
    if (dfPads.begin(dfSerial1)) {
        dfPadsOk = 1;
        dfPads.volume(25);
        dfPads.EQ(DFPLAYER_EQ_NORMAL);
        delay(500);
        dfPads.playFolder(1, 7);   /* prueba al arrancar: carpeta 01, archivo 0007 */
        Serial.println("[AUDIO] DFPlayer 1 (pads) listo — reproduciendo prueba");
    } else {
        Serial.println("[AUDIO] DFPlayer 1 (pads) no responde");
    }

    /* DFPlayer 2 — pista */
    dfSerial2.begin(9600, SERIAL_8N1, PIN_DFP2_RX, PIN_DFP2_TX);
    delay(2000);
    if (dfPista.begin(dfSerial2)) {
        dfPistaOk = 1;
        dfPista.volume(20);
        dfPista.EQ(DFPLAYER_EQ_NORMAL);
        Serial.println("[AUDIO] DFPlayer 2 (pista) listo");
    } else {
        Serial.println("[AUDIO] DFPlayer 2 (pista) no responde");
    }
}

/* ── DFPlayer 1: sonido de pad ───────────────────────────── */
void reproducirPad(int pad) {
    if (!dfPadsOk || pad < 0 || pad > 5) return;
    dfPads.playFolder(1, SND_PAD0 + pad);  /* carpeta 01, archivo 0007-0012 */
    Serial.printf("[AUDIO] reproducirPad %d -> archivo %d\n",
                  pad + 1, SND_PAD0 + pad);
}

/* ── DFPlayer 2: efecto de sonido ────────────────────────── */
void reproducir(int sonido) {
    if (!dfPistaOk) return;
    dfPista.playFolder(1, sonido);
}

/* ── DFPlayer 2: reproducir cancion en loop ──────────────── */
void reproducirCancion(int idCancion) {
    if (!dfPistaOk) return;
    int archivo;
    switch (idCancion) {
        case 0: archivo = SND_CANCION_BILLIE;   break;
        case 1: archivo = SND_CANCION_CAMISA;   break;
        case 2: archivo = SND_CANCION_CENTER;   break;
        case 3: archivo = SND_CANCION_OVERCOMP; break;
        case 4: archivo = SND_CANCION_SEVEN;    break;
        default: return;
    }
    dfPista.playFolder(1, archivo);
    Serial.printf("[AUDIO] Reproduciendo cancion %d (archivo %d)\n",
                  idCancion, archivo);
}

/* ── DFPlayer 2: detener pista ───────────────────────────── */
void detenerCancion(void) {
    if (!dfPistaOk) return;
    dfPista.stop();
    Serial.println("[AUDIO] Pista detenida");
}

/* ── Control de volumen ──────────────────────────────────── */
void setVolumenPads(int vol) {
    if (!dfPadsOk) return;
    if (vol < 0)  vol = 0;
    if (vol > 30) vol = 30;
    dfPads.volume(vol);
}

void setVolumenPista(int vol) {
    if (!dfPistaOk) return;
    if (vol < 0)  vol = 0;
    if (vol > 30) vol = 30;
    dfPista.volume(vol);
}