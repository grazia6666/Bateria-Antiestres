#ifndef PADS_H
#define PADS_H
 
#include <Arduino.h>
 
#define NUM_PADS 6
 
typedef struct {
    int pad;         /* 0-5, -1 = ningun golpe */
    int intensidad;  /* 0-4095 ADC 12 bits     */
} GolpePad;
 
void     padsInit(void);
GolpePad leerGolpe(void);
 
#endif
 