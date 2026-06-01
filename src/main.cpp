#include <Arduino.h>
#include "wifi_manager.h"
#include "web_server.h"
#include "oled_display.h"
#include "pads.h"
#include "leds.h"
#include "audio.h"
#include "scores.h"
#include "game_modes.h"

void setup(void) {
    char ip[20];

    Serial.begin(115200);
    delay(500);
    Serial.println("=== BATERIA ANTI-ESTRES ESPOL ===");

    oledInit();
    delay(1500);

    scoresInit();
    padsInit();
    ledsInit();
    audioInit();

    wifiInit();

    servidorInit();

    ipActual(ip, sizeof(ip));
    oledEspera(ip);

    ledAnimacionInicio();

    Serial.println("[MAIN] Setup completo. Esperando jugador...");
}

void loop(void) {
    gameModeTick();
    delay(10);
}