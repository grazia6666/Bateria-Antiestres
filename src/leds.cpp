#include "leds.h"
#include <FastLED.h>

/*Tira de 60 LEDs dividida en 6 secciones de 10 
   PAD 0 LEDs  0 - 9
   PAD 1 LEDs 10 - 19
   PAD 2 LEDs 20 - 29
   PAD 3 LEDs 30 - 39
   PAD 4 LEDs 40 - 49
   PAD 5  LEDs 50 - 59
 */
#define LEDS_POR_SECCION  10   /* 60 LEDs  6 pads = 10 por sección */

static CRGB leds[NUM_LEDS]; /*creo arreglo de los leds*/

/* Color base de cada pad */
static const CRGB COLORES[6] = {
    CRGB(0,   229, 255),  /* pad 0 celeste  */
    CRGB(255,  31, 110),  /* pad 1 rosita   */
    CRGB(255, 214,   0),  /* pad 2 amarillo */
    CRGB(0,   230, 118),  /* pad 3 verde  */
    CRGB(213,   0, 249),  /* pad 4 morado */
    CRGB(255, 109,   0)   /* pad 5 naranja */
};

/* ── encender/apagar todos los LEDs de una sección ───── */
static void seccion_color(int pad, CRGB color) {
    int inicio, i;
    if (pad < 0 || pad >= 6) return;
    inicio = pad * LEDS_POR_SECCION;
    for (i = inicio; i < inicio + LEDS_POR_SECCION; i++) {
        leds[i] = color;
    }
}

/* Inicia toda la tira  */
void ledsInit(void) {
    FastLED.addLeds<WS2812B, PIN_LEDS, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(80);
    FastLED.clear(true);
    Serial.printf("[LEDS] FastLED listo — %d LEDs, %d por seccion\n",
                  NUM_LEDS, LEDS_POR_SECCION);
}

/* Enciende la sección del pad con su color */
void ledEncender(int pad) {
    if (pad < 0 || pad >= 6) return;
    seccion_color(pad, COLORES[pad]);
    FastLED.show();
}

/* ── Apagar sección del pad  */
void ledApagar(int pad) {
    if (pad < 0 || pad >= 6) return;
    seccion_color(pad, CRGB::Black);
    FastLED.show();
}

/* Apagar todos los LEDs */
void ledApagarTodos(void) {
    FastLED.clear(true);
}

/* Animación acierto de los juegos #1 parpadeo verde en la sección  */
void ledAnimacionCorrecto(int pad) {
    int i;
    if (pad < 0 || pad >= 6) return;
    for (i = 0; i < 2; i++) {
        seccion_color(pad, CRGB::Green);  FastLED.show(); delay(60);
        seccion_color(pad, CRGB::Black);  FastLED.show(); delay(40);
    }
    seccion_color(pad, COLORES[pad]);
    FastLED.show();
}

/* Animación error de los juegos #2 parpadeo rojo en la sección */
void ledAnimacionIncorrecto(int pad) {
    int i;
    if (pad < 0 || pad >= 6) return;
    for (i = 0; i < 3; i++) {
        seccion_color(pad, CRGB::Red);    FastLED.show(); delay(70);
        seccion_color(pad, CRGB::Black);  FastLED.show(); delay(50);
    }
}

/* Animación inicio barrido sección por sección (enciende todos los pads)*/
void ledAnimacionInicio(void) {
    int r, pad;
    for (r = 0; r < 2; r++) {
        for (pad = 0; pad < 6; pad++) {
            FastLED.clear(true);
            seccion_color(pad, COLORES[pad]);
            FastLED.show();
            delay(100);
        }
    }
    /* encender todas las secciones juntas al final */
    for (pad = 0; pad < 6; pad++) seccion_color(pad, COLORES[pad]);
    FastLED.show();
    delay(400);
    FastLED.clear(true);
}

/* Animación game over #3 pulso rojo en toda la tira  */
void ledAnimacionGameOver(void) {
    int r;
    for (r = 0; r < 4; r++) {
        fill_solid(leds, NUM_LEDS, CRGB::Red);
        FastLED.show(); delay(150);
        FastLED.clear(true); delay(100);
    }
}

/* Ajustar brillo según intensidad del golpe (ADC 0-4095) */
void ledSetBrillo(int intensidad) {
    int brillo = (int)((long)intensidad * 180 / 4095) + 40;
    if (brillo > 220) brillo = 220;
    FastLED.setBrightness(brillo);
}