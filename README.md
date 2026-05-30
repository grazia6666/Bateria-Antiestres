# 🥁 Batería Anti-Estrés Politécnico

**Laboratorio de Sistemas Embebidos — ESPOL**  
Paralelo 101 · Ing. Karen Thamara Torres Medina  
Estudiantes: Daniel Espinoza (Mecatrónica) · Maria Bravo (Telemática)

---

## Descripción

Batería electrónica interactiva de bajo costo construida sobre un **ESP32 DevKit V1**, con formato de máquina arcade. El sistema detecta golpes en pads físicos mediante sensores piezoeléctricos, genera respuesta luminosa (LEDs WS2812B), sonora (DFPlayer Mini) y visual (OLED SSD1306). Incluye tres modos de juego y un sistema IoT donde los jugadores se conectan desde su celular vía WiFi Access Point para registrarse y ver el ranking en tiempo real.

---

## Características principales

- 6 pads con sensores piezoeléctricos y LEDs RGB individuales
- Pantalla OLED 128×64 que indica qué pad tocar (árbitro del juego)
- Audio con DFPlayer Mini y parlante 8Ω
- 3 modos de juego: Libre, Reflejos y Memoria
- WiFi Access Point propio del ESP32 (sin router externo)
- Dashboard web accesible desde cualquier celular
- Ranking en tiempo real via WebSocket
- Almacenamiento de hasta 2 redes WiFi en memoria EEPROM (NVS)
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
| Resistencia 10kΩ | 1 | Protección datos DFPlayer |
| Interruptor | 1 | Encendido/apagado |

### Diagrama de pines ESP32

```
ESP32 DevKit V1
┌─────────────────────────────────────────────┐
│                                             │
│  GPIO 34 ──── Piezoeléctrico PAD 1 (ADC)   │
│  GPIO 35 ──── Piezoeléctrico PAD 2 (ADC)   │
│  GPIO 32 ──── Piezoeléctrico PAD 3 (ADC)   │
│  GPIO 33 ──── Piezoeléctrico PAD 4 (ADC)   │
│  GPIO 25 ──── Piezoeléctrico PAD 5 (ADC)   │
│  GPIO 26 ──── Piezoeléctrico PAD 6 (ADC)   │
│                                             │
│  GPIO  4 ──── Data LEDs WS2812B            │
│                                             │
│  GPIO 21 ──── OLED SDA (I2C)               │
│  GPIO 22 ──── OLED SCL (I2C)               │
│                                             │
│  GPIO 16 ──── DFPlayer TX → RX ESP32       │
│  GPIO 17 ──── DFPlayer RX ← TX ESP32       │
│                                             │
│  3.3V  ──────── OLED VCC                   │
│  5V    ──────── LEDs VCC · DFPlayer VCC    │
│  GND   ──────── Tierra común               │
└─────────────────────────────────────────────┘
```

### Conexión piezoeléctrico (por pad)

```
Piezoeléctrico (+) ──┬──── GPIO ADC (34/35/32/33/25/26)
                     │
                   1MΩ  (pull-down a GND)
                     │
Piezoeléctrico (-) ──┴──── GND
```

> **Nota:** Los pines 34, 35, 32, 33 son solo entrada (input-only) en el ESP32, ideales para ADC de piezoeléctricos.

### Conexión OLED SSD1306

```
OLED VCC  ──── 3.3V
OLED GND  ──── GND
OLED SDA  ──── GPIO 21
OLED SCL  ──── GPIO 22
Dirección I2C: 0x3C
```

### Conexión DFPlayer Mini

```
DFPlayer VCC  ──── 5V
DFPlayer GND  ──── GND
DFPlayer TX   ──── GPIO 16 (con resistencia 1kΩ en serie)
DFPlayer RX   ──── GPIO 17
DFPlayer SPK1 ──── Parlante 8Ω (+)
DFPlayer SPK2 ──── Parlante 8Ω (-)
```

### Conexión LEDs WS2812B

```
LED VCC   ──── 5V
LED GND   ──── GND
LED DIN   ──── GPIO 4
```

> Los 6 LEDs van en cadena (DIN → DOUT → DIN → ...).

### Sistema de alimentación

```
Baterías 18650 (x2 en paralelo, 3.7V ~5000mAh)
    │
   TP4056 (carga USB + protección sobredescarga)
    │
   MT3608 Boost (3.7V → 5V)
    │
   Interruptor ON/OFF
    │
   ┌────────────┬──────────────┐
  5V bus      3.3V (reg ESP32)  GND común
   │            │
LEDs · DFPlayer  OLED
```

