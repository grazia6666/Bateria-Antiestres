# 🥁 Batería Anti-Estrés Politécnico

**Proyecto de Sistemas Embebidos — ESPOL**

Estudiantes:  Maria Bravo (Telemática) · Daniel Espinoza (Mecatrónica) 
Paralelo 101 · Ing. Karen Thamara Torres Medina
---

## Descripción

Batería electrónica interactiva de bajo costo construida sobre un **ESP32**, con formato de máquina arcade, ahora ensamblada sobre **PCB**. El sistema detecta golpes en 5 pads físicos mediante sensores piezoeléctricos, genera respuesta luminosa (tira WS2812B de 52 LEDs), sonora (**dos** módulos DFPlayer Mini independientes) y visual (OLED SSD1306). Incluye **cuatro modos de juego** (Libre, Reflejos, Memoria y Canción/Ritmo) y un sistema IoT donde los jugadores se conectan desde su celular vía WiFi para registrarse, jugar y ver el ranking en tiempo real. Enfocado a un público amplio — niños en edad escolar, estudiantes y jóvenes — con el objetivo de estimular coordinación motriz, memoria, tiempo de reacción y reducción del estrés.

---

## Características principales

- 5 sensores piezoeléctricos (pad 6 actualmente **deshabilitado por software** — ver *Problemas conocidos*)
- Tira de **52 LEDs WS2812B**, repartidos en 5 secciones de distinto tamaño (una por pad)
- **Dos** DFPlayer Mini independientes: uno para los sonidos de golpe de cada pad, otro para las pistas de fondo del modo Canción
- Pantalla OLED 128×64 que indica qué pad tocar
- **4 modos de juego:** Libre, Reflejos, Memoria y Canción (ritmo musical sincronizado con LEDs)
- 3 canciones activas: *Billie Jean*, *We Will Rock You* y *Seven Nation Army*
- Umbral de detección de golpe **ajustable de forma independiente por pad**, no un solo valor global
- WiFi con reconexión automática a 2 redes guardadas (EEPROM) y modo Access Point de respaldo si ninguna está disponible
- Dashboard web (interfaz clara, con la paleta de los 5 LEDs físicos como lenguaje visual) accesible desde cualquier celular
- Ranking en tiempo real vía WebSocket, persistente en LittleFS
- Arquitectura sin condiciones de carrera entre el WebSocket y la lógica del juego (cola de acciones pendientes procesada desde un único hilo)

---

## Hardware

### Lista de componentes

| Componente | Cantidad | Función |
|---|---|---|
| ESP32  | 1 | Microcontrolador principal |
| Sensor piezoeléctrico | 6 (1 deshabilitado) | Detectar golpes en los pads |
| Tira LED WS2812B | 1 (52 LEDs) | Iluminación RGB por pad |
| DFPlayer Mini | 2 | Audio de pads + pistas de fondo |
| Parlante 8Ω | 2 | Emisión de sonido (uno por DFPlayer) |
| OLED SSD1306 128×64 | 1 | Pantalla indicadora I2C |
| Resistencia 1kΩ | 2 | Protección línea TX → cada DFPlayer |
| Fuente externa 5V | 1 | Alimentación del sistema completo |


---

## Mapa de pines ESP32

| GPIO | Función | Bloque |
|---|---|---|
| 4 | Datos tira LED WS2812B | — |
| 14 | TX DFPlayer 2 (pistas de fondo) | UART2 |
| 16 | RX DFPlayer 1 (sonidos de pads) | UART1 |
| 17 | TX DFPlayer 1 | UART1 |
| 21 | SDA OLED | I2C |
| 22 | SCL OLED | I2C |
| 27 | RX DFPlayer 2 | UART2 |
| 32 | PAD 3 | ADC1 |
| 33 | PAD 4 | ADC1 |
| 34 | PAD 1 | ADC1 |
| 35 | PAD 2 | ADC1 |
| 36 | PAD 5 | ADC1 |
| 39 | PAD 6 — **deshabilitado** | ADC1 |

