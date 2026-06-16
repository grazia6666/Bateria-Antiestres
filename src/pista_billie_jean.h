// Estructura de cada nota en el tiempo
struct NotaRitmica {
    int pad;                  // Qué tambor golpear (0 a 5)
    unsigned long tiempo_ms;  // En qué milisegundo exacto de la pista debe sonar
    bool luzEncendida;        // Flag: ¿El LED ya avisó al jugador?
    bool evaluada;            // Flag: ¿Ya se golpeó o se falló (para no volver a leerla)?
};

const int NUM_NOTAS_BILLIE = 17; 
NotaRitmica pistaBillieJean[NUM_NOTAS_BILLIE] = {
    // --- COMPÁS 1 --- (Todos los flags inician en 'false')
    {0, 0, false, false}, {2, 0, false, false},       // Beat 1: Bombo y Hi-Hat
    {2, 256, false, false},                           // Beat 1.5: Hi-Hat
    {1, 513, false, false}, {2, 513, false, false},   // Beat 2: Caja y Hi-Hat
    {2, 769, false, false},                           // Beat 2.5: Hi-Hat
    {0, 1026, false, false}, {2, 1026, false, false}, // Beat 3: Bombo y Hi-Hat
    {2, 1282, false, false},                          // Beat 3.5: Hi-Hat
    {1, 1538, false, false}, {2, 1538, false, false}, // Beat 4: Caja y Hi-Hat
    {2, 1795, false, false},                          // Beat 4.5: Hi-Hat
    // --- COMPÁS 2 ---
    {0, 2051, false, false}, {2, 2051, false, false},
    {2, 2308, false, false}, 
    {1, 2564, false, false}, {2, 2564, false, false}
};

// Variables de sintonía del juego (Ajustables)
#define ANTICIPACION_VISUAL 300 // Milisegundos antes del golpe para encender el LED
#define VENTANA_TOLERANCIA 150  // ±150ms de margen de error para que cuente como "Perfecto" o "Bien"