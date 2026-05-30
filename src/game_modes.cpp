#include "game_modes.h"
#include <ArduinoJson.h>
#include "pads.h"
#include "leds.h"
#include "audio.h"
#include "oled_display.h"
#include "scores.h"
#include "web_server.h"

/* ════════════════════════════════════════════════════════════════
   CONFIGURACIÓN DE JUEGO
════════════════════════════════════════════════════════════════ */
#define VIDAS_INICIAL        3
#define COMBO_STEP           5      // aciertos seguidos para subir combo
#define COMBO_MAX            8
#define PUNTOS_BASE_REFLEJO  100    // × combo
#define PUNTOS_BASE_MEMORIA  150    // × ronda × combo
#define PUNTOS_BASE_LIBRE    10     // × intensidad normalizada
#define MEMORIA_MAX_RONDA    12     // secuencia máxima

/* ════════════════════════════════════════════════════════════════
   ESTADO GLOBAL
════════════════════════════════════════════════════════════════ */
enum Modo   { MODO_NINGUNO, MODO_LIBRE, MODO_REFLEJOS, MODO_MEMORIA };
enum Estado {
    EST_INACTIVO,
    EST_COUNTDOWN,
    EST_LIBRE_ACTIVO,
    EST_REFLEJOS_ESPERANDO,   // esperando golpe del pad activo
    EST_REFLEJOS_PAUSA,       // breve pausa entre rondas
    EST_MEMORIA_MOSTRANDO,    // ESP32 muestra la secuencia
    EST_MEMORIA_JUGADOR,      // turno del jugador
    EST_MEMORIA_PAUSA,
    EST_FIN
};

static struct {
    Modo    modo;
    Estado  estado;
    char    jugador[20];

    // Puntuación
    int     score;
    int     combo;
    int     maxCombo;
    int     aciertos;
    int     vidas;
    int     racha;        // aciertos consecutivos para subir combo

    // Reflejos
    int            padActivo;
    unsigned long  tInicioRonda;     // millis al encender el pad
    unsigned long  tPausa;           // millis al iniciar pausa

    // Memoria
    int            secuencia[MEMORIA_MAX_RONDA];
    int            seqLen;
    int            seqRonda;
    int            seqPaso;          // paso del jugador
    int            seqMostrandoIdx;  // índice mientras muestra
    unsigned long  tMostrar;         // timer para mostrar cada pad

    // Countdown
    int            countNum;
    unsigned long  tCount;

} G;

/* ════════════════════════════════════════════════════════════════
   HELPERS — broadcast JSON al celular
════════════════════════════════════════════════════════════════ */
static void broadcast(JsonDocument& doc) {
    String out;
    serializeJson(doc, out);
    wsBroadcast(out);
}

static void bcastScoreUpdate() {
    JsonDocument d;
    d["evento"]  = "score_update";
    d["puntos"]  = G.score;
    d["combo"]   = G.combo;
    broadcast(d);
}

static void bcastPadObjetivo(int pad, int ronda) {
    JsonDocument d;
    d["evento"] = "pad_objetivo";
    d["pad"]    = pad;
    d["ronda"]  = ronda;
    broadcast(d);
}

static void bcastHitCorrecto(int pad, int ptsGanados) {
    JsonDocument d;
    d["evento"]        = "hit_correcto";
    d["pad"]           = pad;
    d["puntos_ganados"]= ptsGanados;
    d["puntos_total"]  = G.score;
    d["combo"]         = G.combo;
    broadcast(d);
}

static void bcastHitIncorrecto(int pad) {
    JsonDocument d;
    d["evento"] = "hit_incorrecto";
    d["pad"]    = pad;
    d["vidas"]  = G.vidas;
    broadcast(d);
}

static void bcastTiempoAgotado() {
    JsonDocument d;
    d["evento"] = "tiempo_agotado";
    d["vidas"]  = G.vidas;
    broadcast(d);
}

static void bcastMostrandoSecuencia(int largo, int ronda) {
    JsonDocument d;
    d["evento"] = "mostrando_secuencia";
    d["largo"]  = largo;
    d["ronda"]  = ronda;
    broadcast(d);
}

static void bcastTurnoJugador(int paso, int total, int ronda) {
    JsonDocument d;
    d["evento"] = "turno_jugador";
    d["paso"]   = paso;
    d["total"]  = total;
    d["ronda"]  = ronda;
    broadcast(d);
}

