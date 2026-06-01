# 🥁 Batería Anti-Estrés Politécnico

**Proyecto de Sistemas Embebidos — ESPOL**  
Paralelo 101 · Ing. Karen Thamara Torres Medina  
Estudiantes: Daniel Espinoza (Mecatrónica) · Maria Bravo (Telemática)

---

## Descripción

Batería electrónica interactiva de bajo costo construida sobre un **ESP32 DevKit V1**, con formato de máquina arcade. El sistema detecta golpes en pads físicos mediante sensores piezoeléctricos, genera respuesta luminosa (LEDs WS2812B), sonora (DFPlayer Mini) y visual (OLED SSD1306). Incluye tres modos de juego y un sistema IoT donde los jugadores se conectan desde su celular vía WiFi Access Point para registrarse y ver el ranking en tiempo real.

---

## Cambios aplicados (retroalimentación del profesor)

- **Pines ADC1 exclusivos:** los sensores piezoeléctricos se movieron a GPIO 34, 35, 32, 33, 36 y 39, todos pertenecientes al bloque ADC1, que está aislado físicamente de la antena WiFi. Los pines GPIO 25 y 26 (ADC2) fueron eliminados porque comparten circuitería con el módulo de radio y corrompían las lecturas cuando un celular interactuaba con el dashboard.
- **C nativo estricto:** eliminado el uso de `String`, `bool`, `enum`, `map()` y `#pragma once` del código propio. Todo reemplazado con `char[]`, `int`, `#define`, aritmética manual y guards `#ifndef`. Las librerías externas (FastLED, Adafruit, AsyncWebServer) siguen siendo C++ porque no existe alternativa, pero todo el código del proyecto está escrito en estilo C nativo.
- **EEPROM.h explícito:** el almacenamiento de redes WiFi usa `EEPROM.h` con posiciones de memoria fijas y documentadas, en lugar de `Preferences.h`, para cumplir exactamente el lineamiento del curso.

---

## Características principales

- 6 pads con sensores piezoeléctricos y LEDs RGB individuales
- Pantalla OLED 128×64 que indica qué pad tocar (árbitro del juego)
- Audio con DFPlayer Mini y parlante 8Ω
- 3 modos de juego: Libre, Reflejos y Memoria
- WiFi Access Point propio del ESP32 (sin router externo)
- Dashboard web accesible desde cualquier celular conectado al AP
- Ranking en tiempo real vía WebSocket
- Almacenamiento de 2 redes WiFi en memoria EEPROM
- Alimentación portátil con 2 baterías 18650 recargables

---

## Hardware

### Lista de componentes

| Componente | Cantidad | Función |
|---|---|---|
| ESP32 DevKit V1 | 1 | Microcontrolador principal |
| Sensor piezoeléctrico | 6 | Detectar golpes en los pads |
| LED WS2812B | 6 | Iluminación RGB por pad |
| DFPlayer Mini | 1 | Reproducción de audio |
| Parlante 8Ω | 1 | Emisión de sonido |
| OLED SSD1306 128×64 | 1 | Pantalla indicadora I2C |
| Batería 18650 3.7V | 2 | Fuente de energía portátil |
| Módulo TP4056 | 1 | Carga y protección de baterías |
| Convertidor MT3608 Boost | 1 | Elevar voltaje 3.7V → 5V |
| Resistencia 1MΩ | 6 | Pull-down para piezoeléctricos |
| Resistencia 1kΩ | 1 | Protección línea TX → DFPlayer |
| Interruptor | 1 | Encendido/apagado |

---

## Mapa completo de pines ESP32

