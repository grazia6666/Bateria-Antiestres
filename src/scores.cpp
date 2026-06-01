#include "scores.h"
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <string.h>

#define SCORES_FILE  "/scores.json"
#define MAX_SCORES   20

void scoresInit(void) {
    if (!LittleFS.begin(true)) {
        Serial.println("[SCORES] Error montando LittleFS");
        return;
    }
    if (!LittleFS.exists(SCORES_FILE)) {
        File f = LittleFS.open(SCORES_FILE, "w");
        f.print("{\"scores\":[]}");
        f.close();
        Serial.println("[SCORES] scores.json creado");
    }
    Serial.println("[SCORES] LittleFS listo");
}

void guardarScore(const char* nombre, const char* modo, int puntos) {
    JsonDocument doc;
    int n, minIdx, minPts, i, p;
    JsonArray arr;

    File fr = LittleFS.open(SCORES_FILE, "r");
    if (fr) { deserializeJson(doc, fr); fr.close(); }

    arr = doc["scores"].is<JsonArray>()
          ? doc["scores"].as<JsonArray>()
          : doc["scores"].to<JsonArray>();

    /* Agregar nuevo score */
    JsonObject entry = arr.add<JsonObject>();
    entry["nombre"] = nombre;
    entry["modo"]   = modo;
    entry["puntos"] = puntos;

    /* Mantener solo top MAX_SCORES */
    n = (int)arr.size();
    if (n > MAX_SCORES) {
        minIdx = 0;
        minPts = arr[0]["puntos"].as<int>();
        for (i = 1; i < n; i++) {
            p = arr[i]["puntos"].as<int>();
            if (p < minPts) { minPts = p; minIdx = i; }
        }
        arr.remove(minIdx);
    }

    File fw = LittleFS.open(SCORES_FILE, "w");
    if (fw) { serializeJson(doc, fw); fw.close(); }
    Serial.printf("[SCORES] Guardado: %s %s %d\n", nombre, modo, puntos);
}

void obtenerScoresJSON(char* buf, int bufLen) {
    File f = LittleFS.open(SCORES_FILE, "r");
    if (!f) {
        strncpy(buf, "{\"scores\":[]}", bufLen - 1);
        buf[bufLen - 1] = '\0';
        return;
    }
    int i = 0;
    while (f.available() && i < bufLen - 1) {
        buf[i++] = (char)f.read();
    }
    buf[i] = '\0';
    f.close();
}