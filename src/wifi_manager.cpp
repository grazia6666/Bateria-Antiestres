#include "wifi_manager.h"
#include <EEPROM.h>
#include <WiFi.h>

#define EEPROM_SIZE      192
#define SSID_MAX_LEN      32
#define PASS_MAX_LEN      64
#define SLOT0_SSID_ADDR    0
#define SLOT0_PASS_ADDR   32
#define SLOT1_SSID_ADDR   96
#define SLOT1_PASS_ADDR  128

/* helpers internos */
static void eeprom_write_str(int addr, const char* str, int maxLen) {
    int i = 0;
    while (str[i] != '\0' && i < maxLen - 1) {
        EEPROM.write(addr + i, (uint8_t)str[i]);
        i++;
    }
    EEPROM.write(addr + i, '\0');
    EEPROM.commit();
}

static void eeprom_read_str(int addr, char* buf, int maxLen) {
    int i = 0;
    uint8_t c;
    while (i < maxLen - 1) {
        c = EEPROM.read(addr + i);
        if (c == '\0' || c == 0xFF) break;
        buf[i] = (char)c;
        i++;
    }
    buf[i] = '\0';
}

/*  */
void guardarRed(int slot, const char* ssid, const char* pass) {
    EEPROM.begin(EEPROM_SIZE);
    int addrSSID = (slot == 0) ? SLOT0_SSID_ADDR : SLOT1_SSID_ADDR;
    int addrPASS = (slot == 0) ? SLOT0_PASS_ADDR : SLOT1_PASS_ADDR;
    eeprom_write_str(addrSSID, ssid, SSID_MAX_LEN);
    eeprom_write_str(addrPASS, pass, PASS_MAX_LEN);
    EEPROM.end();
    Serial.printf("[WiFi] Red %d guardada: %s\n", slot, ssid);
}

int intentarConexion(void) {
    char ssid[SSID_MAX_LEN];
    char pass[PASS_MAX_LEN];
    int addrsSSID[2] = { SLOT0_SSID_ADDR, SLOT1_SSID_ADDR };
    int addrsPASS[2] = { SLOT0_PASS_ADDR, SLOT1_PASS_ADDR };
    int slot, intentos;

    EEPROM.begin(EEPROM_SIZE);

    for (slot = 0; slot < 2; slot++) {
        eeprom_read_str(addrsSSID[slot], ssid, SSID_MAX_LEN);
        eeprom_read_str(addrsPASS[slot], pass, PASS_MAX_LEN);

        if (ssid[0] == '\0') {
            Serial.printf("[WiFi] Slot %d vacio\n", slot);
            continue;
        }

        Serial.printf("[WiFi] Intentando slot %d: %s\n", slot, ssid);
        WiFi.begin(ssid, pass);

        intentos = 0;
        while (WiFi.status() != WL_CONNECTED && intentos < 10) {
            delay(500);
            Serial.print(".");
            intentos++;
        }
        Serial.println();

        if (WiFi.status() == WL_CONNECTED) {
            Serial.printf("[WiFi] Conectado! IP: %s\n",
                          WiFi.localIP().toString().c_str());
            EEPROM.end();
            return 1;
        }
        WiFi.disconnect();
        delay(300);
    }

    EEPROM.end();
    return 0;
}

void iniciarAP(void) {
    WiFi.softAP("BateriaESPOL", "bateria123");
    Serial.printf("[WiFi] AP activo  IP: %s\n",
                  WiFi.softAPIP().toString().c_str());
}

void wifiInit(void) {
    WiFi.mode(WIFI_AP_STA);
    if (!intentarConexion()) {
        WiFi.mode(WIFI_AP);
        iniciarAP();
    }
}

int wifiConectado(void) {
    return (WiFi.status() == WL_CONNECTED) ? 1 : 0;
}

void ipActual(char* buf, int bufLen) {
    const char* ip;
    if (wifiConectado())
        ip = WiFi.localIP().toString().c_str();
    else
        ip = WiFi.softAPIP().toString().c_str();
    strncpy(buf, ip, bufLen - 1);
    buf[bufLen - 1] = '\0';
}