```
                    ESP32 DevKit V1
              ┌──────────────────────────┐
         EN   │ EN              GPIO 23  │
      GPIO 36 │ VP (ADC1) ←─── PIEZO 5  │ ← solo entrada, ADC1
      GPIO 39 │ VN (ADC1) ←─── PIEZO 6  │ ← solo entrada, ADC1
      GPIO 34 │ GPIO 34   ←─── PIEZO 1  │ ← solo entrada, ADC1
      GPIO 35 │ GPIO 35   ←─── PIEZO 2  │ ← solo entrada, ADC1
      GPIO 32 │ GPIO 32   ←─── PIEZO 3  │   ADC1
      GPIO 33 │ GPIO 33   ←─── PIEZO 4  │   ADC1
      GPIO 25 │ GPIO 25   (NO USAR)      │ ← ADC2 incompatible WiFi
      GPIO 26 │ GPIO 26   (NO USAR)      │ ← ADC2 incompatible WiFi
      GPIO 27 │ GPIO 27                  │
      GPIO 14 │ GPIO 14                  │
      GPIO 12 │ GPIO 12                  │
          GND │ GND ────── GND común     │
      GPIO 13 │ GPIO 13                  │
              │ SD2                      │
              │ SD3                      │
              │ CMD                      │
              │ SD0                      │
              │ CLK                      │
              │ SD1                      │
         3.3V │ 3V3 ────── OLED VCC      │
          GND │ GND                      │
       GPIO 1 │ TX0  (monitor serie)     │
       GPIO 3 │ RX0  (monitor serie)     │
      GPIO 21 │ GPIO 21 ──── OLED SDA    │   I2C
      GPIO 22 │ GPIO 22 ──── OLED SCL    │   I2C
      GPIO 19 │ GPIO 19                  │
      GPIO 18 │ GPIO 18                  │
       GPIO 5 │ GPIO 5                   │
      GPIO 17 │ GPIO 17 ──── DFPlayer RX │   UART2 TX del ESP32
      GPIO 16 │ GPIO 16 ←─── DFPlayer TX │   UART2 RX del ESP32
       GPIO 4 │ GPIO 4  ────► LED DIN    │   WS2812B data
       GPIO 0 │ GPIO 0  (BOOT button)    │
       GPIO 2 │ GPIO 2                   │
      GPIO 15 │ GPIO 15                  │
          GND │ GND                      │
          VIN │ VIN ────── 5V bus        │ ← alimentación 5V
              └──────────────────────────┘
```

---

## Conexiones detalladas por periférico

### Sensores piezoeléctricos (×6) — solo ADC1

> ⚠️ **Importante:** usar exclusivamente pines ADC1. El ADC2 comparte circuitería con el WiFi y las lecturas se corrompen durante la comunicación inalámbrica.

```
┌─────────┬──────────┬───────────────────────────┐
│  PAD    │  GPIO    │  Bloque ADC               │
├─────────┼──────────┼───────────────────────────┤
│  PAD 1  │  34      │  ADC1 canal 6 (input-only)│
│  PAD 2  │  35      │  ADC1 canal 7 (input-only)│
│  PAD 3  │  32      │  ADC1 canal 4             │
│  PAD 4  │  33      │  ADC1 canal 5             │
│  PAD 5  │  36 (VP) │  ADC1 canal 0 (input-only)│
│  PAD 6  │  39 (VN) │  ADC1 canal 3 (input-only)│
└─────────┴──────────┴───────────────────────────┘
```

Esquema de conexión por cada pad:

```
Piezoeléctrico (+) ──┬──── GPIO (34 / 35 / 32 / 33 / 36 / 39)
                     │
                    1MΩ   (resistencia pull-down a GND)
                     │
Piezoeléctrico (-) ──┴──── GND
```

Los pines 34, 35, 36 y 39 son input-only (no tienen pull-up/pull-down interno), por eso se requiere la resistencia externa de 1MΩ a GND para fijar el nivel en reposo.

---

### OLED SSD1306 128×64 — I2C

```
OLED VCC  ────  3.3V  (regulador interno del ESP32)
OLED GND  ────  GND
OLED SDA  ────  GPIO 21
OLED SCL  ────  GPIO 22
Dirección I2C: 0x3C
```

