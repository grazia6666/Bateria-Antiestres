#ifndef PISTA_CAMISA_NEGRA_H
#define PISTA_CAMISA_NEGRA_H

#include "tipos_juego.h"

#define CAMISA_NEGRA_BPM  97

/* PAD 0 = Bombo · PAD 1 = Caja · PAD 2 = Hi-Hat */

static NotaRitmica pistaCamisaNegra[] = {
    /* --- COMPAS 1 --- */
    {0, 0,    0,0}, {2, 0,    0,0},   /* Beat 1   : Bombo + Hi-Hat     */
    {2, 309,  0,0},                   /* Beat 1.5 : Hi-Hat             */
    {1, 619,  0,0}, {2, 619,  0,0},   /* Beat 2   : Caja + Hi-Hat      */
    {0, 928,  0,0}, {2, 928,  0,0},   /* Beat 2.5 : Contratiempo Bombo */
    {0, 1237, 0,0}, {2, 1237, 0,0},   /* Beat 3   : Bombo + Hi-Hat     */
    {2, 1546, 0,0},                   /* Beat 3.5 : Hi-Hat             */
    {1, 1856, 0,0}, {2, 1856, 0,0},   /* Beat 4   : Caja + Hi-Hat      */
    {2, 2165, 0,0},                   /* Beat 4.5 : Hi-Hat             */
    /* --- COMPAS 2 --- */
    {0, 2474, 0,0}, {2, 2474, 0,0},
    {2, 2783, 0,0},
    {1, 3093, 0,0}, {2, 3093, 0,0},
    {0, 3402, 0,0}, {2, 3402, 0,0},
    {0, 3711, 0,0}, {2, 3711, 0,0},
    {2, 4020, 0,0}, 
    {1, 4330, 0,0}, {2, 4330, 0,0},
    {2, 4639, 0,0}
};

#define TOTAL_NOTAS_CAMISA_NEGRA \
    (int)(sizeof(pistaCamisaNegra) / sizeof(pistaCamisaNegra[0]))

#endif