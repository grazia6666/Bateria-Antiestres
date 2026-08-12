#include "pads.h"

static const int PINES[NUM_PADS] = {
    PIN_PAD0, PIN_PAD1, PIN_PAD2,
    PIN_PAD3, PIN_PAD4, PIN_PAD5
};

/* Pad deshabilitado por falso contacto en hardware (indice 5 = "pad 6"
   contando desde 1). Se deja NUM_PADS=6 para no tener que renumerar
   LEDs/audio/pines -- solo se ignora en deteccion y en la seleccion
   aleatoria de objetivos (ver game_modes.cpp). Poner -1 para
   rehabilitarlo cuando se arregle el hardware. */
#define PAD_DESHABILITADO 5

#define DEBOUNCE_MS 80

/* Umbral de deteccion por pad. Por defecto todos usan PIEZO_UMBRAL
   (definido en platformio.ini), pero se puede subir individualmente
   si algun pad tiene mas ruido de fondo -- por ejemplo GPIO32/33
   tienen un piso de ruido mas alto (~440-467) por el cristal de
   32.768kHz del RTC del ESP32. Edita el numero de cada pad aqui: */
static const int UMBRALES[NUM_PADS] = {
    450,            /* pad 0 (GPIO34) -- subido: se detectaron golpes falsos en Modo Cancion, ajustar segun [PAD DEBUG] */
    PIEZO_UMBRAL,   /* pad 1 (GPIO35) */
    800,            /* pad 2 (GPIO32) -- subido por el piso de ruido del RTC */
    600,   /* pad 3 (GPIO33) */
    1350,   /* pad 4 (GPIO36) */
    PIEZO_UMBRAL /* pad 5 (GPIO39) -- deshabilitado, no importa el valor */
};

/* Devuelve el umbral configurado para un pad especifico. */
int umbralPad(int pad) {
    if (pad < 0 || pad >= NUM_PADS) return PIEZO_UMBRAL;
    return UMBRALES[pad];
}

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
        if (i == PAD_DESHABILITADO) continue; /* pad 6 fuera de servicio */
        if ((ahora - ultimoGolpe[i]) < DEBOUNCE_MS) continue; /*si es menor a 80 no lo tomo en cuenta*/

        val = analogRead(PINES[i]);

        /* Filtro anti-ruido: un golpe real mantiene el voltaje elevado
           por varios milisegundos; un pico de ruido/EMI suele durar
           microsegundos. Si la primera lectura supera el umbral,
           se confirma con una segunda lectura poco despues -- si el
           valor ya cayo, se descarta como ruido. */
        if (val >= UMBRALES[i]) {
            int valConfirm;
            delayMicroseconds(400);
            valConfirm = analogRead(PINES[i]);
            if (valConfirm < UMBRALES[i]) {
                continue; /* fue un pico aislado, no un golpe real */
            }
            val = (val + valConfirm) / 2;
        }

        /*  DEBUG temporal para saber cual estoy golpeando jajs  */
        if (val > 50)
            Serial.printf("[PAD DEBUG] GPIO%d = %d\n", PINES[i], val);
        /*supera el valor min y si dos sensores se activan al mismo tiempo el que se golpeo mas fuerte se lee*/
        if (val >= UMBRALES[i] && val > resultado.intensidad) {
            resultado.pad        = i;
            resultado.intensidad = val;
        }
    }
    /*Actulizacion del ultimo golpe*/
    if (resultado.pad != -1)
        ultimoGolpe[resultado.pad] = ahora;

    return resultado;
}