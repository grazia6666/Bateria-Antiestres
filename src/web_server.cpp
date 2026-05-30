#include "web_server.h"
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "scores.h"
#include "wifi_manager.h"
#include "game_modes.h"

static AsyncWebServer server(80);
static AsyncWebSocket ws("/ws");

// ── Broadcast a todos los clientes WebSocket ──────────────────────────────
void wsBroadcast(const String& json) {
    ws.textAll(json);
}

// ── Manejador de mensajes WS entrantes ───────────────────────────────────
static void onWsEvent(AsyncWebSocket* s, AsyncWebSocketClient* client,
                      AwsEventType type, void* arg, uint8_t* data, size_t len)
{
    if (type != WS_EVT_DATA) return;

    AwsFrameInfo* info = (AwsFrameInfo*)arg;
    if (!info->final || info->index != 0 || info->len != len) return;
    if (info->opcode != WS_TEXT) return;

    // Parsear JSON
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, (char*)data, len);
    if (err) {
        Serial.printf("[WS] JSON inválido: %s\n", err.c_str());
        return;
    }

    const char* accion = doc["accion"] | "";
    Serial.printf("[WS] accion=%s\n", accion);

    // ── iniciar_juego ────────────────────────────────────────────────────
    if (strcmp(accion, "iniciar_juego") == 0) {
        const char* modo    = doc["modo"]    | "libre";
        const char* jugador = doc["jugador"] | "Jugador";
        gameModeStart(modo, jugador);
    }

    // ── terminar_juego ───────────────────────────────────────────────────
    else if (strcmp(accion, "terminar_juego") == 0) {
        gameModeStop();
    }

    // ── get_scores ───────────────────────────────────────────────────────
    else if (strcmp(accion, "get_scores") == 0) {
        client->text(obtenerScoresJSON());
    }

    // ── guardar_wifi ─────────────────────────────────────────────────────
    else if (strcmp(accion, "guardar_wifi") == 0) {
        int         slot = doc["slot"] | 0;
        const char* ssid = doc["ssid"] | "";
        const char* pass = doc["pass"] | "";
        guardarRed(slot, ssid, pass);
        client->text("{\"ok\":true}");
    }
}

// ── Init del servidor ─────────────────────────────────────────────────────
void servidorInit() {
    ws.onEvent(onWsEvent);
    server.addHandler(&ws);

    // Servir archivos estáticos desde LittleFS
    server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

    // Endpoint REST adicional para scores (por si el WS no está disponible)
    server.on("/api/scores", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->send(200, "application/json", obtenerScoresJSON());
    });

    // 404
    server.onNotFound([](AsyncWebServerRequest* req) {
        req->send(404, "text/plain", "Not found");
    });

    server.begin();
    Serial.println("[SERVER] Servidor HTTP + WS iniciado en puerto 80");
}