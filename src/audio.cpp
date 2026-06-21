#include "audio.h"
#include <DFRobotDFPlayerMini.h>
#include <HardwareSerial.h>

/* ── DEBUG seguro ───────────────────────────── */
#define DBG(x) do { Serial.println(x); Serial.flush(); } while(0)

/* ── DFPlayer 1 (pads) ─────────────────────── */
static HardwareSerial dfSerial1(1);
static DFRobotDFPlayerMini dfPads;
static int dfPadsOk = 0;

/* ── DFPlayer 2 (pistas) ───────────────────── */
static HardwareSerial dfSerial2(2);
static DFRobotDFPlayerMini dfPista;
static int dfPistaOk = 0;

/* ───────────────────────────────────────────── */
void audioInit(void) {

    Serial.begin(115200);
    delay(1000);

    DBG("\n\n[BOOT AUDIO] ===== INICIO AUDIO =====");

    /* ── DFPLAYER 1 ── */
    DBG("[AUDIO] Iniciando DFPlayer 1...");

    dfSerial1.begin(9600, SERIAL_8N1, PIN_DFP1_RX, PIN_DFP1_TX);
    delay(1200);

    if (dfPads.begin(dfSerial1)) {
        dfPadsOk = 1;
        dfPads.volume(25);
        dfPads.EQ(DFPLAYER_EQ_NORMAL);
        DBG("[AUDIO] DFPlayer 1 OK");
    } else {
        DBG("[AUDIO] DFPlayer 1 FAIL");
    }

    /* ── DFPLAYER 2 ── */
    DBG("[AUDIO] Iniciando DFPlayer 2...");

    dfSerial2.begin(9600, SERIAL_8N1, PIN_DFP2_RX, PIN_DFP2_TX);
    delay(1200);

    DBG("[AUDIO] Intentando DFPlayer 2 begin...");

    bool ok = dfPista.begin(dfSerial2);

    if (ok) {
        dfPistaOk = 1;
        dfPista.volume(20);
        dfPista.EQ(DFPLAYER_EQ_NORMAL);
        DBG("[AUDIO] DFPlayer 2 OK");
    } else {
        DBG("[AUDIO] DFPlayer 2 FAIL");
    }

    DBG("[AUDIO] ===== AUDIO INIT TERMINADO =====");

    /* ── DEBUG TEST ── */
    audioDebugDF2();
}

/* ───────────────────────────────────────────── */
void audioDebugDF2(void) {

    DBG("[DF2 DEBUG] INICIO TEST");

    if (!dfPistaOk) {
        DBG("[DF2 DEBUG] DF2 NO INICIALIZADO");
        return;
    }

    Serial.printf("[DF2 DEBUG] Archivos raiz: %d\n", dfPista.readFileCounts());
    Serial.printf("[DF2 DEBUG] Carpetas    : %d\n", dfPista.readFolderCounts());

    DBG("[DF2 DEBUG] Test play(1)");
    dfPista.play(1);
    delay(3000);
    dfPista.stop();

    DBG("[DF2 DEBUG] Test play(2)");
    dfPista.play(2);
    delay(3000);
    dfPista.stop();

    DBG("[DF2 DEBUG] FIN TEST");
}

/* ───────────────────────────────────────────── */
void reproducirPad(int pad) {

    if (!dfPadsOk) return;

    dfPads.play(pad + 1);

    Serial.printf("[AUDIO] PAD %d -> %d\n", pad + 1, pad + 1);
}

/* ───────────────────────────────────────────── */
void reproducirCancion(int id) {

    if (!dfPistaOk) return;

    DBG("[AUDIO] Reproduciendo canción");

    /* ✔ MODO MÁS SEGURO */
    dfPista.play(id);

    Serial.printf("[AUDIO] play(%d)\n", id);
}

/* ───────────────────────────────────────────── */
void detenerCancion(void) {

    if (!dfPistaOk) return;

    dfPista.stop();

    DBG("[AUDIO] Canción detenida");
}

/* ───────────────────────────────────────────── */
void setVolumenPads(int vol) {

    if (!dfPadsOk) return;

    dfPads.volume(constrain(vol, 0, 30));
}

/* ───────────────────────────────────────────── */
void setVolumenPista(int vol) {

    if (!dfPistaOk) return;

    dfPista.volume(constrain(vol, 0, 30));
}