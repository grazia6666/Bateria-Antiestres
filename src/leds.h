#pragma once
#include <Arduino.h>
 
void ledsInit();
void ledEncender(int pad);                          // color del pad
void ledApagar(int pad);
void ledApagarTodos();
void ledAnimacionCorrecto(int pad);                 // destello verde
void ledAnimacionIncorrecto(int pad);               // destello rojo
void ledAnimacionInicio();                          // barrido arcoíris
void ledAnimacionGameOver();                        // pulso rojo
void ledSetBrillo(int intensidad);                  // 0-4095 → 0-255
 