---

### LEDs WS2812B (×6 en cadena)

```
LED VCC   ────  5V bus  (desde MT3608)
LED GND   ────  GND
LED DIN   ────  GPIO 4

Cadena: GPIO 4 ──► LED1 DIN → DOUT ──► LED2 DIN → DOUT ──► ... ──► LED6
```

> Agregar un condensador de 100µF entre VCC y GND del bus de 5V para absorber picos de corriente de los LEDs.

---

### DFPlayer Mini + Parlante 8Ω — UART2

```
DFPlayer VCC   ────  5V bus
DFPlayer GND   ────  GND
DFPlayer RX    ────  GPIO 17  (a través de resistencia 1kΩ en serie)
DFPlayer TX    ────  GPIO 16
DFPlayer SPK1  ────  Parlante 8Ω terminal (+)
DFPlayer SPK2  ────  Parlante 8Ω terminal (-)
```

La resistencia de 1kΩ en la línea RX protege el DFPlayer de niveles de voltaje incorrectos.

---

### Sistema de alimentación

```
Baterías 18650 × 2 (paralelo)
  3.7V · ~5000 mAh combinados
       │
    TP4056
  (carga microUSB + protección sobredescarga)
       │
  Interruptor ON/OFF
       │
    MT3608 Boost
  (3.7V ──► 5V · máx 2A)
       │
       ├──── VIN del ESP32  (5V)
       ├──── VCC LEDs WS2812B  (5V)
       └──── VCC DFPlayer Mini  (5V)

La OLED toma 3.3V del regulador interno del ESP32.
```

> ⚠️ **Advertencia térmica MT3608:** no operar el convertidor al límite de su eficiencia. Mantener la corriente total por debajo de 1.5A para evitar calentamiento crítico y caídas de voltaje. Si los LEDs están todos al máximo simultáneamente, reducir el brillo en el código (`FastLED.setBrightness(80)`).

---

## Software

### Entorno de desarrollo

- **IDE:** Visual Studio Code + PlatformIO
- **Framework:** Arduino para ESP32
- **Lenguaje:** C nativo (archivos `.cpp` requeridos por las librerías externas, pero sin clases propias, sin `String`, sin `bool`, sin `enum`)
- **Sistema de archivos:** LittleFS

### Estructura del proyecto

```
bateria-antistress/
├── platformio.ini
├── data/
│   ├── index.html       ← dashboard web (SPA pasiva)
│   └── scores.json      ← ranking persistente
└── src/
    ├── main.cpp         ← setup() + loop()
    ├── wifi_manager.h/cpp  ← AP WiFi + EEPROM 2 redes
    ├── oled_display.h/cpp  ← pantallas OLED del juego
    ├── pads.h/cpp          ← ADC piezoeléctricos + debounce
    ├── leds.h/cpp          ← WS2812B FastLED
    ├── audio.h/cpp         ← DFPlayer Mini UART2
    ├── scores.h/cpp        ← LittleFS JSON top 20
    ├── web_server.h/cpp    ← AsyncWebServer + WebSocket
    └── game_modes.h/cpp    ← lógica de los 3 modos
```

### Librerías utilizadas

| Librería | Versión | Uso |
|---|---|---|
| ESPAsyncWebServer | latest | Servidor web asíncrono |
| AsyncTCP | latest | TCP asíncrono |
| ArduinoJson | ^7.0.0 | Serialización JSON |
| FastLED | 3.5.0 | Control LEDs WS2812B |
| DFRobotDFPlayerMini | ^1.0.5 | Módulo de audio |
| Adafruit SSD1306 | ^2.5.7 | Driver OLED |
| Adafruit GFX Library | ^1.11.9 | Gráficos OLED |

### Parámetros configurables en `platformio.ini`