---

## Software

### Entorno de desarrollo

- **IDE:** Visual Studio Code + PlatformIO
- **Framework:** Arduino para ESP32
- **Lenguaje:** C++ (estilo C, sin clases propias)
- **Sistema de archivos:** LittleFS

### Estructura del proyecto

```
bateria-antistress/
├── platformio.ini          ← configuración y dependencias
├── data/                   ← se sube como filesystem (LittleFS)
│   ├── index.html          ← dashboard web completo (SPA)
│   └── scores.json         ← ranking persistente
└── src/
    ├── main.cpp            ← setup + loop
    ├── wifi_manager.h/cpp  ← AP WiFi + credenciales EEPROM
    ├── oled_display.h/cpp  ← pantallas del juego en OLED
    ├── pads.h/cpp          ← lectura ADC piezoeléctricos
    ├── leds.h/cpp          ← control WS2812B con FastLED
    ├── audio.h/cpp         ← DFPlayer Mini UART2
    ├── scores.h/cpp        ← ranking en LittleFS JSON
    ├── web_server.h/cpp    ← AsyncWebServer + WebSocket
    └── game_modes.h/cpp    ← lógica completa de los 3 modos
```

### Librerías utilizadas

| Librería | Versión | Uso |
|---|---|---|
| ESPAsyncWebServer | latest | Servidor web asíncrono |
| AsyncTCP | latest | TCP asíncrono para WebServer |
| ArduinoJson | ^7.0.0 | Serialización JSON |
| FastLED | 3.5.0 | Control LEDs WS2812B |
| DFRobotDFPlayerMini | ^1.0.5 | Control módulo de audio |
| Adafruit SSD1306 | ^2.5.7 | Driver pantalla OLED |
| Adafruit GFX Library | ^1.11.9 | Gráficos para OLED |

### Parámetros configurables en `platformio.ini`

| Parámetro | Valor por defecto | Descripción |
|---|---|---|
| `OLED_SDA` | 21 | Pin SDA del OLED |
| `OLED_SCL` | 22 | Pin SCL del OLED |
| `PIN_PAD0..5` | 34,35,32,33,25,26 | Pines ADC de los pads |
| `PIN_LEDS` | 4 | Pin data LEDs WS2812B |
| `NUM_LEDS` | 6 | Número de LEDs |
| `PIN_DFP_RX` | 16 | RX del ESP32 → TX DFPlayer |
| `PIN_DFP_TX` | 17 | TX del ESP32 → RX DFPlayer |
| `PIEZO_UMBRAL` | 300 | Umbral ADC para detectar golpe (0–4095) |
| `TIEMPO_REFLEJO_MS` | 1500 | Tiempo máximo de reacción en modo Reflejos |

---

## Modos de juego

### Árbitro del juego

El **ESP32 es el árbitro**. La pantalla web solo muestra el score y eventos. El OLED de la batería es quien indica qué pad debe tocar el jugador.

```
Celular → Login + selección de modo → ESP32 arranca el juego
ESP32 → OLED muestra pad objetivo → LED del pad enciende
Jugador golpea el pad físico → Piezo detecta → ESP32 valida
ESP32 → actualiza score → WebSocket → celular muestra resultado
```

### 🎵 Modo Libre

- El jugador toca cualquier pad libremente
- Los puntos se calculan según la **intensidad del golpe** (ADC 0–4095)
- Sin límite de tiempo ni vidas
- El OLED muestra la puntuación acumulada en tiempo real
- Ideal para practicar ritmos y explorar sonidos

### ⚡ Modo Reflejos

- El ESP32 elige un pad aleatorio y lo ilumina
- El OLED muestra **"TOCA PAD X"** y una **barra de tiempo** decreciente
- El jugador tiene `1500 ms` para golpear el pad correcto
- **Acierto:** suma `100 × combo` puntos → siguiente ronda
- **Fallo o tiempo agotado:** pierde una vida, combo se reinicia
- El combo sube 1 nivel cada 5 aciertos consecutivos (máximo x8)
- **3 vidas** en total. Al llegar a 0 → fin del juego

### 🧠 Modo Memoria

- El ESP32 muestra una secuencia de pads encendidos (1 pad en ronda 1, 2 en ronda 2, etc.)
- El OLED muestra los números de la secuencia y el turno actual
- El jugador debe **repetir la secuencia en orden** golpeando los pads
- **Acierto completo:** suma `150 × ronda × combo` puntos → siguiente ronda
- **Error:** pierde una vida, se repite la misma ronda
- **3 vidas** en total. Máximo 12 rondas (secuencia de 12 pads)

