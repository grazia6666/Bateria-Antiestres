#include "game_modes.h"
#include <ArduinoJson.h>
#include <string.h>
#include <stdio.h>
#include "pads.h"
#include "leds.h"
#include "audio.h"
#include "oled_display.h"
#include "scores.h"
#include "web_server.h"
#include "modo_cancion.h"

/*  Configuracion*/
#define VIDAS_INICIAL        3
#define COMBO_STEP           5 /*5 aciertos un compo*/
#define COMBO_MAX            8
#define PUNTOS_BASE_REFLEJO  100
#define PUNTOS_BASE_MEMORIA  150
#define PUNTOS_BASE_LIBRE    10
#define MEMORIA_MAX_RONDA    12 /*maximo de rondas*/
#define JSON_BUF              256

/*Enumeraciones estilo C */
#define MODO_NINGUNO   0
#define MODO_LIBRE     1
#define MODO_REFLEJOS  2
#define MODO_MEMORIA   3
 /*estado del juego */
#define EST_INACTIVO           0
#define EST_COUNTDOWN          1
#define EST_LIBRE_ACTIVO       2
#define EST_REFLEJOS_ESPERANDO 3
#define EST_REFLEJOS_PAUSA     4
#define EST_MEMORIA_MOSTRANDO  5
#define EST_MEMORIA_JUGADOR    6
#define EST_MEMORIA_PAUSA      7
#define EST_FIN                8

/* ── Estado global del juego ─────────────────────────────────── */
static struct {
    
    int  modo; /*guarda el modo actual*/
    int  estado; /*estado del juego lo de arriba*/
    char jugador[20];

    int  score;
    int  combo;
    int  maxCombo;
    int  aciertos;
    int  vidas;
    int  racha;

    /* reflejos */
    int           padActivo; /*pad que el jugador debe tocar*/
    unsigned long tInicioRonda; 
    unsigned long tPausa; /*tiempo entre rondas*/

    /* memoria */
    int           secuencia[MEMORIA_MAX_RONDA]; /*es la secuencia que debe seguir*/
    int           seqLen; /*longitud*/
    int           seqRonda; 
    int           seqPaso; 
    int           seqMostrandoIdx;
    unsigned long tMostrar;
    int           padEncendido;

    /* countdown */
    int           countNum;
    unsigned long tCount;
} G;

/* envia info mediante el websocket esp32-json -pagina*/
static void broadcast_json(JsonDocument& doc) {
    char buf[JSON_BUF];
    serializeJson(doc, buf, sizeof(buf));
    wsBroadcast(buf);
}

/*INICIO DE LOS EVENTOS*/
static void bcast_score_update(void) {
    JsonDocument d;
    d["evento"] = "score_update";
    d["puntos"] = G.score;
    d["combo"]  = G.combo;
    broadcast_json(d);
}

static void bcast_pad_objetivo(int pad, int ronda) {
    JsonDocument d;
    d["evento"] = "pad_objetivo";
    d["pad"]    = pad;
    d["ronda"]  = ronda;
    broadcast_json(d);
}

static void bcast_hit_correcto(int pad, int pts) {
    JsonDocument d;
    d["evento"]         = "hit_correcto";
    d["pad"]            = pad;
    d["puntos_ganados"] = pts;
    d["puntos_total"]   = G.score;
    d["combo"]          = G.combo;
    broadcast_json(d);
}

static void bcast_hit_incorrecto(int pad) {
    JsonDocument d;
    d["evento"] = "hit_incorrecto";
    d["pad"]    = pad;
    d["vidas"]  = G.vidas;
    broadcast_json(d);
}

static void bcast_tiempo_agotado(void) {
    JsonDocument d;
    d["evento"] = "tiempo_agotado";
    d["vidas"]  = G.vidas;
    broadcast_json(d);
}


static void bcast_mostrando_secuencia(int largo, int ronda) {
    JsonDocument d;
    d["evento"] = "mostrando_secuencia";
    d["largo"]  = largo;
    d["ronda"]  = ronda;
    broadcast_json(d);
}

