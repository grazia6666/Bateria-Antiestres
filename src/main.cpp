#include <Arduino.h>
#include <LittleFS.h>
#include "wifi_manager.h"
#include "web_server.h"
#include "oled_display.h"
#include "pads.h"
#include "leds.h"
#include "audio.h"
#include "scores.h"
#include "game_modes.h"

void setup(void) {
    char ip[20]; /* Guarda 20 caracteres para la ip */

    Serial.begin(115200);
    delay(1000);   /* esperar a que el monitor serie se conecte */
    Serial.println("=== BATERIA ANTI-ESTRES ESPOL ===");

    oledInit();
    delay(1500);

    scoresInit();

    /* INFO LittleFS  */
    Serial.printf("[FS] Total : %d bytes (%.2f MB)\n",
                  LittleFS.totalBytes(),
                  LittleFS.totalBytes() / 1048576.0f);
    Serial.printf("[FS] Usado : %d bytes\n", LittleFS.usedBytes());
    Serial.printf("[FS] Libre : %d bytes (%.2f MB)\n",
                  LittleFS.totalBytes() - LittleFS.usedBytes(),
                  (LittleFS.totalBytes() - LittleFS.usedBytes()) / 1048576.0f);

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
    gameModeTick(); /*CEREBRO*/
    delay(10);
}