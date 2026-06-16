#ifndef PISTA_SEVEN_NATION_H
#define PISTA_SEVEN_NATION_H

#include "tipos_juego.h"

// Configuración principal de la canción
const int SEVEN_NATION_BPM = 126;
struct NotaRitmica {
    int pad;                  // Qué tambor golpear (0 a 5)
    unsigned long tiempo_ms;  // En qué milisegundo exacto de la pista debe sonar
    bool luzEncendida;        // Flag: ¿El LED ya avisó al jugador?
    bool evaluada;            // Flag: ¿Ya se golpeó o se falló (para no volver a leerla)?
};
// Mapeo según el diseño de tu batería:
// PAD 0 = Bombo (Bass Drum)
// PAD 1 = Caja (Snare)
// PAD 4 = Tom de Piso (Floor Tom)

// Secuencia representativa: 
// 2 compases de Intro (Solo Bombo + Floor Tom)
// 2 compases de Verso (Se añade la Caja en los tiempos 2 y 4)
const NotaRitmica pistaSevenNationArmy[] = {
    
    // --- COMPÁS 1 (Intro) ---
    // Beat 1
    {0, 0, false, false}, {4, 0, false, false},
    // Beat 2
    {0, 476, false, false}, {4, 476, false, false},
    // Beat 3
    {0, 952, false, false}, {4, 952, false, false},
    // Beat 4
    {0, 1428, false, false}, {4, 1428, false, false},

    // --- COMPÁS 2 (Intro) ---
    // Beat 1
    {0, 1904, false, false}, {4, 1904, false, false},
    // Beat 2
    {0, 2380, false, false}, {4, 2380, false, false},
    // Beat 3
    {0, 2857, false, false}, {4, 2857, false, false},
    // Beat 4
    {0, 3333, false, false}, {4, 3333, false, false},

    // --- COMPÁS 3 (Verso: Entra la Caja en tiempos 2 y 4) ---
    // Beat 1
    {0, 3809, false, false}, {4, 3809, false, false},
    // Beat 2 (Añadimos PAD 1)
    {0, 4285, false, false}, {4, 4285, false, false}, {1, 4285, false, false},
    // Beat 3
    {0, 4761, false, false}, {4, 4761, false, false},
    // Beat 4 (Añadimos PAD 1)
    {0, 5238, false, false}, {4, 5238, false, false}, {1, 5238, false, false},

    // --- COMPÁS 4 (Verso) ---
    // Beat 1
    {0, 5714, false, false}, {4, 5714, false, false},
    // Beat 2
    {0, 6190, false, false}, {4, 6190, false, false}, {1, 6190, false, false},
    // Beat 3
    {0, 6666, false, false}, {4, 6666, false, false},
    // Beat 4
    {0, 7142, false, false}, {4, 7142, false, false}, {1, 7142, false, false}
};

// El compilador calcula el número total de notas matemáticamente
const int TOTAL_NOTAS_SEVEN_NATION = sizeof(pistaSevenNationArmy) / sizeof(pistaSevenNationArmy[0]);

#endif