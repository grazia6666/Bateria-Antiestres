#include "pads.h"

// Pines definidos en platformio.ini como build_flags
static const int PINES[NUM_PADS] = {
    PIN_PAD0, PIN_PAD1, PIN_PAD2,
    PIN_PAD3, PIN_PAD4, PIN_PAD5
};

// Tiempo mínimo entre golpes del mismo pad (ms) — evita rebotes
#define DEBOUNCE_MS 80

static unsigned long ultimoGolpe[NUM_PADS] = {0};

void padsInit() {
    for (int i = 0; i < NUM_PADS; i++) {
        pinMode(PINES[i], INPUT);
        // El ESP32 tiene ADC de 12 bits (0-4095) en estos pines
        // No se necesita configuración adicional para ADC de lectura analógica
    }
    Serial.println("[PADS] Inicializados 6 sensores piezoeléctricos");
}

// ── Lee todos los pads y devuelve el primero que supere el umbral ─────────
//   Si varios se golpean simultáneamente toma el de mayor intensidad.
GolpePad leerGolpe() {
    GolpePad resultado = { -1, 0 };
    unsigned long ahora = millis();

    for (int i = 0; i < NUM_PADS; i++) {
        // Debounce: ignorar si golpeó hace menos de DEBOUNCE_MS
        if ((ahora - ultimoGolpe[i]) < DEBOUNCE_MS) continue;

        int val = analogRead(PINES[i]);

        if (val >= PIEZO_UMBRAL) {
            // Si hay empate de intensidad, tomar el primero que supere umbral
            if (val > resultado.intensidad) {
                resultado.pad        = i;
                resultado.intensidad = val;
            }
        }
    }

    if (resultado.pad != -1) {
        ultimoGolpe[resultado.pad] = ahora;
    }

    return resultado;
}