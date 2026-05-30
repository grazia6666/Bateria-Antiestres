#include "wifi_manager.h"
#include <Preferences.h>
#include <WiFi.h>

static Preferences prefs;

// ── Guarda credenciales en NVS (slot 0 o 1) ──────────────────────────────
void guardarRed(int slot, const char* ssid, const char* pass) {
    prefs.begin("wifi-creds", false);
    prefs.putString(("ssid" + String(slot)).c_str(), ssid);
    prefs.putString(("pass" + String(slot)).c_str(), pass);
    prefs.end();
    Serial.printf("[WiFi] Red %d guardada: %s\n", slot, ssid);
}

// ── Intenta conectar a las redes guardadas (slot 0 primero, luego 1) ─────
bool intentarConexion() {
    prefs.begin("wifi-creds", true);

    for (int slot = 0; slot < 2; slot++) {
        String ssid = prefs.getString(("ssid" + String(slot)).c_str(), "");
        String pass = prefs.getString(("pass" + String(slot)).c_str(), "");

        if (ssid.length() == 0) continue;

        Serial.printf("[WiFi] Intentando red %d: %s\n", slot, ssid.c_str());
        WiFi.begin(ssid.c_str(), pass.c_str());

        int intentos = 0;
        while (WiFi.status() != WL_CONNECTED && intentos < 20) {
            delay(500);
            Serial.print(".");
            intentos++;
        }
        Serial.println();

        if (WiFi.status() == WL_CONNECTED) {
            Serial.printf("[WiFi] Conectado! IP: %s\n", WiFi.localIP().toString().c_str());
            prefs.end();
            return true;
        }

        WiFi.disconnect();
        delay(200);
    }

    prefs.end();
    return false;
}

// ── Levanta el Access Point del ESP32 ────────────────────────────────────
void iniciarAP() {
    WiFi.softAP("BateriaESPOL", "bateria123");
    Serial.printf("[WiFi] AP activo → SSID: BateriaESPOL  IP: %s\n",
                  WiFi.softAPIP().toString().c_str());
}

// ── Init completo: intenta STA, si falla AP ───────────────────────────────
void wifiInit() {
    WiFi.mode(WIFI_AP_STA);      // permite AP + STA simultáneo si se conecta
    if (!intentarConexion()) {
        WiFi.mode(WIFI_AP);
        iniciarAP();
    }
}

bool wifiConectado() {
    return WiFi.status() == WL_CONNECTED;
}

String ipActual() {
    if (wifiConectado()) return WiFi.localIP().toString();
    return WiFi.softAPIP().toString();
}