static void bcast_turno_jugador(int paso, int total, int ronda) {
    JsonDocument d;
    d["evento"] = "turno_jugador";
    d["paso"]   = paso;
    d["total"]  = total;
    d["ronda"]  = ronda;
    broadcast_json(d);
}

static void bcast_ronda_completada(int ronda) {
    JsonDocument d;
    d["evento"]       = "ronda_completada";
    d["ronda"]        = ronda;
    d["puntos_total"] = G.score;
    broadcast_json(d);
}

static void bcast_juego_terminado(void) {
    char scoresBuf[2048];
    JsonDocument d;
    d["evento"]       = "juego_terminado";
    d["puntos_final"] = G.score;
    broadcast_json(d);
    obtenerScoresJSON(scoresBuf, sizeof(scoresBuf));
    wsBroadcast(scoresBuf);
}

/*FIN DE LOS EVENTOS*/

/* Logica de combo siempre y cuando la rahca mayor 5 y el combo max 8 */
static void registrar_acierto(void) {
    G.aciertos++;
    G.racha++;
    if (G.racha >= COMBO_STEP) {
        G.racha = 0;
        if (G.combo < COMBO_MAX) G.combo++;
        if (G.combo > G.maxCombo) G.maxCombo = G.combo;
    }
}
/*fallo y quito vida reseteo el combo y la racha*/

static void registrar_fallo(void) {
    G.vidas--;
    G.combo = 1;
    G.racha = 0;
}

/*Fin de juego */
static void end_game(void) {
    const char* modo_str;
    G.estado = EST_FIN;
    ledAnimacionGameOver();
    oledFinJuego(G.score, G.jugador);

    modo_str = (G.modo == MODO_LIBRE)    ? "libre"    :
               (G.modo == MODO_REFLEJOS) ? "reflejos" : "memoria";

    guardarScore(G.jugador, modo_str, G.score);
    bcast_juego_terminado();
    G.modo   = MODO_NINGUNO;
    G.estado = EST_INACTIVO;
}

/*  Countdown cuando inicia cada juego el 3 2 1 */
static void iniciar_countdown(void) {
    G.estado   = EST_COUNTDOWN;
    G.countNum = 3;
    G.tCount   = millis();
    oledContdown(G.countNum);
}

/* Modo Reflejos:  me dice que pad tocar enciende el led muestra en la oled envia el websocket y el jugador responda */
static void reflejos_nueva_ronda(void) {
    int ronda = G.aciertos + 1;
    G.padActivo    = random(0, NUM_PADS);
    G.tInicioRonda = millis();
    G.estado       = EST_REFLEJOS_ESPERANDO;
    ledEncender(G.padActivo);
    oledPadObjetivo(G.padActivo, 100);
    bcast_pad_objetivo(G.padActivo, ronda);
    Serial.printf("\n[REFLEJOS] === Ronda %d ===\n", ronda);
    Serial.printf("[REFLEJOS]  >> TOCA PAD %d <<\n\n", G.padActivo + 1);
}

/*Memoria */
static void memoria_nueva_ronda(void) {
    int i;
    G.seqRonda++;
    if (G.seqLen < MEMORIA_MAX_RONDA)
        G.secuencia[G.seqLen++] = random(0, NUM_PADS);
    G.seqMostrandoIdx = 0;
    G.padEncendido    = 0;
    G.tMostrar        = millis();
    G.estado          = EST_MEMORIA_MOSTRANDO;
    oledMostrarSecuencia(G.secuencia, G.seqLen, G.seqRonda);
    bcast_mostrando_secuencia(G.seqLen, G.seqRonda);

    Serial.printf("\n[MEMORIA] === Ronda %d ===  Secuencia de %d pads:\n", G.seqRonda, G.seqLen);
    Serial.print("[MEMORIA]  Orden: ");
    for (i = 0; i < G.seqLen; i++) {
        Serial.printf("PAD %d", G.secuencia[i] + 1);
        if (i < G.seqLen - 1) Serial.print(" -> ");
    }
    Serial.println();
    Serial.println("[MEMORIA]  Memoriza y repite en ese orden!\n");
}

