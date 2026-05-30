#include "oled_display.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_W      128
#define OLED_H       64
#define OLED_RESET   -1
#define OLED_ADDR  0x3C

static Adafruit_SSD1306 display(OLED_W, OLED_H, &Wire, OLED_RESET);

// ── Init ──────────────────────────────────────────────────────────────────
void oledInit() {
    Wire.begin(OLED_SDA, OLED_SCL);
    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
        Serial.println("[OLED] Error al iniciar SSD1306");
        return;
    }
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.cp437(true);

    // Splash
    display.setTextSize(1);
    display.setCursor(10, 10);
    display.println("BATERIA ANTI-ESTRES");
    display.setCursor(28, 26);
    display.println("ESPOL 2025");
    display.setCursor(8, 44);
    display.println("Iniciando sistema...");
    display.display();
}

// ── Helper: línea horizontal decorativa ──────────────────────────────────
static void hline(int y) {
    display.drawLine(0, y, OLED_W - 1, y, SSD1306_WHITE);
}

// ── Pantalla de espera / AP ───────────────────────────────────────────────
void oledEspera(const char* ip) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(14, 2);
    display.println("BATERIA ANTI-ESTRES");
    hline(12);
    display.setCursor(0, 18);
    display.println("WiFi: BateriaESPOL");
    display.setCursor(0, 30);
    display.print("IP: "); display.println(ip);
    hline(44);
    display.setCursor(4, 50);
    display.println("Esperando jugador...");
    display.display();
}

// ── Cuenta regresiva ─────────────────────────────────────────────────────
void oledContdown(int num) {
    display.clearDisplay();
    hline(12);
    hline(51);
    display.setCursor(30, 16);
    display.setTextSize(1);
    display.println("PREPARATE!");
    display.setTextSize(4);
    if (num > 0) {
        display.setCursor(54, 20);
        display.print(num);
    } else {
        display.setTextSize(2);
        display.setCursor(28, 26);
        display.print("YA!!!");
    }
    display.setTextSize(1);
    display.display();
}

// ── Modo Reflejos: pad objetivo + barra de tiempo ────────────────────────
//   barraProgreso: 100 = llena, 0 = vacía (tiempo agotado)
void oledPadObjetivo(int pad, int barraProgreso) {
    display.clearDisplay();

    // Título
    display.setTextSize(1);
    display.setCursor(22, 0);
    display.println("MODO REFLEJOS");
    hline(10);

    // Pad objetivo — grande y centrado
    display.setTextSize(3);
    char buf[12];
    snprintf(buf, sizeof(buf), "PAD %d", pad + 1);
    int16_t x1, y1; uint16_t w, h;
    display.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h);
    display.setCursor((OLED_W - w) / 2, 16);
    display.print(buf);

    // Barra de tiempo
    display.setTextSize(1);
    hline(44);
    int barW = map(barraProgreso, 0, 100, 0, OLED_W - 4);
    display.fillRect(2, 47, barW, 8, SSD1306_WHITE);
    display.drawRect(2, 47, OLED_W - 4, 8, SSD1306_WHITE);

    // Etiqueta tiempo
    display.setCursor(2, 57);
    display.print("TIEMPO");

    display.display();
}

// ── Modo Memoria: mostrar secuencia al jugador ────────────────────────────
void oledMostrarSecuencia(int* seq, int len, int ronda) {
    display.clearDisplay();

    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("MEMORIZA  R:");
    display.println(ronda);
    hline(10);

    // Mostrar hasta 8 números de la secuencia en 2 filas
    display.setTextSize(2);
    int mostrar = min(len, 8);
    int porFila = min(mostrar, 4);

    for (int i = 0; i < porFila; i++) {
        display.setCursor(4 + i * 30, 14);
        display.print(seq[i] + 1);
    }
    if (mostrar > 4) {
        for (int i = 4; i < mostrar; i++) {
            display.setCursor(4 + (i - 4) * 30, 36);
            display.print(seq[i] + 1);
        }
    }

    display.setTextSize(1);
    hline(56);
    display.setCursor(2, 58);
    display.print("Observa y recuerda");
    display.display();
}

