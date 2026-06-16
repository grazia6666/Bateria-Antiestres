#ifndef PISTA_OVERCOMPENSATE_H
#define PISTA_OVERCOMPENSATE_H

#include "tipos_juego.h"

// Configuración principal de la canción
const int OVERCOMPENSATE_BPM = 120;
const float CENTER_MASS_BPM = 107.5;
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

// Secuencia representativa: 2 compases del Breakbeat Principal (Compases 5 y 6)
const NotaRitmica pistaOvercompensate[] = {
    
    // --- COMPÁS 1 ---
    // Beat 1: Bombo y Hi-Hat
    {0, 0, false, false}, {2, 0, false, false},
    // Beat 1.75: ¡Bombo adelantado! (Semicorchea antes del 2)
    {0, 375, false, false},
    
    // Beat 2: Caja y Hi-Hat (El golpe fuerte)
    {1, 500, false, false}, {2, 500, false, false},
    // Beat 2.5: Bombo y Hi-Hat
    {0, 750, false, false}, {2, 750, false, false},
    
    // Beat 3.25: Bombo a contratiempo
    {0, 1125, false, false},
    // Beat 3.5: Bombo y Hi-Hat
    {0, 1250, false, false}, {2, 1250, false, false},
    
    // Beat 4: Caja y Hi-Hat
    {1, 1500, false, false}, {2, 1500, false, false},
    // Beat 4.5: Solo Hi-Hat
    {2, 1750, false, false},


    // --- COMPÁS 2 ---
    // Beat 1: Bombo y Hi-Hat
    {0, 2000, false, false}, {2, 2000, false, false},
    // Beat 1.75: Bombo adelantado
    {0, 2375, false, false},
    
    // Beat 2: Caja y Hi-Hat
    {1, 2500, false, false}, {2, 2500, false, false},
    // Beat 2.5: Bombo y Hi-Hat
    {0, 2750, false, false}, {2, 2750, false, false},
    
    // Beat 3.25: Bombo a contratiempo
    {0, 3125, false, false},
    // Beat 3.5: Bombo y Hi-Hat
    {0, 3250, false, false}, {2, 3250, false, false},
    
    // Beat 4: Caja y Hi-Hat
    {1, 3500, false, false}, {2, 3500, false, false},
    // Beat 4.5: Solo Hi-Hat
    {2, 3750, false, false}
};

// El compilador calcula el número total de notas matemáticamente
const int TOTAL_NOTAS_OVERCOMPENSATE = sizeof(pistaOvercompensate) / sizeof(pistaOvercompensate[0]);

#endif