static void memoria_iniciar_turno(void) {
    G.seqPaso = 0;
    G.estado  = EST_MEMORIA_JUGADOR;
    oledTurnoJugador(0, G.seqLen, G.seqRonda);
    bcast_turno_jugador(0, G.seqLen, G.seqRonda);
    Serial.println("[MEMORIA]  >> AHORA ES TU TURNO - repite la secuencia en los pads!");
}

/* INICIO DE JUEGO  */
void gameModeStart(const char* modo, const char* jugador) {
    JsonDocument d;
    char buf[JSON_BUF];

    G.score     = 0;
    G.combo     = 1;
    G.maxCombo  = 1;
    G.aciertos  = 0;
    G.vidas     = VIDAS_INICIAL;
    G.racha     = 0;
    G.seqLen    = 0;
    G.seqRonda  = 0;
    G.padActivo = -1;

    strncpy(G.jugador, jugador, sizeof(G.jugador) - 1);
    G.jugador[sizeof(G.jugador) - 1] = '\0';

    if      (strcmp(modo, "reflejos") == 0) G.modo = MODO_REFLEJOS;
    else if (strcmp(modo, "memoria")  == 0) G.modo = MODO_MEMORIA;
    else                                    G.modo = MODO_LIBRE;

    ledApagarTodos();
    ledAnimacionInicio();

    d["evento"] = "juego_iniciado";
    d["vidas"]  = VIDAS_INICIAL;
    d["modo"]   = modo;
    serializeJson(d, buf, sizeof(buf));
    wsBroadcast(buf);

    if (G.modo == MODO_LIBRE) {
        G.estado = EST_LIBRE_ACTIVO;
        oledModoLibre(0);
        Serial.println("[GAME] Modo LIBRE");
    } else {
        iniciar_countdown();
        Serial.printf("[GAME] Modo %s  countdown\n", modo);
    }
}

void gameModeStop(void) {
    if (G.estado == EST_INACTIVO) return;
    ledApagarTodos();
    G.estado = EST_INACTIVO;
    G.modo   = MODO_NINGUNO;
    Serial.println("[GAME] Detenido por usuario");
}