// ── Modo Memoria: turno del jugador ──────────────────────────────────────
void oledTurnoJugador(int paso, int total, int ronda) {
    display.clearDisplay();

    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("TU TURNO  R:");
    display.println(ronda);
    hline(10);

    // Progreso: cuadraditos
    display.setCursor(0, 14);
    display.print("Paso:");
    for (int i = 0; i < total && i < 12; i++) {
        if (i < paso)
            display.fillRect(2 + i * 10, 24, 8, 8, SSD1306_WHITE);   // completado
        else
            display.drawRect(2 + i * 10, 24, 8, 8, SSD1306_WHITE);   // pendiente
    }

    // Paso actual grande
    display.setTextSize(3);
    char buf[6];
    snprintf(buf, sizeof(buf), "%d/%d", paso + 1, total);
    display.setCursor(24, 36);
    display.print(buf);

    display.setTextSize(1);
    display.display();
}

// ── Hit correcto ──────────────────────────────────────────────────────────
void oledHitCorrecto(int pad, int puntos) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(20, 6);
    display.println("CORRECTO!");
    hline(28);
    display.setTextSize(1);
    display.setCursor(0, 34);
    display.print("PAD "); display.print(pad + 1);
    display.print("  +"); display.print(puntos); display.println(" pts");
    display.display();
    // Se muestra brevemente; el juego lo reemplaza pronto
}

// ── Hit incorrecto ────────────────────────────────────────────────────────
void oledHitIncorrecto(int pad, int vidas) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(22, 6);
    display.println("ERROR!");
    hline(28);
    display.setTextSize(1);
    display.setCursor(0, 34);
    display.print("Pad "); display.print(pad + 1);
    display.print(" incorrecto");
    display.setCursor(0, 46);
    display.print("Vidas: ");
    for (int i = 0; i < 3; i++)
        display.print(i < vidas ? "<3 " : "-- ");
    display.display();
}

// ── Score general (modo libre / actualización) ────────────────────────────
void oledScore(int puntos, int vidas, int combo) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(40, 0);
    display.println("SCORE");
    hline(10);
    display.setTextSize(3);
    // Centrar puntos
    char buf[10];
    snprintf(buf, sizeof(buf), "%d", puntos);
    int16_t x1, y1; uint16_t w, h;
    display.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h);
    display.setCursor((OLED_W - w) / 2, 14);
    display.print(buf);

    display.setTextSize(1);
    hline(44);
    display.setCursor(0, 48);
    display.print("Vidas: ");
    for (int i = 0; i < 3; i++) display.print(i < vidas ? "<3 " : "-- ");
    display.setCursor(0, 57);
    display.print("Combo: x"); display.print(combo);
    display.display();
}

// ── Modo libre ────────────────────────────────────────────────────────────
void oledModoLibre(int puntos) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(22, 0);
    display.println("MODO LIBRE");
    hline(10);
    display.setCursor(8, 16);
    display.println("Toca a tu ritmo!");
    display.setTextSize(2);
    char buf[10];
    snprintf(buf, sizeof(buf), "%d", puntos);
    int16_t x1,y1; uint16_t w,h;
    display.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h);
    display.setCursor((OLED_W - w) / 2, 32);
    display.print(buf);
    display.setTextSize(1);
    display.setCursor(44, 55);
    display.print("pts");
    display.display();
}

// ── Fin de juego ──────────────────────────────────────────────────────────
void oledFinJuego(int puntos, const char* jugador) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(28, 0);
    display.println("FIN DE JUEGO");
    hline(10);
    display.setCursor(0, 14);
    display.print("Jugador: "); display.println(jugador);
    display.setTextSize(2);
    char buf[10];
    snprintf(buf, sizeof(buf), "%d", puntos);
    int16_t x1,y1; uint16_t w,h;
    display.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h);
    display.setCursor((OLED_W - w) / 2, 28);
    display.print(buf);
    display.setTextSize(1);
    display.setCursor(48, 48);
    display.println("puntos");
    display.setCursor(10, 57);
    display.println("Ver ranking en celular");
    display.display();
}