Todos los sensores piezoeléctricos están en pines **ADC1**, deliberadamente aislados del ADC2 (que no es confiable mientras el WiFi está activo). Los pines **GPIO 34, 35, 36 y 39 son input-only y no tienen resistencia pull-down interna** — necesitan resistencia externa a GND para no flotar y dar falsos positivos.



### OLED SSD1306 — I2C

```
VCC ── 3.3V (regulador interno ESP32)
GND ── GND
SDA ── GPIO 21
SCL ── GPIO 22
Dirección I2C: 0x3C
```

### DFPlayer 1 — sonidos de pads (UART1)

```
VCC  ── 5V
GND  ── GND
RX   ── GPIO 16  (ESP32 recibe)
TX   ── GPIO 17  (ESP32 envía, con resistencia 1kΩ en serie)
SD con 0001-0006.mp3 en la raíz (bombo, caja, hi-hat, tom grave, tom agudo, crash)
```

### DFPlayer 2 — pistas de fondo (UART2)

```
VCC  ── 5V
GND  ── GND
RX   ── GPIO 27  (ESP32 recibe)
TX   ── GPIO 14  (ESP32 envía, con resistencia 1kΩ en serie)
SD con 0001-0005.mp3 en la raíz (canciones)
```

### Tira LED WS2812B

```
VCC ── 5V
GND ── GND
DIN ── GPIO 4
52 LEDs repartidos en 6 secciones de distinto tamaño (ver LEDS_POR_PAD en leds.cpp)
```

> ⚠️ Verificar en `leds.cpp` que la suma de `LEDS_POR_PAD[]` coincida exactamente con `NUM_LEDS` de `platformio.ini` — un desajuste no corrompe memoria (hay clamps de seguridad) pero deja LEDs sin asignar o secciones más cortas de lo esperado.

---

## Software

### Entorno de desarrollo

 **IDE:** Visual Studio Code + PlatformIO
 **Framework:** Arduino para ESP32
 **Sistema de archivos:** LittleFS

### Estructura del proyecto

```
bateria-antistress/
├── platformio.ini
├── data/
│   ├── index.html        ← dashboard web
│   └── scores.json       ← ranking persistente
└── src/
    ├── main.cpp             ← setup() + loop()
    ├── tipos_juego.h        ← struct NotaRitmica, constantes de timing
    ├── wifi_manager.h/cpp   ← STA (2 redes en EEPROM) + fallback AP
    ├── oled_display.h/cpp   ← pantallas OLED del juego
    ├── pads.h/cpp           ← ADC piezoeléctricos, debounce, umbral por pad
    ├── leds.h/cpp           ← WS2812B FastLED (52 LEDs, secciones desiguales)
    ├── audio.h/cpp          ← dos DFPlayer Mini (pads + pistas)
    ├── scores.h/cpp         ← LittleFS JSON top 20
    ├── web_server.h/cpp     ← AsyncWebServer + WebSocket + cola de acciones
    ├── game_modes.h/cpp     ← lógica de Libre, Reflejos y Memoria
    ├── modo_cancion.h/cpp   ← lógica del modo Canción (ritmo musical)
    ├── pista_billie_jean.h        ← notas de Billie Jean (activa)
    ├── pista_seven_nation.h       ← notas de Seven Nation Army (activa)
    ├── pista_we_will_rock_you.h   ← notas de We Will Rock You (activa)
    ├── pista_camisa_negra.h       ← pendiente (no incluida en la tabla)
    └── pista_overcompensate.h     ← pendiente (no incluida en la tabla)
```

### Librerías utilizadas

| Librería | Uso |
|---|---|
| ESPAsyncWebServer + AsyncTCP | Servidor web y WebSocket asíncrono |
| ArduinoJson | Serialización JSON |
| FastLED | Control tira WS2812B |
| DFRobotDFPlayerMini | Control de los dos módulos de audio |
| Adafruit SSD1306 + GFX | Driver y gráficos OLED |
| LittleFS | Sistema de archivos (dashboard + scores) |
| EEPROM | Persistencia de credenciales WiFi |

