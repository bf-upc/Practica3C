# Práctica 3C - Control Dual con Autenticación BLE (PIN de 6 dígitos)

[![PlatformIO](https://img.shields.io/badge/PlatformIO-IDE-blue?logo=platformio)](https://platformio.org/)
[![ESP32-S3](https://img.shields.io/badge/ESP32-S3-orange)](https://www.espressif.com/)

Este repositorio contiene el código para la **Práctica 3C**, que implementa un sistema de control dual en un ESP32-S3 con **seguridad mejorada mediante autenticación por PIN en BLE**. El dispositivo actúa como punto de acceso WiFi y como periférico BLE, pero ahora el control por BLE requiere un PIN de 6 dígitos.

## 📝 Descripción

El proyecto demuestra un sistema embebido con múltiples interfaces de comunicación y **control de acceso seguro** para BLE. La variable de estado del LED (`ledState`) es compartida entre interfaces, pero las operaciones por BLE están protegidas por autenticación.

### 🆕 Novedades (Ejercicio Extra C)
- **Autenticación por PIN de 6 dígitos** para control BLE
- Nueva característica BLE `AUTH` para verificación de identidad
- Flag `bleAuthenticated` que habilita/deshabilita el control LED por BLE
- Mensajes de depuración detallados para el flujo de autenticación
- Estado de autenticación incluido en la característica JSON

## ✨ Funcionalidades

### WiFi (Punto de Acceso)
- Crea red WiFi con SSID: `ESP32-S3_BERNAT` (pass: `12345678`)
- Servidor web en `http://192.168.4.1`
- Control LED sin autenticación (acceso libre por WiFi)

### BLE con Seguridad PIN
- Servicio BLE con 4 características:
  - **ADC** (`...b26a8`): Lectura analógica (READ/NOTIFY)
  - **LED** (`...b26a9`): Control LED (WRITE) **protegido por PIN**
  - **JSON** (`...b26aa`): Estado completo (READ)
  - **AUTH** (`...b26ab`): Autenticación PIN (WRITE/READ) **NUEVA**

### 🔐 Flujo de Autenticación BLE
```
1. Cliente se conecta al dispositivo BLE
2. Cliente escribe PIN "123456" en característica AUTH
3. ESP32 verifica y responde "OK" o "FAIL"
4. Si OK → flag bleAuthenticated = true → permite control LED
5. Si FAIL → flag bleAuthenticated = false → comandos LED rechazados
```

## 📁 Estructura del Proyecto

```
Practica3C/
├── include/          # Archivos de cabecera
├── lib/              # Librerías privadas
├── src/              # Código fuente principal
│   └── ejercicioC.cpp # Versión con autenticación PIN
├── test/             # Pruebas
└── platformio.ini    # Configuración PlatformIO
```

## 🚀 Comenzando

### Prerrequisitos
- **Hardware**: ESP32-S3 (o compatible con WiFi + BLE)
- **Software**: PlatformIO, app BLE (nRF Connect/LightBlue)

### Instalación Rápida
```bash
git clone https://github.com/bf-upc/Practica3C.git
cd Practica3C
pio run --target upload
pio device monitor
```

## 🎮 Guía de Uso

### 1️⃣ Conexión WiFi (Acceso Web)
```bash
# Conéctate a la red WiFi:
SSID: ESP32-S3_BERNAT
Password: 12345678

# Abre en navegador:
http://192.168.4.1
```
✅ Control LED **sin autenticación** por web

### 2️⃣ Conexión BLE con Autenticación
```
📱 Usando nRF Connect:

1. Escanea y conecta a "ESP32-S3_BERNAT"
2. Busca característica AUTH (UUID ...26ab)
3. Escribe "123456" → recibe "OK"
4. ¡Ya puedes escribir en LED (...26a9)!
   - "1" = ENCENDER
   - "0" = APAGAR
```

### 3️⃣ Verificación de Estado
La característica JSON (`...26aa`) devuelve:
```json
{
  "adc": 2048,
  "led": true,
  "uptime": 42,
  "auth": true    // ← estado de autenticación BLE
}
```

## 🔧 Detalles Técnicos

### Configuración PIN
```cpp
#define CORRECT_PIN "123456"   // PIN por defecto (modificable)
```

### Características BLE
| UUID (sufijo) | Propiedades | Descripción | Requiere Auth |
|---------------|-------------|-------------|---------------|
| `...b26a8` | READ/NOTIFY | Valor ADC (GPIO4) | No |
| `...b26a9` | WRITE/READ | Control LED | **SÍ** |
| `...b26aa` | READ | Estado JSON | No |
| `...b26ab` | WRITE/READ | Autenticación PIN | No |

### Mensajes de Seguridad
```
🔐 Intento de PIN: '123456'
✅ PIN correcto — acceso BLE concedido

🔐 Intento de PIN: '000000'
❌ PIN incorrecto (intento: '000000')

🚫 Intento de control LED sin autenticación — rechazado
```

## 📊 Comparativa Ejercicios

| Característica | Ejercicio A | Ejercicio C |
|----------------|--------------|--------------|
| WiFi AP | ✅ | ✅ |
| BLE | ✅ | ✅ |
| Control Web | ✅ | ✅ |
| Autenticación PIN | ❌ | ✅ |
| Característica AUTH | ❌ | ✅ |
| Flag bleAuthenticated | ❌ | ✅ |
| Reset Auth al desconectar | ❌ | ✅ |

## 🛡️ Modelo de Seguridad

```
CONEXIÓN BLE ESTABLECIDA
        ↓
┌─────────────────┐
│  NO AUTENTICADO │ ← LED WRITE bloqueado
└─────────────────┘
        ↓ (escribe PIN correcto)
┌─────────────────┐
│   AUTENTICADO   │ ← LED WRITE permitido
└─────────────────┘
        ↓ (desconexión)
┌─────────────────┐
│  NO AUTENTICADO │ ← Reset automático
└─────────────────┘
```

## 📡 Salida Monitor Serie

```
🔐 BLE + WiFi AP + PIN Auth — ESP32-S3_BERNAT
📶 WiFi AP — IP: 192.168.4.1
📡 BLE advertising iniciado
🔐 Flujo de autenticación:
   1. Conectar con nRF Connect
   2. Escribir '123456' en característica AUTH (UUID ...26ab)
   3. Si responde 'OK' → ya puedes controlar el LED
   4. Si responde 'FAIL' → PIN incorrecto, acceso denegado

✅ Cliente BLE conectado — autenticación requerida
🔐 Intento de PIN: '123456'
✅ PIN correcto — acceso BLE concedido
💡 LED ENCENDIDO via BLE
🔔 NOTIFY ADC: 2048 (1.65V) | LED: ON | Auth: OK
❌ Cliente BLE desconectado
```

## ⚙️ Personalización

Para cambiar el PIN de autenticación:
```cpp
#define CORRECT_PIN "tu_pin"   // modificar en ejercicioC.cpp
```

## 📝 Notas Importantes

- El PIN se verifica **en cada intento** de autenticación
- La autenticación se **resetea automáticamente** al desconectar BLE
- El control por WiFi **no requiere autenticación** (acceso libre)
- La característica LED responde `AUTH_REQUIRED` si no hay autenticación
- El estado de autenticación es visible en la característica JSON

## 📄 Licencia

Proyecto académico - Universitat Politècnica de Catalunya (UPC)

---

**¿Dudas?** Revisa el código fuente en `src/ejercicioC.cpp` o el histórico de commits para más detalles.
```
