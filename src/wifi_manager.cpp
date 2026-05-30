#include "wifi_manager.h"
#include <EEPROM.h>
#include <WiFi.h>

// ── Mapa de memoria EEPROM ────────────────────────────────────────────────
//   Slot 0: SSID en dirección   0  (hasta 32 chars)
//           PASS en dirección  32  (hasta 64 chars)
//   Slot 1: SSID en dirección  96  (hasta 32 chars)
//           PASS en dirección 128  (hasta 64 chars)
//   Tamaño total reservado: 192 bytes
#define EEPROM_SIZE     192
#define SSID_MAX_LEN     32
#define PASS_MAX_LEN     64
#define SLOT0_SSID_ADDR   0
#define SLOT0_PASS_ADDR  32
#define SLOT1_SSID_ADDR  96
#define SLOT1_PASS_ADDR 128

// ── Helpers internos ──────────────────────────────────────────────────────
static void escribirString(int direccion, const char* texto, int maxLen) {
    int len = strlen(texto);
    if (len > maxLen - 1) len = maxLen - 1;
    for (int i = 0; i < len; i++) {
        EEPROM.write(direccion + i, texto[i]);
    }
    EEPROM.write(direccion + len, '\0');   // null terminator
    EEPROM.commit();
}

static String leerString(int direccion, int maxLen) {
    String resultado = "";
    for (int i = 0; i < maxLen; i++) {
        char c = (char)EEPROM.read(direccion + i);
        if (c == '\0') break;
        // Ignorar bytes sin inicializar (0xFF)
        if ((uint8_t)c == 0xFF) break;
        resultado += c;
    }
    return resultado;
}

// ── Guarda credenciales en EEPROM (slot 0 o 1) ───────────────────────────
void guardarRed(int slot, const char* ssid, const char* pass) {
    EEPROM.begin(EEPROM_SIZE);

    int addrSSID = (slot == 0) ? SLOT0_SSID_ADDR : SLOT1_SSID_ADDR;
    int addrPASS = (slot == 0) ? SLOT0_PASS_ADDR : SLOT1_PASS_ADDR;

    escribirString(addrSSID, ssid, SSID_MAX_LEN);
    escribirString(addrPASS, pass, PASS_MAX_LEN);

    EEPROM.end();
    Serial.printf("[WiFi] Red %d guardada en EEPROM: %s\n", slot, ssid);
}

// ── Intenta conectar a las 2 redes guardadas en EEPROM ───────────────────
bool intentarConexion() {
    EEPROM.begin(EEPROM_SIZE);

    int addrsSSID[2] = { SLOT0_SSID_ADDR, SLOT1_SSID_ADDR };
    int addrsPASS[2] = { SLOT0_PASS_ADDR, SLOT1_PASS_ADDR };

    for (int slot = 0; slot < 2; slot++) {
        String ssid = leerString(addrsSSID[slot], SSID_MAX_LEN);
        String pass = leerString(addrsPASS[slot], PASS_MAX_LEN);

        if (ssid.length() == 0) {
            Serial.printf("[WiFi] Slot %d vacío, saltando\n", slot);
            continue;
        }

        Serial.printf("[WiFi] Intentando slot %d: %s\n", slot, ssid.c_str());
        WiFi.begin(ssid.c_str(), pass.c_str());

        int intentos = 0;
        while (WiFi.status() != WL_CONNECTED && intentos < 10) {
            delay(500);
            Serial.print(".");
            intentos++;
        }
        Serial.println();

        if (WiFi.status() == WL_CONNECTED) {
            Serial.printf("[WiFi] Conectado! IP: %s\n", WiFi.localIP().toString().c_str());
            EEPROM.end();
            return true;
        }

        WiFi.disconnect();
        delay(300);
    }

    EEPROM.end();
    return false;
}

// ── Levanta el Access Point del ESP32 ────────────────────────────────────
void iniciarAP() {
    WiFi.softAP("BateriaESPOL", "bateria123");
    Serial.printf("[WiFi] AP activo → SSID: BateriaESPOL  IP: %s\n",
                  WiFi.softAPIP().toString().c_str());
}

// ── Init completo: intenta STA, si falla levanta AP ──────────────────────
void wifiInit() {
    WiFi.mode(WIFI_AP_STA);
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