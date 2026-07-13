#include "oled_display.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <stdio.h>

#define OLED_W     128
#define OLED_H      64
#define OLED_RESET  -1
#define OLED_ADDR  0x3C

static Adafruit_SSD1306 display(OLED_W, OLED_H, &Wire, OLED_RESET);

static void hline(int y) {
    display.drawLine(0, y, OLED_W - 1, y, SSD1306_WHITE);
}

void oledInit(void) {
    Wire.begin(OLED_SDA, OLED_SCL);
    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
        Serial.println("[OLED] Error SSD1306");
        return;
    }
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.cp437(true);
    display.setTextSize(1);
    display.setCursor(10, 10); display.println("BATERIA ANTI-ESTRES");
    display.setCursor(28, 26); display.println("ESPOL 2026F");
    display.setCursor(8,  44); display.println("Iniciando...");
    display.display();
    Serial.println("[OLED] SSD1306 listo");
}

void oledEspera(const char* ip) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(14, 2);  display.println("BATERIA ANTI-ESTRES");
    hline(12);
    display.setCursor(0, 18); display.println("WiFi: BateriaESPOL");
    display.setCursor(0, 30); display.print("IP: 192.168.4.1 "); display.println(ip);
    hline(44);
    display.setCursor(4, 50); display.println("Esperando jugador...");
    display.display();
}

void oledContdown(int num) {
    display.clearDisplay();
    hline(12); hline(51);
    display.setTextSize(1);
    display.setCursor(30, 2); display.println("PREPARATE!");
    display.setTextSize(4);
    if (num > 0) {
        display.setCursor(54, 18);
        display.print(num);
    } else {
        display.setTextSize(2);
        display.setCursor(28, 24);
        display.print("YA!!!");
    }
    display.setTextSize(1);
    display.display();
}

void oledPadObjetivo(int pad, int barraProgreso) {
    char buf[12];
    int16_t x1, y1;
    uint16_t w, h;
    int barW;

    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(22, 0); display.println("MODO REFLEJOS");
    hline(10);

    display.setTextSize(3);
    snprintf(buf, sizeof(buf), "PAD %d", pad + 1);
    display.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h);
    display.setCursor((OLED_W - w) / 2, 16);
    display.print(buf);

    display.setTextSize(1);
    hline(44);
    barW = (barraProgreso * (OLED_W - 4)) / 100;
    display.fillRect(2, 47, barW, 8, SSD1306_WHITE);
    display.drawRect(2, 47, OLED_W - 4, 8, SSD1306_WHITE);
    display.setCursor(2, 57); display.print("TIEMPO");
    display.display();
}

void oledMostrarSecuencia(int* seq, int len, int ronda) {
    /* No mostramos la secuencia numerica — solo decimos que sigan los LEDs */
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(18, 0); display.println("MODO MEMORIA");
    hline(10);
    display.setCursor(0, 18); display.println("  Sigue los LEDs");
    display.setCursor(0, 30); display.println("  que se enciendan");
    display.setCursor(0, 42); display.println("  y toca ese pad!");
    hline(54);
    display.setCursor(28, 57); display.print("Ronda "); display.print(ronda);
    display.display();
}

void oledTurnoJugador(int paso, int total, int ronda) {
    /* lo cambie por el oledMemoriaScore */
    (void)paso; (void)total; (void)ronda;
}

/* ── Memoria jugando muestra puntaje + instruccion ─────────── */
void oledMemoriaScore(int score, int ronda) {
    char buf[12];
    int16_t x1, y1;
    uint16_t w, h;

    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(18, 0); display.println("MODO MEMORIA");
    hline(10);

    display.setCursor(0, 14); display.println("Sigue los LEDs!");

    /* Puntaje grande centrado */
    display.setTextSize(2);
    snprintf(buf, sizeof(buf), "%d", score);
    display.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h);
    display.setCursor((OLED_W - w) / 2, 26);
    display.print(buf);

    display.setTextSize(1);
    display.setCursor(44, 46); display.print("pts");

    hline(54);
    display.setCursor(28, 57); display.print("Ronda "); display.print(ronda);
    display.display();
}

