#ifndef PISTA_CAMISA_NEGRA_H
#define PISTA_CAMISA_NEGRA_H

#include "tipos_juego.h"

// Configuración principal de la canción
const int CAMISA_NEGRA_BPM = 97;
struct NotaRitmica {
    int pad;                  // Qué tambor golpear (0 a 5)
    unsigned long tiempo_ms;  // En qué milisegundo exacto de la pista debe sonar
    bool luzEncendida;        // Flag: ¿El LED ya avisó al jugador?
    bool evaluada;            // Flag: ¿Ya se golpeó o se falló (para no volver a leerla)?
};
// Mapeo según el diseño de tu batería:
// PAD 0 = Bombo (Bass Drum)
// PAD 1 = Caja (Snare)
// PAD 2 = Hi-Hat 

// Secuencia representativa: 2 compases de la Sección A (El ritmo principal Guasca/Pop)
const NotaRitmica pistaCamisaNegra[] = {
    
    // --- COMPÁS 1 ---
    // Beat 1: Bombo y Hi-Hat
    {0, 0, false, false}, {2, 0, false, false},
    // Beat 1.5: Solo Hi-Hat
    {2, 309, false, false},
    // Beat 2: Caja y Hi-Hat
    {1, 619, false, false}, {2, 619, false, false},
    // Beat 2.5: ¡CONTRATIEMPO! Bombo y Hi-Hat adelantado
    {0, 928, false, false}, {2, 928, false, false},
    // Beat 3: Bombo y Hi-Hat
    {0, 1237, false, false}, {2, 1237, false, false},
    // Beat 3.5: Solo Hi-Hat
    {2, 1546, false, false},
    // Beat 4: Caja y Hi-Hat
    {1, 1856, false, false}, {2, 1856, false, false},
    // Beat 4.5: Solo Hi-Hat
    {2, 2165, false, false},

    // --- COMPÁS 2 ---
    // Beat 1: Bombo y Hi-Hat
    {0, 2474, false, false}, {2, 2474, false, false},
    // Beat 1.5: Solo Hi-Hat
    {2, 2783, false, false},
    // Beat 2: Caja y Hi-Hat
    {1, 3093, false, false}, {2, 3093, false, false},
    // Beat 2.5: ¡CONTRATIEMPO! Bombo y Hi-Hat
    {0, 3402, false, false}, {2, 3402, false, false},
    // Beat 3: Bombo y Hi-Hat
    {0, 3711, false, false}, {2, 3711, false, false},
    // Beat 3.5: Solo Hi-Hat
    {2, 4020, false, false},
    // Beat 4: Caja y Hi-Hat
    {1, 4330, false, false}, {2, 4330, false, false},
    // Beat 4.5: Solo Hi-Hat
    {2, 4639, false, false}
};

// El compilador calcula el número total de notas matemáticamente
const int TOTAL_NOTAS_CAMISA_NEGRA = sizeof(pistaCamisaNegra) / sizeof(pistaCamisaNegra[0]);

#endif