### Parámetros configurables en `platformio.ini`

| Parámetro | Valor | Descripción |
|---|---|---|
| `PIN_PAD0`…`PIN_PAD5` | 34,35,32,33,36,39 | Piezoeléctricos, ADC1 |
| `PIN_LEDS` | 4 | Data tira WS2812B |
| `NUM_LEDS` | 52 | Cantidad total de LEDs de la tira (**verificar que coincida con el conteo físico real**) |
| `OLED_SDA` / `OLED_SCL` | 21 / 22 | I2C OLED |
| `PIN_DFP1_RX` / `PIN_DFP1_TX` | 16 / 17 | UART1 — DFPlayer de pads |
| `PIN_DFP2_RX` / `PIN_DFP2_TX` | 27 / 14 | UART2 — DFPlayer de pistas |
| `PIEZO_UMBRAL` | 300 | Umbral ADC por defecto (0-4095) — cada pad puede tener su propio valor, ver `UMBRALES[]` en `pads.cpp` |
| `TIEMPO_REFLEJO_MS` | 1500 | Tiempo de reacción en modo Reflejos |

### Ajuste de sensibilidad por pad

`pads.cpp` define un arreglo `UMBRALES[NUM_PADS]` — cada pad tiene su propio umbral de detección, independiente de los demás. Útil porque no todos los GPIO se comportan igual: los pines input-only (34, 35, 36, 39) no tienen pull-down interno y son más sensibles a ruido de fondo, y GPIO32/33 comparten circuitería con el oscilador RTC de 32.768kHz de la placa, lo que les da un piso de ruido más alto y estable incluso sin nada conectado. Para calibrar, mirar los valores `[PAD DEBUG] GPIOxx = ...` que imprime el monitor serie y ajustar el número de la línea correspondiente en `UMBRALES[]`.

### Deshabilitar/rehabilitar un pad por software

`pads.cpp` tiene `#define PAD_DESHABILITADO N` (índice 0-5, actualmente `5` = pad 6/GPIO39). El pad deshabilitado se ignora por completo en la detección (`leerGolpe()`) y en la selección aleatoria de objetivos de Reflejos/Memoria (`NUM_PADS_ACTIVOS` en `game_modes.cpp`). Para rehabilitarlo, poner `PAD_DESHABILITADO -1` y `NUM_PADS_ACTIVOS = NUM_PADS`.

### Mapa de memoria EEPROM (WiFi)

```
Dirección   0 →  SSID red 1  (32 bytes)
Dirección  32 →  PASS red 1  (64 bytes)
Dirección  96 →  SSID red 2  (32 bytes)
Dirección 128 →  PASS red 2  (64 bytes)
─────────────────────────────────────────
Total reservado: 192 bytes
```

---

## Modos de juego

### Flujo general

```
Celular → Login + selección de modo
       → WS: {accion: iniciar_juego / iniciar_cancion}
       → El callback de WS solo deja la accion pendiente (no ejecuta directo)
       → loop() la procesa en el hilo principal (evita condiciones de carrera)
       → OLED / LEDs indican qué pad tocar
       → Jugador golpea el pad físico
       → Piezo detecta intensidad (umbral por pad)
       → ESP32 valida, LED responde, audio suena
       → WebSocket → celular actualiza score en tiempo real
```

### 🎵 Modo Libre
Toca cualquier pad libremente. Puntos proporcionales a la intensidad del golpe. Sin límite de tiempo ni vidas.

### ⚡ Modo Reflejos
ESP32 elige un pad al azar (excluyendo el pad deshabilitado) → LED enciende → hay `TIEMPO_REFLEJO_MS` para golpearlo. Acierto: `100 × combo` puntos. Fallo o tiempo agotado: pierde una vida, combo reinicia. Combo sube cada 5 aciertos consecutivos (máx. x8). 3 vidas.

