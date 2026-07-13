#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H
 
#include <Arduino.h>
 
void wifiInit(void);  /*intenta conectarse a la guarda si no el mismo ap*/
void guardarRed(int slot, const char* ssid, const char* pass);
int  intentarConexion(void);   /* retorna 1=conectado 0=fallo */
void iniciarAP(void);
int  wifiConectado(void);      /* retorna 1=si 0=no */
void ipActual(char* buf, int bufLen);
 
#endif
 