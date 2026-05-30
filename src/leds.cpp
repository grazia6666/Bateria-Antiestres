#include "leds.h"
#include <FastLED.h>

static CRGB leds[NUM_LEDS];

// Color base de cada pad (coincide con la UI web)
static const CRGB COLORES[6] = {
    CRGB(0,   229, 255),   // pad 0 — cyan
    CRGB(255, 31,  110),   // pad 1 — pink
    CRGB(255, 214, 0  ),   // pad 2 — yellow
    CRGB(0,   230, 118),   // pad 3 — green
    CRGB(213, 0,   249),   // pad 4 — purple
    CRGB(255, 109, 0  ),   // pad 5 — orange
};

void ledsInit() {
    FastLED.addLeds<WS2812B, PIN_LEDS, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(80);
    FastLED.clear(true);
    Serial.println("[LEDS] FastLED inicializado");
}

void ledEncender(int pad) {
    if (pad < 0 || pad >= NUM_LEDS) return;
    leds[pad] = COLORES[pad];
    FastLED.show();
}

void ledApagar(int pad) {
    if (pad < 0 || pad >= NUM_LEDS) return;
    leds[pad] = CRGB::Black;
    FastLED.show();
}

void ledApagarTodos() {
    FastLED.clear(true);
}

// ── Destello verde (acierto) ──────────────────────────────────────────────
void ledAnimacionCorrecto(int pad) {
    if (pad < 0 || pad >= NUM_LEDS) return;
    CRGB original = COLORES[pad];
    for (int i = 0; i < 2; i++) {
        leds[pad] = CRGB::Green;
        FastLED.show();
        delay(60);
        leds[pad] = CRGB::Black;
        FastLED.show();
        delay(40);
    }
    leds[pad] = original;
    FastLED.show();
}

// ── Destello rojo (error) ─────────────────────────────────────────────────
void ledAnimacionIncorrecto(int pad) {
    if (pad < 0 || pad >= NUM_LEDS) return;
    for (int i = 0; i < 3; i++) {
        leds[pad] = CRGB::Red;
        FastLED.show();
        delay(70);
        leds[pad] = CRGB::Black;
        FastLED.show();
        delay(50);
    }
}

// ── Barrido arcoíris al iniciar ───────────────────────────────────────────
void ledAnimacionInicio() {
    for (int r = 0; r < 3; r++) {
        for (int i = 0; i < NUM_LEDS; i++) {
            ledApagarTodos();
            leds[i] = COLORES[i];
            FastLED.show();
            delay(80);
        }
    }
    // Encender todos juntos
    for (int i = 0; i < NUM_LEDS; i++) leds[i] = COLORES[i];
    FastLED.show();
    delay(300);
    FastLED.clear(true);
}

// ── Pulso rojo (game over) ────────────────────────────────────────────────
void ledAnimacionGameOver() {
    for (int r = 0; r < 4; r++) {
        fill_solid(leds, NUM_LEDS, CRGB::Red);
        FastLED.show();
        delay(150);
        FastLED.clear(true);
        delay(100);
    }
}

// ── Ajustar brillo según intensidad del golpe (0-4095 → 0-255) ───────────
void ledSetBrillo(int intensidad) {
    int brillo = map(constrain(intensidad, 0, 4095), 0, 4095, 40, 220);
    FastLED.setBrightness(brillo);
}