### 🧠 Modo Memoria
La secuencia crece cada ronda (1 pad → hasta 12). El OLED/LEDs muestran la secuencia, luego el turno del jugador. Acierto completo de ronda: `150 × ronda × combo` puntos. Error: pierde una vida y repite la ronda. 3 vidas, máximo 12 rondas.

### 🎸 Modo Canción
Suena una pista de fondo real (DFPlayer 2) mientras los LEDs indican, con anticipación, qué pad tocar y cuándo, según una partitura transcrita a milisegundos. Acierto perfecto: `200 × combo`; acierto dentro de ventana: `100 × combo`. El feedback visual usa un flash de LED **no bloqueante** (no usa `delay()`) para no desincronizar las notas siguientes. No se puede iniciar simultáneamente con Libre/Reflejos/Memoria (exclusión mutua por software).

**Canciones activas:**

| id | Canción | BPM | Pads usados | Notas |
|---|---|---|---|---|
| 0 | Billie Jean | 117 | Bombo, Caja, Hi-Hat | 1436 |
| 2 | We Will Rock You | 87 | Bombo, Hi-Hat (platillo remapeado desde el pad deshabilitado) | 240 (compases 1-40; el final de la canción no está transcrito) |
| 4 | Seven Nation Army | 126 | Bombo, Caja, Tom piso, Hi-Hat | 128 |

Camisa Negra (id 1) y Overcompensate (id 3) tienen archivo de notas pero no están conectados a la tabla activa (`tablaNotas[]` en `modo_cancion.cpp`).

---

## Interfaz web (dashboard)

### Acceso
1. Conectarse a la red WiFi que el ESP32 tenga configurada, o al Access Point de respaldo **`BateriaESPOL`** / **`bateria123`** si no hay red guardada.
2. Abrir el navegador en la IP que muestra el OLED al iniciar (`192.168.4.1` en modo AP).

### Pantallas
| Pantalla | Descripción |
|---|---|
| Login | Ingresar nombre del jugador |
| Selección de modo | Libre, Reflejos, Memoria o Canción |
| Selección de canción | Lista de canciones activas con BPM y cantidad de notas |
| Juego | Score en tiempo real, vidas o progreso de canción, combo, feed de eventos |
| Resultado | Puntuación final, aciertos, combo máximo |
| Ranking | Top 10 global y por modo/canción, tiempo real |
| Config WiFi | Guardar hasta 2 redes en EEPROM |

### Protocolo WebSocket

**Celular → ESP32:**
```json
{ "accion": "iniciar_juego", "modo": "reflejos", "jugador": "Daniel" }
{ "accion": "iniciar_cancion", "id": 2, "jugador": "Daniel" }
{ "accion": "terminar_juego" }
{ "accion": "get_scores" }
{ "accion": "guardar_wifi", "slot": 0, "ssid": "MiRed", "pass": "clave" }
```

**ESP32 → Celular (modos clásicos):**
```json
{ "evento": "juego_iniciado", "vidas": 3, "modo": "reflejos" }
{ "evento": "pad_objetivo", "pad": 2, "ronda": 5 }
{ "evento": "hit_correcto", "pad": 2, "puntos_ganados": 200, "puntos_total": 850, "combo": 2 }
{ "evento": "hit_incorrecto", "pad": 4, "vidas": 2 }
{ "evento": "tiempo_agotado", "vidas": 2 }
{ "evento": "mostrando_secuencia", "largo": 4, "ronda": 4 }
{ "evento": "turno_jugador", "paso": 2, "total": 4, "ronda": 4 }
{ "evento": "ronda_completada", "ronda": 4, "puntos_total": 1200 }
{ "evento": "juego_terminado", "puntos_final": 1200 }
```

