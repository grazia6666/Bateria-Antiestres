#pragma once
#include <Arduino.h>
 
void     wifiInit();          // intenta redes guardadas, si falla levanta AP
void     guardarRed(int slot, const char* ssid, const char* pass);
bool     intentarConexion();
void     iniciarAP();
bool     wifiConectado();
String   ipActual();
 