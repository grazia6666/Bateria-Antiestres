#ifndef PISTA_SEVEN_NATION_H
#define PISTA_SEVEN_NATION_H

#include "tipos_juego.h"

#define SEVEN_NATION_BPM  126

/* PAD 0 = Bombo · PAD 1 = Caja · PAD 4 = Tom de Piso */

static NotaRitmica pistaSevenNationArmy[] = {
    /* --- COMPAS 1 (Intro) --- */
    {0, 0,    0,0}, {4, 0,    0,0},   /* Beat 1 */
    {0, 476,  0,0}, {4, 476,  0,0},   /* Beat 2 */
    {0, 952,  0,0}, {4, 952,  0,0},   /* Beat 3 */
    {0, 1428, 0,0}, {4, 1428, 0,0},   /* Beat 4 */
    /* --- COMPAS 2 (Intro) --- */
    {0, 1904, 0,0}, {4, 1904, 0,0},
    {0, 2380, 0,0}, {4, 2380, 0,0},
    {0, 2857, 0,0}, {4, 2857, 0,0},
    {0, 3333, 0,0}, {4, 3333, 0,0},
    /* --- COMPAS 3 (Verso: entra Caja en tiempos 2 y 4) --- */
    {0, 3809, 0,0}, {4, 3809, 0,0},
    {0, 4285, 0,0}, {4, 4285, 0,0}, {1, 4285, 0,0},
    {0, 4761, 0,0}, {4, 4761, 0,0},
    {0, 5238, 0,0}, {4, 5238, 0,0}, {1, 5238, 0,0},
    /* --- COMPAS 4 (Verso) --- */
    {0, 5714, 0,0}, {4, 5714, 0,0},
    {0, 6190, 0,0}, {4, 6190, 0,0}, {1, 6190, 0,0},
    {0, 6666, 0,0}, {4, 6666, 0,0},
    {0, 7142, 0,0}, {4, 7142, 0,0}, {1, 7142, 0,0}
};

#define TOTAL_NOTAS_SEVEN_NATION \
    (int)(sizeof(pistaSevenNationArmy) / sizeof(pistaSevenNationArmy[0]))

#endif