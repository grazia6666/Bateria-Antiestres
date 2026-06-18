#ifndef PISTA_CENTER_MASS_H
#define PISTA_CENTER_MASS_H

#include "tipos_juego.h"

#define CENTER_MASS_BPM  107   /* 107.5 redondeado */

/* PAD 0 = Bombo · PAD 1 = Caja · PAD 2 = Hi-Hat */

static NotaRitmica pistaCenterMass[] = {
    /* --- COMPAS 1 --- */
    {0, 0,    0,0}, {2, 0,    0,0},   /* Beat 1    : Bombo + Hi-Hat      */
    {0, 279,  0,0}, {2, 279,  0,0},   /* Beat 1.5  : Bombo + Hi-Hat      */
    {0, 419,  0,0},                   /* Beat 1.75 : Doble Bombo rapido  */
    {1, 558,  0,0}, {2, 558,  0,0},   /* Beat 2    : Caja + Hi-Hat       */
    {2, 837,  0,0},                   /* Beat 2.5  : Hi-Hat              */
    {0, 1116, 0,0}, {2, 1116, 0,0},   /* Beat 3    : Bombo + Hi-Hat      */
    {1, 1395, 0,0}, {2, 1395, 0,0},   /* Beat 3.5  : Caja + Hi-Hat       */
    {0, 1674, 0,0}, {2, 1674, 0,0},   /* Beat 4    : Bombo + Hi-Hat      */
    {2, 1953, 0,0},                   /* Beat 4.5  : Hi-Hat              */
    /* --- COMPAS 2 --- */
    {0, 2232, 0,0}, {2, 2232, 0,0},
    {0, 2511, 0,0}, {2, 2511, 0,0},
    {0, 2651, 0,0},
    {1, 2790, 0,0}, {2, 2790, 0,0},
    {2, 3069, 0,0},
    {0, 3348, 0,0}, {2, 3348, 0,0},
    {1, 3627, 0,0}, {2, 3627, 0,0},
    {0, 3906, 0,0}, {2, 3906, 0,0},
    {1, 4046, 0,0},
    {2, 4185, 0,0}
};

#define TOTAL_NOTAS_CENTER_MASS \
    (int)(sizeof(pistaCenterMass) / sizeof(pistaCenterMass[0]))

#endif