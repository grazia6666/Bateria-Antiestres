#include "web_server.h"
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <string.h>
#include "scores.h"
#include "wifi_manager.h"
#include "game_modes.h"
#include "modo_cancion.h"
/*guardo 4096 bytes para el json */
#define JSON_BUF_LEN 4096
/*creo el servidor http*/
static AsyncWebServer server(80);
/*/ws ruta -- la finalidad que la paginase comunique con el esp32*/
static AsyncWebSocket ws("/ws");

void wsBroadcast(const char* json) {
    ws.textAll(json);
}

/* ── Cola de acciones pendientes ──────────────────────────────────
   ESPAsyncWebServer ejecuta onWsEvent() en una tarea de FreeRTOS
   separada del loop() principal (en el ESP32, potencialmente en el
   otro nucleo). Antes, onWsEvent() llamaba directo a
   gameModeStart()/gameModeStop()/cancionStart()/cancionStop(), que
   leen y escriben las estructuras de estado del juego (G en
   game_modes.cpp, C en modo_cancion.cpp) -- las mismas que
   gameModeTick()/cancionTick() leen y escriben constantemente desde
   loop(). Sin ninguna proteccion, dos nucleos tocando la misma
   estructura al mismo tiempo es una condicion de carrera: se veian
   sintomas como el modo cancion siguiendo activo (imprimiendo MISS)
   varias lineas despues de haberse "detenido".

   La solucion: el callback de WS solo deja anotado QUE se pidio, en
   variables simples (no la estructura completa del juego). El unico
   que ejecuta esas acciones es procesarAccionesPendientes(), llamado
   desde loop() -- un solo hilo, sin carreras. */
#define ACCION_NINGUNA         0
#define ACCION_INICIAR_JUEGO   1
#define ACCION_TERMINAR_JUEGO  2
#define ACCION_INICIAR_CANCION 3

static volatile int  pendienteAccion = ACCION_NINGUNA;
static char          pendienteModo[16];
static char          pendienteJugador[20];
static int           pendienteCancionId;

void procesarAccionesPendientes(void) {
    /* Copia local para evitar que el callback de WS modifique estos
       valores justo mientras se estan usando aqui. */
    int accion = pendienteAccion;
    if (accion == ACCION_NINGUNA) return;
    pendienteAccion = ACCION_NINGUNA;

    switch (accion) {
        case ACCION_INICIAR_JUEGO:
            gameModeStart(pendienteModo, pendienteJugador);
            break;
        case ACCION_TERMINAR_JUEGO:
            gameModeStop();
            cancionStop();
            break;
        case ACCION_INICIAR_CANCION:
            cancionStart(pendienteCancionId, pendienteJugador);
            break;
    }
}

/* un lciente envia un mensaje mediante websocket
inicar el juego terminar el juego, etc */
static void onWsEvent(AsyncWebSocket* s, AsyncWebSocketClient* client,
                      AwsEventType type, void* arg,
                      uint8_t* data, size_t len)
{
    JsonDocument doc;
    const char*  accion;
    char         scoresBuf[JSON_BUF_LEN];

    if (type != WS_EVT_DATA) return;

    AwsFrameInfo* info = (AwsFrameInfo*)arg;
    if (!info->final || info->index != 0 || info->len != len) return;
    if (info->opcode != WS_TEXT) return;

    if (deserializeJson(doc, (char*)data, len) != DeserializationError::Ok) {
        Serial.println("[WS] JSON invalido");
        return;
    }

    accion = doc["accion"] | "";
    Serial.printf("[WS] accion=%s\n", accion);

    if (strcmp(accion, "iniciar_juego") == 0) {
        const char* modo    = doc["modo"]    | "libre";
        const char* jugador = doc["jugador"] | "Jugador";
        strncpy(pendienteModo, modo, sizeof(pendienteModo) - 1);
        pendienteModo[sizeof(pendienteModo) - 1] = '\0';
        strncpy(pendienteJugador, jugador, sizeof(pendienteJugador) - 1);
        pendienteJugador[sizeof(pendienteJugador) - 1] = '\0';
        pendienteAccion = ACCION_INICIAR_JUEGO;
    }
    else if (strcmp(accion, "terminar_juego") == 0) {
        pendienteAccion = ACCION_TERMINAR_JUEGO;
    }
    else if (strcmp(accion, "iniciar_cancion") == 0) {
        int         id      = doc["id"]      | 0;
        const char* jugador = doc["jugador"] | "Jugador";
        pendienteCancionId = id;
        strncpy(pendienteJugador, jugador, sizeof(pendienteJugador) - 1);
        pendienteJugador[sizeof(pendienteJugador) - 1] = '\0';
        pendienteAccion = ACCION_INICIAR_CANCION;
    }
    else if (strcmp(accion, "get_scores") == 0) {
        obtenerScoresJSON(scoresBuf, JSON_BUF_LEN);
        client->text(scoresBuf);
    }
    else if (strcmp(accion, "guardar_wifi") == 0) {
        int         slot = doc["slot"] | 0;
        const char* ssid = doc["ssid"] | "";
        const char* pass = doc["pass"] | "";
        guardarRed(slot, ssid, pass);
        client->text("{\"ok\":true}");
    }
}

void servidorInit(void) {
    ws.onEvent(onWsEvent); /*cada mnsj es procesdo por el esto*/
    server.addHandler(&ws); /*integra el websocket al http*/

    server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

    server.on("/api/scores", HTTP_GET, [](AsyncWebServerRequest* req) {
        static char buf[JSON_BUF_LEN];
        obtenerScoresJSON(buf, JSON_BUF_LEN);
        req->send(200, "application/json", buf);
    });

    server.on("/api/fs", HTTP_GET, [](AsyncWebServerRequest* req) {
        static char buf[128];
        unsigned long total = LittleFS.totalBytes();
        unsigned long usado = LittleFS.usedBytes();
        unsigned long libre = total - usado;
        snprintf(buf, sizeof(buf),
            "{\"total\":%lu,\"usado\":%lu,\"libre\":%lu}",
            total, usado, libre);
        req->send(200, "application/json", buf);
    });

    server.onNotFound([](AsyncWebServerRequest* req) {
        req->send(404, "text/plain", "Not found");
    });

    server.begin();
    Serial.println("[SERVER] HTTP + WS activo en puerto 80");
}