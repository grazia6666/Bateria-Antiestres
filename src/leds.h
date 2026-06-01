#ifndef LEDS_H
#define LEDS_H

#include <Arduino.h>

void ledsInit(void);
void ledEncender(int pad);
void ledApagar(int pad);
void ledApagarTodos(void);
void ledAnimacionCorrecto(int pad);
void ledAnimacionIncorrecto(int pad);
void ledAnimacionInicio(void);
void ledAnimacionGameOver(void);
void ledSetBrillo(int intensidad);

#endif