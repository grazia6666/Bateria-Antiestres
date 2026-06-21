#include "modo_cancion.h"
#include <ArduinoJson.h>
#include <string.h>
#include <stdio.h>
#include "pads.h"
#include "leds.h"
#include "audio.h"
#include "oled_display.h"
#include "scores.h"
#include "web_server.h"

/* ── Solo las pistas disponibles ────────────────────────────── */
#include "pista_billie_jean.h"
#include "pista_seven_nation.h"
/* #include "pista_camisa_negra.h"    — pendiente */
/* #include "pista_center_mass.h"     — pendiente */
/* #include "pista_overcompensate.h"  — pendiente */

/* ── Tabla de canciones — solo id 0 y 4 activos ─────────────── */
static NotaRitmica* tablaNotas[TOTAL_CANCIONES] = {
    pistaBillieJean,      /* id 0 */
    NULL,                 /* id 1 — Camisa Negra, pendiente */
    NULL,                 /* id 2 — Center of Mass, pendiente */
    NULL,                 /* id 3 — Overcompensate, pendiente */
    pistaSevenNationArmy  /* id 4 */
};

static const int totalNotas[TOTAL_CANCIONES] = {
    TOTAL_NOTAS_BILLIE_JEAN,
    0,
    0,
    0,
    TOTAL_NOTAS_SEVEN_NATION
};

static const char* nombresCancion[TOTAL_CANCIONES] = {
    "Billie Jean",
    "Camisa Negra",
    "Center of Mass",
    "Overcompensate",
    "Seven Nation Army"
};

static const int archivoCancion[TOTAL_CANCIONES] = {
    CANCION_BILLIE_JEAN,
    CANCION_CAMISA_NEGRA,
    CANCION_CENTER_MASS,
    CANCION_OVERCOMPENSATE,
    CANCION_SEVEN_NATION
};

/* ── Estado del modo cancion ─────────────────────────────────── */
static struct {
    int           activo;
    int           idCancion;
    char          jugador[20];
    unsigned long tInicio;
    int           score;
    int           combo;
    int           maxCombo;
    int           aciertos;
    int           fallos;
    int           racha;
    NotaRitmica*  notas;
    int           totalN;
} C;

/* ── Helpers broadcast ───────────────────────────────────────── */
static void cbcast(JsonDocument& doc) {
    char buf[256];
    serializeJson(doc, buf, sizeof(buf));
    wsBroadcast(buf);
}

static void bcast_cancion_update(void) {
    JsonDocument d;
    d["evento"]   = "cancion_update";
    d["puntos"]   = C.score;
    d["combo"]    = C.combo;
    d["aciertos"] = C.aciertos;
    d["fallos"]   = C.fallos;
    cbcast(d);
}

static void bcast_nota_hit(int pad, int pts, int perfecto) {
    JsonDocument d;
    d["evento"]   = "nota_hit";
    d["pad"]      = pad;
    d["puntos"]   = pts;
    d["perfecto"] = perfecto;
    d["combo"]    = C.combo;
    cbcast(d);
}

static void bcast_nota_miss(int pad) {
    JsonDocument d;
    d["evento"] = "nota_miss";
    d["pad"]    = pad;
    cbcast(d);
}

static void bcast_cancion_fin(void) {
    char scoresBuf[2048];
    JsonDocument d;
    d["evento"]       = "cancion_terminada";
    d["puntos_final"] = C.score;
    d["aciertos"]     = C.aciertos;
    d["fallos"]       = C.fallos;
    d["cancion"]      = nombresCancion[C.idCancion];
    cbcast(d);
    obtenerScoresJSON(scoresBuf, sizeof(scoresBuf));
    wsBroadcast(scoresBuf);
}

/* ── Reset de flags de la pista ──────────────────────────────── */
static void resetPista(void) {
    int i;
    for (i = 0; i < C.totalN; i++) {
        C.notas[i].luzEncendida = 0;
        C.notas[i].evaluada     = 0;
    }
}