static void bcastRondaCompletada(int ronda) {
    JsonDocument d;
    d["evento"]       = "ronda_completada";
    d["ronda"]        = ronda;
    d["puntos_total"] = G.score;
    broadcast(d);
}

static void bcastJuegoTerminado() {
    JsonDocument d;
    d["evento"]       = "juego_terminado";
    d["puntos_final"] = G.score;
    broadcast(d);
    // También enviar ranking actualizado
    wsBroadcast(obtenerScoresJSON());
}

/* ════════════════════════════════════════════════════════════════
   LÓGICA DE COMBO
════════════════════════════════════════════════════════════════ */
static void registrarAcierto() {
    G.aciertos++;
    G.racha++;
    if (G.racha >= COMBO_STEP) {
        G.racha = 0;
        if (G.combo < COMBO_MAX) G.combo++;
        if (G.combo > G.maxCombo) G.maxCombo = G.combo;
    }
}

static void registrarFallo() {
    G.vidas--;
    G.combo = 1;
    G.racha = 0;
}

/* ════════════════════════════════════════════════════════════════
   FIN DE JUEGO
════════════════════════════════════════════════════════════════ */
static void endGame() {
    G.estado = EST_FIN;
    ledAnimacionGameOver();
    reproducir(SND_GAMEOVER);
    oledFinJuego(G.score, G.jugador);
    guardarScore(G.jugador,
                 G.modo == MODO_LIBRE ? "libre" :
                 G.modo == MODO_REFLEJOS ? "reflejos" : "memoria",
                 G.score);
    bcastJuegoTerminado();
    G.modo   = MODO_NINGUNO;
    G.estado = EST_INACTIVO;
}

/* ════════════════════════════════════════════════════════════════
   COUNTDOWN COMÚN
════════════════════════════════════════════════════════════════ */
static void iniciarCountdown() {
    G.estado   = EST_COUNTDOWN;
    G.countNum = 3;
    G.tCount   = millis();
    oledContdown(G.countNum);
    reproducir(SND_COUNTDOWN);
}

/* ════════════════════════════════════════════════════════════════
   MODO REFLEJOS — funciones
════════════════════════════════════════════════════════════════ */
static void reflejosNuevaRonda() {
    // Pad aleatorio
    G.padActivo    = random(0, NUM_PADS);
    G.tInicioRonda = millis();
    G.estado       = EST_REFLEJOS_ESPERANDO;
    int ronda      = G.aciertos + 1;

    ledEncender(G.padActivo);
    oledPadObjetivo(G.padActivo, 100);
    bcastPadObjetivo(G.padActivo, ronda);

    Serial.printf("[REFLEJOS] Ronda %d → PAD %d\n", ronda, G.padActivo + 1);
}

/* ════════════════════════════════════════════════════════════════
   MODO MEMORIA — funciones
════════════════════════════════════════════════════════════════ */
static void memoriaNuevaRonda() {
    G.seqRonda++;
    // Agregar un pad aleatorio a la secuencia
    if (G.seqLen < MEMORIA_MAX_RONDA) {
        G.secuencia[G.seqLen++] = random(0, NUM_PADS);
    }
    G.seqMostrandoIdx = 0;
    G.tMostrar        = millis();
    G.estado          = EST_MEMORIA_MOSTRANDO;

    oledMostrarSecuencia(G.secuencia, G.seqLen, G.seqRonda);
    bcastMostrandoSecuencia(G.seqLen, G.seqRonda);

    Serial.printf("[MEMORIA] Ronda %d — secuencia de %d\n", G.seqRonda, G.seqLen);
}

static void memoriaIniciarTurno() {
    G.seqPaso = 0;
    G.estado  = EST_MEMORIA_JUGADOR;
    oledTurnoJugador(0, G.seqLen, G.seqRonda);
    bcastTurnoJugador(0, G.seqLen, G.seqRonda);
}

