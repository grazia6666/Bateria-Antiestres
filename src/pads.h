#pragma once
#include <Arduino.h>
 
#define NUM_PADS 6
 
struct GolpePad {
    int  pad;         // 0-5, -1 = ninguno
    int  intensidad;  // 0-4095 (ADC 12 bits)
};
 
void      padsInit();
GolpePad  leerGolpe();   // retorna el pad golpeado; pad=-1 si no hay golpe