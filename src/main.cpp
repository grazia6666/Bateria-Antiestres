#include <Arduino.h>
#include "wifi_manager.h"
#include "web_server.h"
#include "oled_display.h"
#include "pads.h"
#include "leds.h"
#include "audio.h"
#include "scores.h"
#include "game_modes.h"

void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("\n=== BATERIA ANTI-ESTRES ESPOL ===");

    // 1. OLED — primero para mostrar progreso de arranque
    oledInit();
    delay(1500);

    // 2. Sistema de archivos (LittleFS) y scores
    scoresInit();

    // 3. Periféricos físicos
    padsInit();
    ledsInit();
    audioInit();

    // 4. WiFi (intenta STA, si falla AP)
    wifiInit();

    // 5. Servidor web + WebSocket
    servidorInit();

    // 6. Mostrar IP en OLED
    oledEspera(ipActual().c_str());

    // 7. Animación de bienvenida en LEDs
    ledAnimacionInicio();

    Serial.println("[MAIN] Setup completo. Esperando jugador...");
}

void loop() {
    // Tick del juego — maneja toda la lógica de modos
    gameModeTick();

    // Pequeña pausa para no saturar el ADC ni el procesador
    delay(10);
}