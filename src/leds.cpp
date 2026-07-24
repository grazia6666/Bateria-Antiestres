#include "leds.h"
#include "pads.h"
#include <FastLED.h>

/* LEDs por sección: cada pad tiene una cantidad distinta de LEDs
   (tira irregular / cableada a mano, no todas las secciones son
   iguales). Antes se usaba NUM_LEDS / NUM_PADS con división entera,
   lo que asumía secciones parejas y además dejaba LEDs sobrantes sin
   usar cuando NUM_LEDS no era múltiplo exacto de NUM_PADS (ej. con
   NUM_LEDS=52 y NUM_PADS=6 sobraban 4 LEDs al final sin asignar).

   Ahora se define explícitamente cuántos LEDs tiene cada pad, y el
   offset de inicio de cada sección se calcula sumando los anteriores.
   La suma de LEDS_POR_PAD[] DEBE ser igual a NUM_LEDS. */
static const int LEDS_POR_PAD[NUM_PADS] = { 9, 8, 9, 9, 9, 8 };

/* Offset de inicio de cada sección, calculado en ledsInit() a partir
   de LEDS_POR_PAD[] (suma acumulada). */
static int seccionInicio[NUM_PADS];

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
    int inicio, fin, i;
    if (pad < 0 || pad >= NUM_PADS) return;
    inicio = seccionInicio[pad];
    fin    = inicio + LEDS_POR_PAD[pad];
    if (fin > NUM_LEDS) fin = NUM_LEDS;   /* nunca escribir fuera del arreglo */
    for (i = inicio; i < fin; i++) {
        leds[i] = color;
    }
}

/* Inicia toda la tira  */
void ledsInit(void) {
    int pad, acumulado, sumaTotal;

    /* Calcular offset de inicio de cada seccion a partir de
       LEDS_POR_PAD[], y de paso validar que la suma cuadre con
       NUM_LEDS — si no cuadra, es un error de configuracion y es
       mejor avisar por Serial que arrancar con secciones mal
       alineadas (y volver a corromper memoria vecina). */
    acumulado = 0;
    for (pad = 0; pad < NUM_PADS; pad++) {
        seccionInicio[pad] = acumulado;
        acumulado += LEDS_POR_PAD[pad];
    }
    sumaTotal = acumulado;

    if (sumaTotal != NUM_LEDS) {
        Serial.printf("[LEDS] ERROR: suma de LEDS_POR_PAD=%d != NUM_LEDS=%d — "
                      "revisa la configuracion, puede corromper memoria\n",
                      sumaTotal, NUM_LEDS);
    }

    FastLED.addLeds<WS2812B, PIN_LEDS, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(80);
    FastLED.clear(true);
    Serial.printf("[LEDS] FastLED listo — %d LEDs, %d pads (secciones: ",
                  NUM_LEDS, NUM_PADS);
    for (pad = 0; pad < NUM_PADS; pad++) {
        Serial.printf("%d%s", LEDS_POR_PAD[pad], (pad < NUM_PADS - 1) ? "," : "");
    }
    Serial.println(")");
}

/* Enciende la sección del pad con su color */
void ledEncender(int pad) {
    if (pad < 0 || pad >= NUM_PADS) return;
    seccion_color(pad, COLORES[pad]);
    FastLED.show();
}

/* ── Apagar sección del pad  */
void ledApagar(int pad) {
    if (pad < 0 || pad >= NUM_PADS) return;
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
    if (pad < 0 || pad >= NUM_PADS) return;
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
    if (pad < 0 || pad >= NUM_PADS) return;
    for (i = 0; i < 3; i++) {
        seccion_color(pad, CRGB::Red);    FastLED.show(); delay(70);
        seccion_color(pad, CRGB::Black);  FastLED.show(); delay(50);
    }
}

/* Animación inicio barrido sección por sección (enciende todos los pads)*/
void ledAnimacionInicio(void) {
    int r, pad;
    for (r = 0; r < 2; r++) {
        for (pad = 0; pad < NUM_PADS; pad++) {
            FastLED.clear(true);
            seccion_color(pad, COLORES[pad]);
            FastLED.show();
            delay(100);
        }
    }
    /* encender todas las secciones juntas al final */
    for (pad = 0; pad < NUM_PADS; pad++) seccion_color(pad, COLORES[pad]);
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