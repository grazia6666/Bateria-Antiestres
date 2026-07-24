#include "pads.h"

static const int PINES[NUM_PADS] = {
    PIN_PAD0, PIN_PAD1, PIN_PAD2,
    PIN_PAD3, PIN_PAD4, PIN_PAD5
};

#define DEBOUNCE_MS 80

/*Guarda el instante en que cada sensor fue activado por última vez.*/
static unsigned long ultimoGolpe[NUM_PADS];

/*verificacion si valen o no*/
void padsInit(void) {
    int i;
    for (i = 0; i < NUM_PADS; i++) {
        /* INPUT_PULLDOWN solo tiene efecto real en GPIO32/33.
           GPIO34/35/36/39 son "input-only" y NO tienen pull-down
           interno en el ESP32 -- si esos pads dan falsos positivos,
           hace falta una resistencia de pull-down EXTERNA (1M-4.7M)
           entre la señal del piezo y GND. */
        pinMode(PINES[i], INPUT_PULLDOWN);
        ultimoGolpe[i] = 0;
    }
    Serial.println("[PADS] 6 sensores piezoelectricos listos");
}

/*revisa los sensores y detecta cual fue golpeado*/
GolpePad leerGolpe(void) {
    GolpePad resultado;
    unsigned long ahora = millis(); /*devuelve el tiempo desde que la esp32 se encendio */
    int i, val;

    resultado.pad        = -1;
    resultado.intensidad =  0;

    for (i = 0; i < NUM_PADS; i++) {
        if ((ahora - ultimoGolpe[i]) < DEBOUNCE_MS) continue; /*si es menor a 80 no lo tomo en cuenta*/

        val = analogRead(PINES[i]);

        /* Filtro anti-ruido: un golpe real mantiene el voltaje elevado
           por varios milisegundos; un pico de ruido/EMI suele durar
           microsegundos. Si la primera lectura supera el umbral,
           se confirma con una segunda lectura poco despues -- si el
           valor ya cayo, se descarta como ruido. */
        if (val >= PIEZO_UMBRAL) {
            int valConfirm;
            delayMicroseconds(400);
            valConfirm = analogRead(PINES[i]);
            if (valConfirm < PIEZO_UMBRAL) {
                continue; /* fue un pico aislado, no un golpe real */
            }
            val = (val + valConfirm) / 2;
        }

        /*  DEBUG temporal para saber cual estoy golpeando jajs  */
        if (val > 50)
            Serial.printf("[PAD DEBUG] GPIO%d = %d\n", PINES[i], val);
        /*supera el valor min y si dos sensores se activan al mismo tiempo el que se golpeo mas fuerte se lee*/
        if (val >= PIEZO_UMBRAL && val > resultado.intensidad) {
            resultado.pad        = i;
            resultado.intensidad = val;
        }
    }
    /*Actulizacion del ultimo golpe*/
    if (resultado.pad != -1)
        ultimoGolpe[resultado.pad] = ahora;

    return resultado;
}