#ifndef PISTA_CENTER_MASS_H
#define PISTA_CENTER_MASS_H

#include "tipos_juego.h"

// Configuración principal de la canción
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

// Secuencia representativa: 2 compases del Groove Principal (Compás 7 y 8)
const NotaRitmica pistaCenterMass[] = {
    
    // --- COMPÁS 1 ---
    // Beat 1: Bombo y Hi-Hat
    {0, 0, false, false}, {2, 0, false, false},
    // Beat 1.5: Bombo y Hi-Hat (Corchea)
    {0, 279, false, false}, {2, 279, false, false},
    // Beat 1.75: ¡Doble Bombo rápido! (Semicorchea adelantada)
    {0, 419, false, false},
    
    // Beat 2: Caja y Hi-Hat
    {1, 558, false, false}, {2, 558, false, false},
    // Beat 2.5: Solo Hi-Hat
    {2, 837, false, false},
    
    // Beat 3: Bombo y Hi-Hat
    {0, 1116, false, false}, {2, 1116, false, false},
    // Beat 3.5: Caja y Hi-Hat
    {1, 1395, false, false}, {2, 1395, false, false},
    
    // Beat 4: Bombo y Hi-Hat
    {0, 1674, false, false}, {2, 1674, false, false},
    // Beat 4.5: Solo Hi-Hat
    {2, 1953, false, false},


    // --- COMPÁS 2 ---
    // Beat 1: Bombo y Hi-Hat
    {0, 2232, false, false}, {2, 2232, false, false},
    // Beat 1.5: Bombo y Hi-Hat
    {0, 2511, false, false}, {2, 2511, false, false},
    // Beat 1.75: ¡Doble Bombo rápido!
    {0, 2651, false, false},
    
    // Beat 2: Caja y Hi-Hat
    {1, 2790, false, false}, {2, 2790, false, false},
    // Beat 2.5: Solo Hi-Hat
    {2, 3069, false, false},
    
    // Beat 3: Bombo y Hi-Hat
    {0, 3348, false, false}, {2, 3348, false, false},
    // Beat 3.5: Caja y Hi-Hat
    {1, 3627, false, false}, {2, 3627, false, false},
    
    // Beat 4: Bombo y Hi-Hat
    {0, 3906, false, false}, {2, 3906, false, false},
    // Beat 4.25: Nota fantasma rápida en la Caja (Semicorchea)
    {1, 4046, false, false},
    // Beat 4.5: Solo Hi-Hat
    {2, 4185, false, false}
};

// El compilador calcula el número total de notas matemáticamente
const int TOTAL_NOTAS_CENTER_MASS = sizeof(pistaCenterMass) / sizeof(pistaCenterMass[0]);

#endif