/* ── API publica ─────────────────────────────────────────────── */
void cancionStart(int idCancion, const char* jugador) {
    JsonDocument d;
    char buf[128];

    /* Validar que la cancion este disponible */
    if (idCancion < 0 || idCancion >= TOTAL_CANCIONES ||
        tablaNotas[idCancion] == NULL) {
        Serial.printf("[CANCION] id=%d no disponible\n", idCancion);
        return;
    }

    C.activo    = 1;
    C.idCancion = idCancion;
    C.score     = 0;
    C.combo     = 1;
    C.maxCombo  = 1;
    C.aciertos  = 0;
    C.fallos    = 0;
    C.racha     = 0;
    C.notas     = tablaNotas[idCancion];
    C.totalN    = totalNotas[idCancion];

    strncpy(C.jugador, jugador, sizeof(C.jugador) - 1);
    C.jugador[sizeof(C.jugador) - 1] = '\0';

    resetPista();
    ledApagarTodos();

    reproducirCancion(archivoCancion[idCancion]);

    delay(500);
    C.tInicio = millis();

    d["evento"]  = "cancion_iniciada";
    d["cancion"] = nombresCancion[idCancion];
    d["jugador"] = jugador;
    serializeJson(d, buf, sizeof(buf));
    wsBroadcast(buf);

    Serial.printf("\n[CANCION] Iniciando: %s (%d notas)\n",
                  nombresCancion[idCancion], C.totalN);
    Serial.printf("[CANCION] Primera nota: pad=%d t=%lu\n",
                  C.notas[0].pad, C.notas[0].tiempo_ms);
    Serial.printf("[CANCION] Ultima nota : pad=%d t=%lu\n",
                  C.notas[C.totalN-1].pad, C.notas[C.totalN-1].tiempo_ms);
    Serial.printf("[CANCION] Archivo DFPlayer: %d\n",
                  archivoCancion[idCancion]);
}

void cancionStop(void) {
    if (!C.activo) return;
    C.activo = 0;
    ledApagarTodos();
    detenerCancion();
    Serial.println("[CANCION] Detenida por usuario");
}

int cancionActiva(void) {
    return C.activo;
}

/* ── Tick principal ──────────────────────────────────────────── */
void cancionTick(void) {
    unsigned long ahora, elapsed;
    int i, pad, pts, perfecto;
    GolpePad g;

    if (!C.activo) return;

    ahora   = millis();
    elapsed = ahora - C.tInicio;

    /* ── 1. Encender LEDs y detectar MISS ────────────────── */
    for (i = 0; i < C.totalN; i++) {
        if (C.notas[i].evaluada) continue;

        if (!C.notas[i].luzEncendida &&
            elapsed >= C.notas[i].tiempo_ms - ANTICIPACION_VISUAL) {
            ledEncender(C.notas[i].pad);
            C.notas[i].luzEncendida = 1;
        }

        if (elapsed > C.notas[i].tiempo_ms + VENTANA_TOLERANCIA) {
            C.notas[i].evaluada = 1;
            ledApagar(C.notas[i].pad);
            C.fallos++;
            C.combo = 1;
            C.racha = 0;
            bcast_nota_miss(C.notas[i].pad);
            Serial.printf("[CANCION] MISS PAD %d\n", C.notas[i].pad + 1);
        }
    }

    /* ── 2. Leer golpe del jugador ────────────────────────── */
    g = leerGolpe();
    if (g.pad == -1) goto check_fin;

    pad      = g.pad;
    pts      = 0;
    perfecto = 0;

    for (i = 0; i < C.totalN; i++) {
        if (C.notas[i].evaluada)      continue;
        if (C.notas[i].pad != pad)    continue;
        if (!C.notas[i].luzEncendida) continue;

        long diff = (long)elapsed - (long)C.notas[i].tiempo_ms;
        if (diff < 0) diff = -diff;

        if (diff <= VENTANA_TOLERANCIA) {
            C.notas[i].evaluada = 1;
            perfecto = (diff <= VENTANA_TOLERANCIA / 2) ? 1 : 0;
            pts      = perfecto ? 200 * C.combo : 100 * C.combo;
            C.score += pts;
            C.aciertos++;
            C.racha++;
            if (C.racha >= 5) {
                C.racha = 0;
                if (C.combo < 8) C.combo++;
                if (C.combo > C.maxCombo) C.maxCombo = C.combo;
            }
            reproducirPad(pad);
            ledAnimacionCorrecto(pad);
            bcast_nota_hit(pad, pts, perfecto);
            bcast_cancion_update();
            Serial.printf("[CANCION] %s PAD %d +%d pts combo x%d\n",
                          perfecto ? "PERFECTO!" : "BIEN!",
                          pad + 1, pts, C.combo);
            goto check_fin;
        }
    }

    /* Golpe fuera de ventana */
    C.combo = 1;
    C.racha = 0;
    reproducirPad(pad);
    ledAnimacionIncorrecto(pad);
    Serial.printf("[CANCION] Fuera de tiempo PAD %d\n", pad + 1);

check_fin:
    {
        int todasEvaluadas = 1;
        for (i = 0; i < C.totalN; i++) {
            if (!C.notas[i].evaluada) { todasEvaluadas = 0; break; }
        }
        if (todasEvaluadas) {
            C.activo = 0;
            oledFinJuego(C.score, C.jugador);
            guardarScore(C.jugador, nombresCancion[C.idCancion], C.score);
            bcast_cancion_fin();
            Serial.printf("[CANCION] Fin! Score=%d Aciertos=%d Fallos=%d\n",
                          C.score, C.aciertos, C.fallos);
        }
    }
}