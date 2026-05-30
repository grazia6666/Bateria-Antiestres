#include "scores.h"
#include <LittleFS.h>
#include <ArduinoJson.h>

#define SCORES_FILE  "/scores.json"
#define MAX_SCORES   20

void scoresInit() {
    if (!LittleFS.begin(true)) {
        Serial.println("[SCORES] Error montando LittleFS");
        return;
    }
    // Crear archivo vacío si no existe
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

    // Leer existente
    File fr = LittleFS.open(SCORES_FILE, "r");
    if (fr) {
        deserializeJson(doc, fr);
        fr.close();
    }

    JsonArray arr = doc["scores"].is<JsonArray>()
                    ? doc["scores"].as<JsonArray>()
                    : doc["scores"].to<JsonArray>();

    // Agregar nuevo
    JsonObject entry = arr.add<JsonObject>();
    entry["nombre"] = nombre;
    entry["modo"]   = modo;
    entry["puntos"] = puntos;

    // Mantener solo top MAX_SCORES ordenados por puntos
    // Convertir a std::vector para ordenar
    int n = arr.size();
    if (n > MAX_SCORES) {
        // Encontrar el de menor puntuación y eliminarlo
        int minIdx = 0, minPts = arr[0]["puntos"].as<int>();
        for (int i = 1; i < n; i++) {
            int p = arr[i]["puntos"].as<int>();
            if (p < minPts) { minPts = p; minIdx = i; }
        }
        arr.remove(minIdx);
    }

    // Escribir de vuelta
    File fw = LittleFS.open(SCORES_FILE, "w");
    if (fw) {
        serializeJson(doc, fw);
        fw.close();
        Serial.printf("[SCORES] Score guardado: %s %s %d\n", nombre, modo, puntos);
    }
}

String obtenerScoresJSON() {
    File f = LittleFS.open(SCORES_FILE, "r");
    if (!f) return "{\"scores\":[]}";
    String s = f.readString();
    f.close();
    return s;
}