#include "audio.h"
#include <DFRobotDFPlayerMini.h>
#include <HardwareSerial.h>

/*  DFPlayer 1 sonidos de pads (UART1) ─
   GPIO 16 = RX ESP32 <- TX DFPlayer
   GPIO 17 = TX ESP32 -> RX DFPlayer (1kOhm en serie)
*/
static HardwareSerial      dfSerial1(1); /*UART1*/
static DFRobotDFPlayerMini dfPads; /*objeto que controla el sonido de los pads*/
static int                 dfPadsOk = 0; /*bandera para saber si el modulo responde correcto */

/* ── DFPlayer 2: pistas de fondo (UART2) 
   GPIO 27 = RX ESP32 <- TX DFPlayer
   GPIO 14 = TX ESP32 -> RX DFPlayer (1kOhm en serie)
   Confirmado: dfPista.play(id) lee directo de la raiz de la SD
*/
static HardwareSerial      dfSerial2(2);
static DFRobotDFPlayerMini dfPista;
static int                 dfPistaOk = 0;

void audioInit(void) {
    /* DFPlayer 1 — pads*/
    dfSerial1.begin(9600, SERIAL_8N1, PIN_DFP1_RX, PIN_DFP1_TX);
    delay(1200);
    if (dfPads.begin(dfSerial1)) {
        dfPadsOk = 1;
        dfPads.volume(5);
        dfPads.EQ(DFPLAYER_EQ_NORMAL);
        Serial.println("[AUDIO] DFPlayer 1 (pads) listo");
    } else {
        Serial.println("[AUDIO] DFPlayer 1 (pads) no responde");
    }

    /* DFPlayer 2 — pistas */
    dfSerial2.begin(9600, SERIAL_8N1, PIN_DFP2_RX, PIN_DFP2_TX);
    delay(1200);
    if (dfPista.begin(dfSerial2)) {
        dfPistaOk = 1;
        dfPista.volume(5);
        dfPista.EQ(DFPLAYER_EQ_NORMAL);
        Serial.println("[AUDIO] DFPlayer 2 (pistas) listo");
    } else {
        Serial.println("[AUDIO] DFPlayer 2 (pistas) no responde");
    }
}

/* DFPlayer 1: reproducir sonido del pad */
void reproducirPad(int pad) {
    if (!dfPadsOk || pad < 0 || pad > 5) return; /*valida si funciona mi df*/
    dfPads.play(pad + 1);   /* porque el pad es arreglo en 0 y el archivo inicia en 1 */
    Serial.printf("[AUDIO] PAD %d -> archivo %d\n", pad + 1, pad + 1);
}

/* DFPlayer 2: reproducir pista de cancion  */
void reproducirCancion(int id) {
    if (!dfPistaOk) return; /*validacion si funciona el df*/
    dfPista.play(id);   /* id 1-5, confirmado: play() directo funciona */
    Serial.printf("[AUDIO] Pista %d iniciada\n", id);
}

/* DFPlayer 2 -  detener pista */
void detenerCancion(void) {
    if (!dfPistaOk) return;
    dfPista.stop();
    Serial.println("[AUDIO] Pista detenida");
}

/* Volumen  */
void setVolumenPads(int vol) {
    if (!dfPadsOk) return;
    dfPads.volume(constrain(vol, 0, 30));
}

void setVolumenPista(int vol) {
    if (!dfPistaOk) return;
    dfPista.volume(constrain(vol, 0, 30));
}