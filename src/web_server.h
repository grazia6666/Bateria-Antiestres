#pragma once
#include <Arduino.h>

void servidorInit();
void wsBroadcast(const String& json);   // enviar a todos los clientes conectados