void oledHitCorrecto(int pad, int puntos) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(20, 6);  display.println("CORRECTO!");
    hline(28);
    display.setTextSize(1);
    display.setCursor(0, 34);
    display.print("PAD "); display.print(pad + 1);
    display.print("  +");  display.print(puntos);
    display.println(" pts");
    display.display();
}

void oledHitIncorrecto(int pad, int vidas) {
    int i;
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(22, 6); display.println("ERROR!");
    hline(28);
    display.setTextSize(1);
    display.setCursor(0, 34);
    display.print("Pad "); display.print(pad + 1);
    display.println(" incorrecto");
    display.setCursor(0, 46);
    display.print("Vidas: ");
    for (i = 0; i < 3; i++)
        display.print(i < vidas ? "<3 " : "-- ");
    display.display();
}

void oledScore(int puntos, int vidas, int combo) {
    char buf[10];
    int16_t x1, y1;
    uint16_t w, h;
    int i;

    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(40, 0); display.println("SCORE");
    hline(10);

    display.setTextSize(3);
    snprintf(buf, sizeof(buf), "%d", puntos);
    display.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h);
    display.setCursor((OLED_W - w) / 2, 14);
    display.print(buf);

    display.setTextSize(1);
    hline(44);
    display.setCursor(0, 48);
    display.print("Vidas: ");
    for (i = 0; i < 3; i++) display.print(i < vidas ? "<3 " : "-- ");
    display.setCursor(0, 57);
    display.print("Combo: x"); display.print(combo);
    display.display();
}

void oledModoLibre(int puntos) {
    char buf[10];
    int16_t x1, y1;
    uint16_t w, h;

    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(22, 0); display.println("MODO LIBRE");
    hline(10);
    display.setCursor(8, 16); display.println("Toca a tu ritmo!");

    display.setTextSize(2);
    snprintf(buf, sizeof(buf), "%d", puntos);
    display.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h);
    display.setCursor((OLED_W - w) / 2, 32);
    display.print(buf);

    display.setTextSize(1);
    display.setCursor(44, 55); display.print("pts");
    display.display();
}

/*  Modo cancion: puntaje en tiempo real─ */
void oledModoCancion(int puntos, int aciertos, int fallos, int combo) {
    char buf[12];
    int16_t x1, y1;
    uint16_t w, h;

    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(18, 0); display.println("MODO CANCION");
    hline(10);

    /* Puntaje grande centrado */
    display.setTextSize(2);
    snprintf(buf, sizeof(buf), "%d", puntos);
    display.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h);
    display.setCursor((OLED_W - w) / 2, 13);
    display.print(buf);

    display.setTextSize(1);
    display.setCursor(48, 31); display.print("pts");

    hline(38);

    display.setCursor(0,  42); display.print("OK:"); display.print(aciertos);
    display.setCursor(42, 42); display.print("X:"); display.print(fallos);
    display.setCursor(84, 42); display.print("x"); display.print(combo);

    hline(53);
    display.setCursor(10, 56); display.print("Sigue el ritmo!");
    display.display();
}

/* Fin de partida — todos los modos  */
void oledFinJuego(int puntos, const char* jugador) {
    char buf[10];
    int16_t x1, y1;
    uint16_t w, h;

    display.clearDisplay();

    /* Puntaje grande centrado */
    display.setTextSize(3);
    snprintf(buf, sizeof(buf), "%d", puntos);
    display.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h);
    display.setCursor((OLED_W - w) / 2, 2);
    display.print(buf);

    display.setTextSize(1);
    display.setCursor(44, 28); display.println("puntos");

    hline(36);

    display.setCursor(0, 40); display.print(jugador);

    hline(50);

    /* Mensaje principal */
    display.setTextSize(1);
    display.setCursor(0, 54); display.println("SELEC. OTRO MODO!");
    display.display();
}