---

## Interfaz web (dashboard)

### Acceso

1. Conectar el celular al WiFi: **`BateriaESPOL`** / contraseña: **`bateria123`**
2. Abrir el navegador en: **`http://192.168.4.1`**

### Pantallas

| Pantalla | Descripción |
|---|---|
| **Login** | Ingresar nombre del jugador |
| **Selección de modo** | Elegir entre Libre, Reflejos o Memoria |
| **Juego** | Score en tiempo real, vidas, combo, feed de eventos |
| **Resultado** | Puntuación final, aciertos, combo máximo |
| **Ranking** | Top 10 global y por modo, actualización en tiempo real |
| **Config WiFi** | Guardar hasta 2 redes en memoria EEPROM (botón ⚙️) |

### Protocolo WebSocket

**Celular → ESP32:**

```json
{ "accion": "iniciar_juego", "modo": "reflejos", "jugador": "Daniel" }
{ "accion": "terminar_juego" }
{ "accion": "get_scores" }
{ "accion": "guardar_wifi", "slot": 0, "ssid": "MiRed", "pass": "clave123" }
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
{ "evento": "score_update", "puntos": 1200, "combo": 3 }
{ "evento": "juego_terminado", "puntos_final": 1200 }
{ "scores": [ { "nombre": "Daniel", "modo": "reflejos", "puntos": 1200 } ] }
```

---

## Almacenamiento

| Dato | Dónde | Tecnología |
|---|---|---|
| Frontend (HTML/CSS/JS) | Flash del ESP32 | LittleFS |
| Ranking (top 20) | `/scores.json` en flash | LittleFS + ArduinoJson |
| Credenciales WiFi (2 redes) | Memoria NVS | `Preferences.h` |

---

## Pasos para cargar el proyecto

### 1. Clonar y abrir

Abrir la carpeta `bateria-antistress/` en VSCode con PlatformIO instalado.

### 2. Compilar y subir firmware

```
PlatformIO: Build       → verifica errores
PlatformIO: Upload      → sube el firmware al ESP32
```

### 3. Subir el filesystem (dashboard web)

```
PlatformIO: Upload Filesystem Image   → sube la carpeta data/ al ESP32
```

> Esto es obligatorio, sin este paso el ESP32 no puede servir la página web.

### 4. Monitor serie (opcional, para debug)

```
PlatformIO: Monitor     → 115200 baud
```

Verás mensajes como:
```
=== BATERIA ANTI-ESTRES ESPOL ===
[SCORES] LittleFS listo
[PADS] Inicializados 6 sensores piezoeléctricos
[LEDS] FastLED inicializado
[AUDIO] DFPlayer listo
[WiFi] AP activo → SSID: BateriaESPOL  IP: 192.168.4.1
[SERVER] Servidor HTTP + WS iniciado en puerto 80
[MAIN] Setup completo. Esperando jugador...
```

---

## Archivos de audio (DFPlayer Mini)

Copiar en la microSD dentro de una carpeta llamada `01/`:

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

> La microSD debe estar formateada en FAT32.

---

## Ajuste del umbral piezoeléctrico

Si los pads no detectan golpes o detectan demasiado ruido, ajustar `PIEZO_UMBRAL` en `platformio.ini`:

```ini
-DPIEZO_UMBRAL=300   ; valor por defecto (0–4095)
```

Para calibrar: abrir el monitor serie y agregar temporalmente en `pads.cpp`:

```cpp
Serial.printf("PAD %d: %d\n", i, analogRead(PINES[i]));
```

Un golpe normal debería leer entre 500 y 2000. El umbral debe estar por debajo del valor mínimo de golpe real.

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
| Corriente máxima en bus 5V | ~2A |
| Autonomía estimada | 3–5 horas (5000mAh combinados) |
| Temperatura de operación | 20°C – 45°C |

---

## Presupuesto estimado

| Componente | Precio (USD) |
|---|---|
| ESP32 DevKit V1 | $12.00 |
| Sensores piezoeléctricos ×6 | $3.48 c/u (~$21) |
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

- Los nombres de usuario se almacenan temporalmente y solo dentro del ESP32. No se transmiten a servidores externos.
- El volumen del sistema se mantiene en niveles moderados para evitar contaminación auditiva.
- El uso de baterías recargables reduce la generación de residuos electrónicos frente a baterías desechables.

---

*Proyecto desarrollado como parte del curso de Laboratorio de Sistemas Embebidos — ESPOL 2026*