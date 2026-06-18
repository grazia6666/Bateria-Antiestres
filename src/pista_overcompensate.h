#ifndef PISTA_OVERCOMPENSATE_H
#define PISTA_OVERCOMPENSATE_H

#include "tipos_juego.h"

#define OVERCOMPENSATE_BPM  120

/* PAD 0 = Bombo · PAD 1 = Caja · PAD 2 = Hi-Hat */

static NotaRitmica pistaOvercompensate[] = {
    /* --- COMPAS 1 --- */
    {0, 0,    0,0}, {2, 0,    0,0},   /* Beat 1    : Bombo + Hi-Hat      */
    {0, 375,  0,0},                   /* Beat 1.75 : Bombo adelantado    */
    {1, 500,  0,0}, {2, 500,  0,0},   /* Beat 2    : Caja + Hi-Hat       */
    {0, 750,  0,0}, {2, 750,  0,0},   /* Beat 2.5  : Bombo + Hi-Hat      */
    {0, 1125, 0,0},                   /* Beat 3.25 : Bombo a contratiempo*/
    {0, 1250, 0,0}, {2, 1250, 0,0},   /* Beat 3.5  : Bombo + Hi-Hat      */
    {1, 1500, 0,0}, {2, 1500, 0,0},   /* Beat 4    : Caja + Hi-Hat       */
    {2, 1750, 0,0},                   /* Beat 4.5  : Hi-Hat              */
    /* --- COMPAS 2 --- */
    {0, 2000, 0,0}, {2, 2000, 0,0},
    {0, 2375, 0,0},
    {1, 2500, 0,0}, {2, 2500, 0,0},
    {0, 2750, 0,0}, {2, 2750, 0,0},
    {0, 3125, 0,0},
    {0, 3250, 0,0}, {2, 3250, 0,0},
    {1, 3500, 0,0}, {2, 3500, 0,0},
    {2, 3750, 0,0}
};

#define TOTAL_NOTAS_OVERCOMPENSATE \
    (int)(sizeof(pistaOvercompensate) / sizeof(pistaOvercompensate[0]))

#endif