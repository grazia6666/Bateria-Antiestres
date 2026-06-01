#include "audio.h"
#include <DFRobotDFPlayerMini.h>
#include <HardwareSerial.h>

static HardwareSerial      dfSerial(2);
static DFRobotDFPlayerMini dfPlayer;
static int                 dfOk = 0;

void audioInit(void) {
    dfSerial.begin(9600, SERIAL_8N1, PIN_DFP_RX, PIN_DFP_TX);
    delay(1000);
    if (dfPlayer.begin(dfSerial)) {
        dfOk = 1;
        dfPlayer.volume(20);
        dfPlayer.EQ(DFPLAYER_EQ_NORMAL);
        Serial.println("[AUDIO] DFPlayer listo");
    } else {
        Serial.println("[AUDIO] DFPlayer no responde");
    }
}

void reproducir(int sonido) {
    if (!dfOk) return;
    dfPlayer.play(sonido);
}

void reproducirPad(int pad) {
    if (!dfOk || pad < 0 || pad > 5) return;
    dfPlayer.play(SND_PAD0 + pad);
}

void setVolumen(int vol) {
    if (!dfOk) return;
    if (vol < 0)  vol = 0;
    if (vol > 30) vol = 30;
    dfPlayer.volume(vol);
}