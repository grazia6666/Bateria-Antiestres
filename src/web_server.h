#ifndef WEB_SERVER_H
#define WEB_SERVER_H
 
#include <Arduino.h>
 
void servidorInit(void);
void wsBroadcast(const char* json);
void procesarAccionesPendientes(void); /* llamar desde loop(), NO desde el callback de WS */
 
#endif