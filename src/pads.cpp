#include "pads.h"

static const int PINES[NUM_PADS] = {
    PIN_PAD0, PIN_PAD1, PIN_PAD2,
    PIN_PAD3, PIN_PAD4, PIN_PAD5
};

#define DEBOUNCE_MS 80

static unsigned long ultimoGolpe[NUM_PADS];

void padsInit(void) {
    int i;
    for (i = 0; i < NUM_PADS; i++) {
        pinMode(PINES[i], INPUT);
        ultimoGolpe[i] = 0;
    }
    Serial.println("[PADS] 6 sensores piezoelectricos listos");
}

GolpePad leerGolpe(void) {
    GolpePad resultado;
    unsigned long ahora = millis();
    int i, val;

    resultado.pad        = -1;
    resultado.intensidad =  0;

    for (i = 0; i < NUM_PADS; i++) {
        if ((ahora - ultimoGolpe[i]) < DEBOUNCE_MS) continue;
        val = analogRead(PINES[i]);
        if (val >= PIEZO_UMBRAL && val > resultado.intensidad) {
            resultado.pad        = i;
            resultado.intensidad = val;
        }
    }

    if (resultado.pad != -1)
        ultimoGolpe[resultado.pad] = ahora;

    return resultado;
}