/* ════════════════════════════════════════════════════════════════
   API PÚBLICA
════════════════════════════════════════════════════════════════ */
void gameModeStart(const char* modo, const char* jugador) {
    // Reset estado
    G.score    = 0;
    G.combo    = 1;
    G.maxCombo = 1;
    G.aciertos = 0;
    G.vidas    = VIDAS_INICIAL;
    G.racha    = 0;
    G.seqLen   = 0;
    G.seqRonda = 0;
    G.padActivo= -1;

    strncpy(G.jugador, jugador, sizeof(G.jugador) - 1);
    G.jugador[sizeof(G.jugador) - 1] = '\0';

    if      (strcmp(modo, "reflejos") == 0) G.modo = MODO_REFLEJOS;
    else if (strcmp(modo, "memoria")  == 0) G.modo = MODO_MEMORIA;
    else                                    G.modo = MODO_LIBRE;

    ledApagarTodos();
    ledAnimacionInicio();

    // Broadcast: juego iniciado
    JsonDocument d;
    d["evento"] = "juego_iniciado";
    d["vidas"]  = VIDAS_INICIAL;
    d["modo"]   = modo;
    broadcast(d);

    if (G.modo == MODO_LIBRE) {
        G.estado = EST_LIBRE_ACTIVO;
        oledModoLibre(0);
        Serial.println("[GAME] Modo LIBRE iniciado");
    } else {
        iniciarCountdown();
        Serial.printf("[GAME] Modo %s — countdown\n", modo);
    }
}

void gameModeStop() {
    if (G.estado == EST_INACTIVO) return;
    ledApagarTodos();
    G.estado = EST_INACTIVO;
    G.modo   = MODO_NINGUNO;
    Serial.println("[GAME] Juego detenido por usuario");
}

