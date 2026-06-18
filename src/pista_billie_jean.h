#ifndef PISTA_BILLIE_JEAN_H
#define PISTA_BILLIE_JEAN_H

#include "tipos_juego.h"

#define BILLIE_JEAN_BPM  117

/* PAD 0 = Bombo · PAD 1 = Caja · PAD 2 = Hi-Hat */

static NotaRitmica pistaBillieJean[] = {
    /* --- COMPAS 1 --- */
    {0, 0,    0,0}, {2, 0,    0,0},   /* Beat 1   : Bombo + Hi-Hat */
    {2, 256,  0,0},                   /* Beat 1.5 : Hi-Hat          */
    {1, 513,  0,0}, {2, 513,  0,0},   /* Beat 2   : Caja + Hi-Hat   */
    {2, 769,  0,0},                   /* Beat 2.5 : Hi-Hat          */
    {0, 1026, 0,0}, {2, 1026, 0,0},   /* Beat 3   : Bombo + Hi-Hat  */
    {2, 1282, 0,0},                   /* Beat 3.5 : Hi-Hat          */
    {1, 1538, 0,0}, {2, 1538, 0,0},   /* Beat 4   : Caja + Hi-Hat   */
    {2, 1795, 0,0},                   /* Beat 4.5 : Hi-Hat          */
    /* --- COMPAS 2 --- */
    {0, 2051, 0,0}, {2, 2051, 0,0},
    {2, 2308, 0,0},
    {1, 2564, 0,0}, {2, 2564, 0,0}
};

#define TOTAL_NOTAS_BILLIE_JEAN \
    (int)(sizeof(pistaBillieJean) / sizeof(pistaBillieJean[0]))

#endif