**ESP32 → Celular (modo Canción):**
```json
{ "evento": "cancion_iniciada", "cancion": "We Will Rock You", "jugador": "Daniel", "vidas": 3 }
{ "evento": "nota_hit", "pad": 0, "puntos": 200, "perfecto": true, "combo": 3 }
{ "evento": "nota_miss", "pad": 2, "vidas": 2 }
{ "evento": "cancion_terminada", "puntos_final": 3400, "aciertos": 180, "fallos": 20, "cancion": "We Will Rock You" }
```

```json
{ "scores": [ { "nombre": "Daniel", "modo": "reflejos", "puntos": 1200 } ] }
```

---

## Pasos para cargar el proyecto

### 1. Abrir en VSCode
Abrir la carpeta del proyecto con PlatformIO instalado.

### 2. Compilar y subir firmware
```
PlatformIO: Build    → verifica errores de compilación
PlatformIO: Upload   → sube el firmware al ESP32
```

### 3. Subir el filesystem
```
PlatformIO: Upload Filesystem Image  → sube la carpeta data/ (index.html + scores.json)
```
Sin este paso el ESP32 responde "Not found" al abrir el dashboard — es un paso separado de subir el firmware, hay que hacer ambos.

### 4. Monitor serie (debug)
```
PlatformIO: Monitor  → 115200 baud
```
Salida esperada:
```
=== BATERIA ANTI-ESTRES ESPOL ===
[OLED] SSD1306 listo
[SCORES] LittleFS listo
[PADS] 6 sensores piezoelectricos listos
[LEDS] FastLED listo — 52 LEDs, 6 pads (secciones: ...)
[AUDIO] DFPlayer 1 (pads) listo
[AUDIO] DFPlayer 2 (pistas) listo
[WiFi] AP activo  IP: 192.168.4.1   (o conectado a red guardada)
[SERVER] HTTP + WS activo en puerto 80
[MAIN] Setup completo. Esperando jugador...
```

---

## Problemas conocidos / pendientes

- **Pad 6 (GPIO39) deshabilitado por software** — falso contacto detectado en el PCB. Ver `PAD_DESHABILITADO` en `pads.cpp`. Pendiente: revisión física del PCB (continuidad de la resistencia pull-down y de la traza/vía hacia GPIO39).
- **Verificar `NUM_LEDS` vs. suma de `LEDS_POR_PAD[]`** — deben coincidir exactamente; el monitor serie avisa si no cuadran (`[LEDS] ERROR: ...`).
- **`oledTurnoJugador()` no actualiza la pantalla** durante el turno del jugador en modo Memoria (quedó vacía tras un refactor; la lógica de turno sigue funcionando vía WebSocket, solo el texto en el OLED físico no se actualiza en ese momento puntual).
- **`ipActual()` en `wifi_manager.cpp`** puede devolver una IP corrupta de forma intermitente al mostrarla en el OLED (uso de un `String` temporal ya liberado).
- **Buffer de scores JSON con tamaños distintos** entre `web_server.cpp` (4096 bytes) y los broadcasts de fin de juego en `game_modes.cpp`/`modo_cancion.cpp` (2048 bytes) — no debería truncarse en uso normal, pero conviene unificar si el ranking crece mucho.
- **We Will Rock You** solo tiene transcrito el patrón principal (compases 1-40); el final de la canción (relleno de batería) no está incluido.

---

## Consideraciones de seguridad (público infantil)

- Los pads son sensores piezoeléctricos de bajo voltaje, sin partes eléctricas expuestas al contacto directo del usuario.
- El volumen de ambos DFPlayer es ajustable por software (`setVolumenPads()` / `setVolumenPista()`, 0-30).
- No hay componentes de alto consumo o generación de calor accesibles al usuario.
- Los nombres de jugador se almacenan únicamente dentro del ESP32 (LittleFS), no se transmiten a servidores externos.

---

*Proyecto desarrollado como parte del curso de Laboratorio de Sistemas Embebidos — ESPOL 2026*