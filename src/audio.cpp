#include "audio.h"
#include <DFRobotDFPlayerMini.h>
#include <HardwareSerial.h>

static HardwareSerial       dfSerial(2);   // UART2
static DFRobotDFPlayerMini  dfPlayer;
static bool                 dfOk = false;

void audioInit() {
    dfSerial.begin(9600, SERIAL_8N1, PIN_DFP_RX, PIN_DFP_TX);
    delay(1000);   // DFPlayer necesita tiempo para arrancar

    if (dfPlayer.begin(dfSerial)) {
        dfOk = true;
        dfPlayer.volume(20);
        dfPlayer.EQ(DFPLAYER_EQ_NORMAL);
        Serial.println("[AUDIO] DFPlayer listo");
    } else {
        Serial.println("[AUDIO] Error: DFPlayer no responde");
    }
}

void reproducir(Sonido s) {
    if (!dfOk) return;
    dfPlayer.play(static_cast<int>(s));
}

void reproducirPad(int pad) {
    if (!dfOk || pad < 0 || pad > 5) return;
    dfPlayer.play(static_cast<int>(SND_PAD0) + pad);
}

void setVolumen(int vol) {
    if (!dfOk) return;
    dfPlayer.volume(constrain(vol, 0, 30));
}