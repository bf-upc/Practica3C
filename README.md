# Práctica 3C — BLE + WiFi AP + Seguridad PIN con ESP32-S3

[![PlatformIO](https://img.shields.io/badge/PlatformIO-IDE-blue?logo=platformio)](https://platformio.org/)
[![ESP32-S3](https://img.shields.io/badge/ESP32--S3-orange)](https://www.espressif.com/)
[![BLE](https://img.shields.io/badge/BLE-4.2-blue)](https://www.bluetooth.com/)
[![WiFi](https://img.shields.io/badge/WiFi-AP-green)](https://docs.espressif.com/)

**Universitat Politècnica de Catalunya (UPC)**
Asignatura: Microprocesadores y Sistemas Digitales · Curso 2025-2026
Alumno: Bernat · Dispositivo: ESP32-S3_BERNAT

---

## 📋 Descripción

Este repositorio contiene el código completo de la **Práctica 3C**, que implementa un servidor BLE avanzado sobre ESP32-S3 con las siguientes capacidades:

- **Servidor BLE** con 4 características (ADC, LED, JSON, AUTH)
- **WiFi Access Point** simultáneo para control desde navegador web
- **Seguridad por PIN** de 6 dígitos para proteger el control del LED via BLE
- **Notificaciones automáticas** del valor ADC cada segundo (sin polling)
- **Estado JSON** con ADC, LED, uptime y estado de autenticación

El sistema demuestra la coexistencia de BLE y WiFi sobre la misma radio 2.4GHz del ESP32-S3 mediante time-sharing, con una variable de estado compartida que garantiza coherencia entre ambas interfaces.

---

## 🔌 Características BLE implementadas

| UUID | Propiedad | Descripción |
|------|-----------|-------------|
| `...26a8` | READ \| NOTIFY | Valor ADC GPIO4 (0-4095). Notificación automática cada 1s |
| `...26a9` | READ \| WRITE | Control del LED. Requiere autenticación previa |
| `...26aa` | READ | JSON con `{adc, led, uptime, auth}` |
| `...26ab` | READ \| WRITE | Autenticación por PIN. Escribe el PIN, lee `OK` o `FAIL` |

UUID del servicio: `4fafc201-1fb5-459e-8fcc-c5c9c331914b`

---

## 📁 Estructura del repositorio

```
Practica3C/
├── src/
│   └── main.cpp              # Código fuente final (BLE + WiFi + PIN Auth)
├── include/                  # Cabeceras
├── lib/                      # Librerías
├── capturas/                 # Capturas de pantalla de todas las pruebas
├── informe/
│   └── informe_v3.pdf        # Informe final generado con IA (iteración 3)
├── prompts/
│   └── prompts_usados.md     # Todos los prompts utilizados durante la práctica
├── platformio.ini            # Configuración PlatformIO
└── README.md
```

---

## 🚀 Compilación y carga

### Requisitos

- [PlatformIO](https://platformio.org/) (extensión VSCode o CLI)
- Placa ESP32-S3 (testado en ESP32-S3-DevKitC-1)
- App [nRF Connect](https://www.nordicsemi.com/Products/Development-tools/nRF-Connect-for-mobile) en el móvil

### Pasos

```bash
# Clonar el repositorio
git clone https://github.com/bf-upc/Practica3C.git
cd Practica3C

# Compilar y subir con PlatformIO
pio run --target upload

# Abrir monitor serie
pio device monitor --baud 115200
```

---

## 📱 Cómo usar

### Control desde nRF Connect (BLE)

1. Abre nRF Connect y busca `ESP32-S3_BERNAT`
2. Conecta y localiza el servicio `4fafc201...`
3. **Autenticación obligatoria**: escribe `123456` en la característica AUTH (`...26ab`)
4. Lee el valor de AUTH → debe mostrar `OK`
5. Ahora puedes escribir `1` (encender) o `0` (apagar) en la característica LED (`...26a9`)
6. Para activar notificaciones ADC: escribe `0100` en el descriptor CCCD (`0x2902`)

### Control desde navegador web (WiFi AP)

1. Conecta tu móvil/PC al WiFi `ESP32-S3_BERNAT` (contraseña: `12345678`)
2. Abre el navegador en `http://192.168.4.1`
3. Usa los botones Encender / Apagar directamente (sin PIN)

### Escenarios de seguridad BLE

| Acción | Respuesta AUTH | Respuesta LED |
|--------|---------------|---------------|
| WRITE AUTH `123456` | `OK` | — |
| WRITE LED `1` (autenticado) | — | LED encendido |
| WRITE AUTH `000000` (PIN malo) | `FAIL` | — |
| WRITE LED `1` (sin autenticar) | — | `AUTH_REQUIRED` |

---

## 🔧 Configuración principal

```cpp
#define ADC_PIN       4         // GPIO4 para ADC en ESP32-S3
#define CORRECT_PIN   "123456"  // PIN de autenticación BLE
const char* AP_SSID     = "ESP32-S3_BERNAT";
const char* AP_PASSWORD = "12345678";
```

---

## 📊 Informe

El informe técnico completo generado con IA está disponible en [`informe/informe_v3.pdf`](informe/informe_v3.pdf).

Incluye:
- Introducción al protocolo BLE y modelo GATT
- Análisis de MTU y frecuencia de notificaciones
- Descripción bloque a bloque del código
- Resultados con 17 capturas de pantalla
- Reflexión sobre el uso de la IA
- Evaluación: **10/10**

---

## 📝 Proceso de iteración

| Iteración | Mejoras | Nota |
|-----------|---------|------|
| 1 | Código BLE básico con 3 características | 8.5/10 |
| 2 | Ejercicio A (BLE+WiFi), cuestionario BLE, análisis MTU | 9.2/10 |
| 3 | Ejercicio C (seguridad PIN), readADCSafe(), reflexión ampliada | **10/10** |

---

## 📚 Referencias

- [Random Nerd Tutorials — ESP32 BLE Arduino IDE](https://randomnerdtutorials.com/esp32-ble-arduino-ide/)
- [Documentación Espressif BLE](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/bluetooth/)
- [Material docente Práctica 3C — UPC 2025-2026]