#ifndef TIPOS_JUEGO_H
#define TIPOS_JUEGO_H
 
/* ── Estructura de cada nota en el tiempo ────────────────────────
   pad         : cual pad golpear (0-5)
   tiempo_ms   : en que milisegundo exacto de la pista debe sonar
   luzEncendida: flag — el LED ya aviso al jugador?
   evaluada    : flag — ya se golpeo o fallo?
──────────────────────────────────────────────────────────────── */
typedef struct {
    int           pad;
    unsigned long tiempo_ms;
    int           luzEncendida;   /* 0=no 1=si  (bool en C nativo) */
    int           evaluada;       /* 0=no 1=si */
} NotaRitmica;
 
/* ── Resultado de evaluar un golpe ────────────────────────────── */
#define EVAL_PERFECTO  2   /* dentro de VENTANA_TOLERANCIA        */
#define EVAL_BIEN      1   /* dentro de VENTANA_TOLERANCIA*2      */
#define EVAL_MISS      0   /* fuera de ventana o pad incorrecto   */
 
/* ── Constantes globales de juego ─────────────────────────────── */
#define ANTICIPACION_VISUAL  300   /* ms antes del golpe: enciende LED  */
#define VENTANA_TOLERANCIA   150   /* ±ms margen para contar como acierto*/
 
#endif
 