/* ════════════════════════════════════════════════════════════════
   TICK — se llama en cada loop()
════════════════════════════════════════════════════════════════ */
void gameModeTick() {
    if (G.estado == EST_INACTIVO || G.estado == EST_FIN) return;

    unsigned long ahora = millis();

    /* ── COUNTDOWN ──────────────────────────────────────────────── */
    if (G.estado == EST_COUNTDOWN) {
        if (ahora - G.tCount >= 950) {
            G.tCount = ahora;
            G.countNum--;
            if (G.countNum > 0) {
                oledContdown(G.countNum);
                reproducir(SND_COUNTDOWN);
            } else {
                oledContdown(0);   // "¡YA!"
                delay(500);
                if      (G.modo == MODO_REFLEJOS) reflejosNuevaRonda();
                else if (G.modo == MODO_MEMORIA)  memoriaNuevaRonda();
            }
        }
        return;
    }

    /* ── MODO LIBRE ─────────────────────────────────────────────── */
    if (G.estado == EST_LIBRE_ACTIVO) {
        GolpePad g = leerGolpe();
        if (g.pad != -1) {
            int pts = map(g.intensidad, PIEZO_UMBRAL, 4095, PUNTOS_BASE_LIBRE, PUNTOS_BASE_LIBRE * 5);
            G.score += pts;
            ledSetBrillo(g.intensidad);
            ledEncender(g.pad);
            delay(60);
            ledApagar(g.pad);
            reproducirPad(g.pad);
            oledModoLibre(G.score);
            bcastScoreUpdate();
        }
        return;
    }

    /* ── MODO REFLEJOS — esperando golpe ────────────────────────── */
    if (G.estado == EST_REFLEJOS_ESPERANDO) {
        unsigned long elapsed = ahora - G.tInicioRonda;

        // Actualizar barra de tiempo en OLED cada 50 ms
        static unsigned long tUltOled = 0;
        if (ahora - tUltOled > 50) {
            tUltOled = ahora;
            int barra = map(elapsed, 0, TIEMPO_REFLEJO_MS, 100, 0);
            barra = constrain(barra, 0, 100);
            oledPadObjetivo(G.padActivo, barra);
        }

        // Tiempo agotado
        if (elapsed >= (unsigned long)TIEMPO_REFLEJO_MS) {
            ledAnimacionIncorrecto(G.padActivo);
            reproducir(SND_MISS);
            registrarFallo();
            bcastTiempoAgotado();
            oledHitIncorrecto(G.padActivo, G.vidas);
            if (G.vidas <= 0) { endGame(); return; }
            G.estado  = EST_REFLEJOS_PAUSA;
            G.tPausa  = ahora;
            return;
        }

        // Leer golpe
        GolpePad g = leerGolpe();
        if (g.pad == -1) return;

        ledApagar(G.padActivo);

        if (g.pad == G.padActivo) {
            // ✅ CORRECTO
            int pts = PUNTOS_BASE_REFLEJO * G.combo;
            G.score += pts;
            registrarAcierto();
            ledAnimacionCorrecto(g.pad);
            reproducir(SND_HIT);
            if (G.combo >= 3) reproducir(SND_COMBO);
            oledHitCorrecto(g.pad, pts);
            bcastHitCorrecto(g.pad, pts);
        } else {
            // ❌ INCORRECTO
            registrarFallo();
            ledAnimacionIncorrecto(g.pad);
            reproducir(SND_MISS);
            oledHitIncorrecto(g.pad, G.vidas);
            bcastHitIncorrecto(g.pad);
            if (G.vidas <= 0) { endGame(); return; }
        }

        G.estado = EST_REFLEJOS_PAUSA;
        G.tPausa = ahora;
        return;
    }

    /* ── MODO REFLEJOS — pausa entre rondas ─────────────────────── */
    if (G.estado == EST_REFLEJOS_PAUSA) {
        if (ahora - G.tPausa >= 700) {
            reflejosNuevaRonda();
        }
        return;
    }

    /* ── MODO MEMORIA — mostrando secuencia ─────────────────────── */
    if (G.estado == EST_MEMORIA_MOSTRANDO) {
        // Mostrar cada pad 500 ms encendido + 300 ms apagado
        static bool padEncendido = false;
        static unsigned long tPad = 0;

        if (!padEncendido) {
            if (ahora - G.tMostrar >= 300) {  // espera apagado
                if (G.seqMostrandoIdx >= G.seqLen) {
                    // Terminó de mostrar → turno jugador
                    delay(400);
                    memoriaIniciarTurno();
                    padEncendido = false;
                    return;
                }
                int p = G.secuencia[G.seqMostrandoIdx];
                ledEncender(p);
                reproducirPad(p);
                tPad       = ahora;
                padEncendido = true;
            }
        } else {
            if (ahora - tPad >= 500) {
                int p = G.secuencia[G.seqMostrandoIdx];
                ledApagar(p);
                G.seqMostrandoIdx++;
                G.tMostrar   = ahora;
                padEncendido = false;
            }
        }
        return;
    }

    /* ── MODO MEMORIA — turno del jugador ───────────────────────── */
    if (G.estado == EST_MEMORIA_JUGADOR) {
        GolpePad g = leerGolpe();
        if (g.pad == -1) return;

        int esperado = G.secuencia[G.seqPaso];

        if (g.pad == esperado) {
            // ✅ Paso correcto
            ledAnimacionCorrecto(g.pad);
            reproducir(SND_HIT);
            G.seqPaso++;
            oledTurnoJugador(G.seqPaso, G.seqLen, G.seqRonda);
            bcastTurnoJugador(G.seqPaso, G.seqLen, G.seqRonda);

            if (G.seqPaso >= G.seqLen) {
                // ✅ Ronda completa
                int pts = PUNTOS_BASE_MEMORIA * G.seqRonda * G.combo;
                G.score += pts;
                registrarAcierto();
                if (G.combo >= 3) reproducir(SND_COMBO);
                reproducir(SND_WIN);
                bcastRondaCompletada(G.seqRonda);

                if (G.seqLen >= MEMORIA_MAX_RONDA) {
                    // Ganó todas las rondas
                    endGame();
                    return;
                }
                G.estado = EST_MEMORIA_PAUSA;
                G.tPausa = ahora;
            }
        } else {
            // ❌ Paso incorrecto
            ledAnimacionIncorrecto(g.pad);
            reproducir(SND_MISS);
            registrarFallo();
            oledHitIncorrecto(g.pad, G.vidas);
            bcastHitIncorrecto(g.pad);

            if (G.vidas <= 0) { endGame(); return; }

            // Reintentar misma ronda: mostrar secuencia de nuevo
            G.seqPaso         = 0;
            G.seqMostrandoIdx = 0;
            G.tMostrar        = ahora;
            G.estado          = EST_MEMORIA_PAUSA;
            G.tPausa          = ahora;
        }
        return;
    }

    /* ── MODO MEMORIA — pausa entre rondas ──────────────────────── */
    if (G.estado == EST_MEMORIA_PAUSA) {
        if (ahora - G.tPausa >= 1000) {
            memoriaNuevaRonda();
        }
        return;
    }
}