| Parámetro | Valor | Descripción |
|---|---|---|
| `PIN_PAD0` | 34 | Piezoeléctrico PAD 1 — ADC1 |
| `PIN_PAD1` | 35 | Piezoeléctrico PAD 2 — ADC1 |
| `PIN_PAD2` | 32 | Piezoeléctrico PAD 3 — ADC1 |
| `PIN_PAD3` | 33 | Piezoeléctrico PAD 4 — ADC1 |
| `PIN_PAD4` | 36 | Piezoeléctrico PAD 5 — ADC1 |
| `PIN_PAD5` | 39 | Piezoeléctrico PAD 6 — ADC1 |
| `PIN_LEDS` | 4 | Data LEDs WS2812B |
| `OLED_SDA` | 21 | SDA pantalla OLED |
| `OLED_SCL` | 22 | SCL pantalla OLED |
| `PIN_DFP_RX` | 16 | RX ESP32 ← TX DFPlayer |
| `PIN_DFP_TX` | 17 | TX ESP32 → RX DFPlayer |
| `PIEZO_UMBRAL` | 300 | Umbral ADC golpe (0–4095) |
| `TIEMPO_REFLEJO_MS` | 1500 | Tiempo reacción modo Reflejos |

### Mapa de memoria EEPROM

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
       → ESP32 arranca el juego
       → OLED muestra pad objetivo
       → LED del pad enciende
       → Jugador golpea el pad físico
       → Piezo detecta intensidad (ADC1)
       → ESP32 valida
       → LED anima + OLED actualiza + audio suena
       → WebSocket → celular muestra score
```

### 🎵 Modo Libre

- Toca cualquier pad libremente
- Puntos proporcionales a la intensidad del golpe (ADC 0–4095)
- Sin límite de tiempo ni vidas
- OLED muestra puntuación acumulada

### ⚡ Modo Reflejos

- ESP32 elige pad aleatorio → LED enciende → OLED muestra "TOCA PAD X" + barra de tiempo
- 1500 ms para golpear el pad correcto
- Acierto: `100 × combo` puntos
- Fallo o tiempo agotado: pierde una vida, combo reinicia
- Combo sube cada 5 aciertos consecutivos (máximo x8)
- 3 vidas — al llegar a 0 termina el juego

### 🧠 Modo Memoria

- Secuencia crece cada ronda (1 pad → 2 → ... → 12)
- OLED muestra la secuencia completa, luego el turno del jugador
- Acierto completo: `150 × ronda × combo` puntos
- Error: pierde una vida y repite la misma ronda
- 3 vidas — máximo 12 rondas

---

## Interfaz web (dashboard)

### Acceso

1. Conectar al WiFi: **`BateriaESPOL`** / contraseña: **`bateria123`**
2. Abrir navegador en: **`http://192.168.4.1`**

### Pantallas

| Pantalla | Descripción |
|---|---|
| Login | Ingresar nombre del jugador |
| Selección de modo | Elegir entre Libre, Reflejos o Memoria |
| Juego | Score en tiempo real, vidas, combo, feed de eventos |
| Resultado | Puntuación final, aciertos, combo máximo |
| Ranking | Top 10 global y por modo, tiempo real |
| Config WiFi | Guardar hasta 2 redes en EEPROM (botón ⚙️) |

### Protocolo WebSocket

**Celular → ESP32:**
```json
{ "accion": "iniciar_juego", "modo": "reflejos", "jugador": "Daniel" }
{ "accion": "terminar_juego" }
{ "accion": "get_scores" }
{ "accion": "guardar_wifi", "slot": 0, "ssid": "MiRed", "pass": "clave" }
```