/*   TICK — llamado en cada loop()*/
void gameModeTick(void) {
    unsigned long ahora = millis();
    unsigned long elapsed;
    int           barra, pts, p, esperado;
    GolpePad      g;

    /* Tick del modo cancion (corre en paralelo con game_modes) */
    cancionTick();

    if (G.estado == EST_INACTIVO || G.estado == EST_FIN) return;

    /* COUNTDOWN */
    if (G.estado == EST_COUNTDOWN) {
        if (ahora - G.tCount >= 950UL) {
            G.tCount = ahora;
            G.countNum--;
            oledContdown(G.countNum);
            if (G.countNum <= 0) {
                delay(500);
                if      (G.modo == MODO_REFLEJOS) reflejos_nueva_ronda();
                else if (G.modo == MODO_MEMORIA)  memoria_nueva_ronda();
            }
        }
        return;
    }

    /* MODO LIBRE  */
    if (G.estado == EST_LIBRE_ACTIVO) {
        g = leerGolpe();
        if (g.pad != -1) {
            pts = PUNTOS_BASE_LIBRE +
                  (int)((long)(g.intensidad - PIEZO_UMBRAL) * (PUNTOS_BASE_LIBRE * 4) /
                        (4095 - PIEZO_UMBRAL));
            G.score += pts;
            reproducirPad(g.pad);
            ledSetBrillo(g.intensidad);
            ledEncender(g.pad);
            delay(60);
            ledApagar(g.pad);
            oledModoLibre(G.score);
            bcast_score_update();
        }
        return;
    }

    /* REFLEJOS  esperando golpe */
    if (G.estado == EST_REFLEJOS_ESPERANDO) {
        elapsed = ahora - G.tInicioRonda;

        static unsigned long tUltOled = 0;
        if (ahora - tUltOled > 50UL) {
            tUltOled = ahora;
            barra = (int)(((long)(TIEMPO_REFLEJO_MS - elapsed) * 100) /
                          TIEMPO_REFLEJO_MS);
            if (barra < 0)   barra = 0;
            if (barra > 100) barra = 100;
            oledPadObjetivo(G.padActivo, barra);
        }

        if (elapsed >= (unsigned long)TIEMPO_REFLEJO_MS) {
            ledAnimacionIncorrecto(G.padActivo);
            registrar_fallo();
            bcast_tiempo_agotado();
            oledHitIncorrecto(G.padActivo, G.vidas);
            if (G.vidas <= 0) { end_game(); return; }
            G.estado = EST_REFLEJOS_PAUSA;
            G.tPausa = ahora;
            return;
        }

        g = leerGolpe();
        if (g.pad == -1) return;

        ledApagar(G.padActivo);

        if (g.pad == G.padActivo) {
            pts = PUNTOS_BASE_REFLEJO * G.combo;
            G.score += pts;
            registrar_acierto();
            reproducirPad(g.pad);
            ledAnimacionCorrecto(g.pad);
            oledHitCorrecto(g.pad, pts);
            bcast_hit_correcto(g.pad, pts);
        } else {
            registrar_fallo();
            reproducirPad(g.pad);
            ledAnimacionIncorrecto(g.pad);
            oledHitIncorrecto(g.pad, G.vidas);
            bcast_hit_incorrecto(g.pad);
            if (G.vidas <= 0) { end_game(); return; }
        }
        G.estado = EST_REFLEJOS_PAUSA;
        G.tPausa = ahora;
        return;
    }

    /*REFLEJOS — pausa */
    if (G.estado == EST_REFLEJOS_PAUSA) {
        if (ahora - G.tPausa >= 700UL) reflejos_nueva_ronda();
        return;
    }

    /* MEMORIA — mostrando secuencia */
    if (G.estado == EST_MEMORIA_MOSTRANDO) {
        if (!G.padEncendido) {
            if (ahora - G.tMostrar >= 300UL) {
                if (G.seqMostrandoIdx >= G.seqLen) {
                    delay(400);
                    memoria_iniciar_turno();
                    return;
                }
                p = G.secuencia[G.seqMostrandoIdx];
                ledEncender(p);
                /* sin sonido — solo LED al mostrar secuencia */
                G.tMostrar    = ahora;
                G.padEncendido = 1;
            }
        } else {
            if (ahora - G.tMostrar >= 500UL) {
                p = G.secuencia[G.seqMostrandoIdx];
                ledApagar(p);
                G.seqMostrandoIdx++;
                G.tMostrar     = ahora;
                G.padEncendido = 0;
            }
        }
        return;
    }

    /* MEMORIA — turno jugador*/
    if (G.estado == EST_MEMORIA_JUGADOR) {
        g = leerGolpe();
        if (g.pad == -1) return;

        esperado = G.secuencia[G.seqPaso];

        if (g.pad == esperado) {
            reproducirPad(g.pad);
            ledAnimacionCorrecto(g.pad);
            G.seqPaso++;
            oledTurnoJugador(G.seqPaso, G.seqLen, G.seqRonda);
            bcast_turno_jugador(G.seqPaso, G.seqLen, G.seqRonda);

            if (G.seqPaso >= G.seqLen) {
                pts = PUNTOS_BASE_MEMORIA * G.seqRonda * G.combo;
                G.score += pts;
                registrar_acierto();
                bcast_ronda_completada(G.seqRonda);
                if (G.seqLen >= MEMORIA_MAX_RONDA) { end_game(); return; }
                G.estado = EST_MEMORIA_PAUSA;
                G.tPausa = ahora;
            }
        } else {
            reproducirPad(g.pad);
            ledAnimacionIncorrecto(g.pad);
            registrar_fallo();
            oledHitIncorrecto(g.pad, G.vidas);
            bcast_hit_incorrecto(g.pad);
            if (G.vidas <= 0) { end_game(); return; }
            G.seqPaso         = 0;
            G.seqMostrandoIdx = 0;
            G.padEncendido    = 0;
            G.tMostrar        = ahora;
            G.estado          = EST_MEMORIA_PAUSA;
            G.tPausa          = ahora;
        }
        return;
    }

    /*  MEMORIA — pausa entre rondas  */
    if (G.estado == EST_MEMORIA_PAUSA) {
        if (ahora - G.tPausa >= 1000UL) memoria_nueva_ronda();
        return;
    }
}