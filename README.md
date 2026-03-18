# Práctica 3C - Control Dual: WiFi (AP) + BLE Simultáneo

[![PlatformIO](https://img.shields.io/badge/PlatformIO-IDE-blue?logo=platformio)](https://platformio.org/)
[![ESP32](https://img.shields.io/badge/ESP32-S3-orange)](https://www.espressif.com/)

Este repositorio contiene el código para la **Práctica 3C**, que implementa un sistema de control dual en un ESP32-S3. El dispositivo actúa como un **punto de acceso WiFi** y como **periférico BLE** simultáneamente, permitiendo controlar un LED desde una página web o desde una aplicación BLE.

## 📝 Descripción

El proyecto demuestra la capacidad de un ESP32 para gestionar múltiples interfaces de comunicación de forma concurrente. La variable de estado del LED (`ledState`) es **compartida y sincronizada** entre ambas interfaces, permitiendo un control coherente sin importar el método utilizado.

**Características principales:**
- **Punto de Acceso WiFi**: Crea su propia red WiFi para acceder a un servidor web embebido
- **Servidor BLE**: Actúa como periféneo Bluetooth Low Energy
- **Control Unificado**: El estado del LED se mantiene sincronizado entre ambas interfaces
- **Monitorización ADC**: Lectura analógica del pin GPIO4 visible en ambas plataformas
- **Notificaciones BLE**: Envío periódico del valor ADC cuando hay un cliente conectado

## ✨ Funcionalidad Destacada (Ejercicio Extra A)

El commit más reciente implementa el **control simultáneo por WiFi (AP) y BLE**, con las siguientes capacidades:

- **WiFi**: El ESP32-S3 crea su propia red con SSID `ESP32-S3_BERNAT` y contraseña `12345678`
- **Servidor Web**: Interfaz web accesible en `http://192.168.4.1` con botones para encender/apagar el LED
- **BLE**: Servicio con tres características (ADC, LED y JSON) para control desde apps como nRF Connect
- **Sincronización**: El LED se actualiza inmediatamente venga el comando de WiFi o BLE

## 📁 Estructura del Proyecto

```
Practica3C/
├── include/          # Archivos de cabecera
├── lib/              # Librerías del proyecto
├── src/              # Código fuente principal (ejercicioA.cpp)
├── test/             # Pruebas
└── platformio.ini    # Configuración de PlatformIO
```

## 🚀 Comenzando

### Prerrequisitos

- **Plataforma**: [PlatformIO](https://platformio.org/) (extensión VSCode o CLI)
- **Hardware**: Placa ESP32-S3 (o compatible con WiFi + BLE)
- **Conexiones**: LED incorporado (o externo en el pin correspondiente) y ADC en GPIO4

### Instalación y Uso

1. **Clonar el repositorio:**
   ```bash
   git clone https://github.com/bf-upc/Practica3C.git
   ```

2. **Abrir en PlatformIO** y compilar el proyecto

3. **Subir el firmware** a tu placa ESP32-S3:
   ```bash
   pio run --target upload
   ```

4. **Abrir el monitor serie** para ver la salida de depuración:
   ```bash
   pio device monitor
   ```

## 🎮 Cómo Usarlo

### Conexión WiFi (Punto de Acceso)

1. El ESP32-S3 crea la red WiFi: **`ESP32-S3_BERNAT`** (contraseña: `12345678`)
2. Conéctate a esta red desde tu teléfono u ordenador
3. Abre un navegador y ve a **`http://192.168.4.1`**
4. Verás una interfaz web con un indicador LED y botones para controlarlo

### Conexión BLE

1. Usa una app como **nRF Connect** o **LightBlue** en tu móvil
2. Escanea dispositivos BLE y conecta a **`ESP32-S3_BERNAT`**
3. Explora el servicio con UUID: `4fafc201-1fb5-459e-8fcc-c5c9c331914b`
4. Características disponibles:
   - **ADC** (`...b26a8`): Lectura del pin GPIO4 (valor 0-4095)
   - **LED** (`...b26a9`): Escribe `1` para encender, `0` para apagar
   - **JSON** (`...b26aa`): Información completa en formato JSON

## 🔧 Detalles Técnicos

### Pines Utilizados
| Pin | Función | Descripción |
|-----|---------|-------------|
| LED_BUILTIN | LED | LED interno de la placa |
| GPIO4 | ADC | Entrada analógica (0-3.3V) |

### Red WiFi
| Parámetro | Valor |
|-----------|-------|
| SSID | ESP32-S3_BERNAT |
| Contraseña | 12345678 |
| IP | 192.168.4.1 |
| Puerto | 80 |

### Servicio BLE
| UUID | Propiedades | Descripción |
|------|-------------|-------------|
| Servicio: `4fafc201-1fb5-459e-8fcc-c5c9c331914b` | - | Servicio principal |
| Caract. ADC: `...b26a8` | READ, NOTIFY | Valor analógico |
| Caract. LED: `...b26a9` | WRITE | Control LED |
| Caract. JSON: `...b26aa` | READ | Estado completo |

### Comunicación JSON
```json
{
  "adc": 2048,
  "led": true,
  "uptime": 42
}
```

## 📡 Salida por Monitor Serie

El programa proporciona información detallada por el puerto serie:
```
🔌 BLE + WiFi AP — ESP32-S3_BERNAT
📶 WiFi AP activo — IP: 192.168.4.1
🌐 Servidor web iniciado en http://192.168.4.1
📡 BLE advertising iniciado
——————————————————————————
📱 Conecta al WiFi 'ESP32-S3_BERNAT' (pass: 12345678)
🌐 Abre http://192.168.4.1 para controlar el LED
🔵 O usa nRF Connect con BLE
💡 LED ENCENDIDO via WiFi Web
🔔 NOTIFY ADC: 1234 (1.02V) | LED: ON
💡 LED APAGADO via BLE
```

## 📜 Notas Importantes

- El dispositivo **no se conecta a Internet**, crea su propia red local
- Puedes controlar el LED simultáneamente desde la web y BLE
- Las notificaciones BLE se activan cuando un cliente habilita las notificaciones en la característica ADC
- El valor ADC se actualiza cada segundo cuando hay un cliente BLE conectado

## 📄 Licencia

Proyecto académico - Universidad Politécnica de Cataluña (UPC)