**ESP32 → Celular:**
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
{ "scores": [ { "nombre": "Daniel", "modo": "reflejos", "puntos": 1200 } ] }
```

---

## Pasos para cargar el proyecto

### 1. Abrir en VSCode
Abrir la carpeta `bateria-antistress/` con PlatformIO instalado.

### 2. Compilar y subir firmware
```
PlatformIO: Build    → verifica errores de compilación
PlatformIO: Upload   → sube el firmware al ESP32
```
Si sale "Write timeout": mantener presionado el botón **BOOT** del ESP32 al momento en que aparece `Connecting...` en el terminal.

### 3. Subir el filesystem
```
PlatformIO: Upload Filesystem Image  → sube la carpeta data/ al ESP32
```
Sin este paso el ESP32 no puede servir la página web.

### 4. Monitor serie (debug)
```
PlatformIO: Monitor  → 115200 baud
```
Salida esperada:
```
=== BATERIA ANTI-ESTRES ESPOL ===
[SCORES] LittleFS listo
[PADS] 6 sensores piezoelectricos listos
[LEDS] FastLED listo
[AUDIO] DFPlayer listo
[WiFi] AP activo  IP: 192.168.4.1
[SERVER] HTTP + WS activo en puerto 80
[MAIN] Setup completo. Esperando jugador...
```

---

## Archivos de audio (DFPlayer Mini)

Copiar en la microSD dentro de una carpeta llamada `01/` (FAT32):

```
microSD/
└── 01/
    ├── 0001.mp3   ← golpe correcto
    ├── 0002.mp3   ← golpe incorrecto
    ├── 0003.mp3   ← combo conseguido
    ├── 0004.mp3   ← ronda completada
    ├── 0005.mp3   ← game over
    ├── 0006.mp3   ← tick cuenta regresiva
    ├── 0007.mp3   ← sonido PAD 1 (bombo)
    ├── 0008.mp3   ← sonido PAD 2 (caja)
    ├── 0009.mp3   ← sonido PAD 3 (hi-hat cerrado)
    ├── 0010.mp3   ← sonido PAD 4 (hi-hat abierto)
    ├── 0011.mp3   ← sonido PAD 5 (tom)
    └── 0012.mp3   ← sonido PAD 6 (crash)
```

---

## Ajuste del umbral piezoeléctrico

Si los pads no detectan golpes o hay falsos positivos, ajustar en `platformio.ini`:

```ini
-DPIEZO_UMBRAL=300
```

Para calibrar, agregar temporalmente en `pads.cpp`:
```c
Serial.printf("PAD %d: %d\n", i, analogRead(PINES[i]));
```
Un golpe normal lee entre 500 y 2000. El umbral debe estar bajo el mínimo de golpe real.

---

## Especificaciones eléctricas

| Parámetro | Valor |
|---|---|
| Voltaje de entrada | 3.7V (baterías 18650) |
| Voltaje de operación | 5V (boost MT3608) |
| Consumo ESP32 | ~0.79W |
| Consumo LEDs WS2812B (máx.) | ~7.20W |
| Consumo DFPlayer + parlante | ~1.50W |
| Potencia total máxima | ~9.49W |
| Corriente máxima bus 5V | ~2A |
| Autonomía estimada | 3–5 horas (5000mAh) |
| Temperatura de operación | 20°C – 45°C |

---

## Presupuesto estimado

| Componente | Precio (USD) |
|---|---|
| ESP32 DevKit V1 | $12.00 |
| Sensores piezoeléctricos ×6 | ~$21.00 ($3.48 c/u) |
| LEDs WS2812B ×6 | $5.00 |
| DFPlayer Mini | $4.50 |
| Parlante 8Ω | $1.50 |
| Baterías 18650 ×2 | $3.35 |
| Módulo TP4056 | $1.50 |
| Convertidor MT3608 | $2.00 |
| OLED SSD1306 | $6.40 |
| **Total estimado** | **$57–70 USD** |

---

## Consideraciones éticas

- Los nombres de usuario se almacenan únicamente dentro del ESP32 y no se transmiten a servidores externos.
- El volumen se mantiene en niveles moderados para evitar contaminación auditiva.
- El uso de baterías recargables reduce la generación de residuos electrónicos.

---

*Proyecto desarrollado como parte del curso de Laboratorio de Sistemas Embebidos — ESPOL 2026