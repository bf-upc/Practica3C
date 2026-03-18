
# Prompts utilizados — Práctica 3C BLE con ESP32

**Alumno:** Bernat  
**Herramienta IA:** DeepSeek y Claude
**Conversación:** https://chat.deepseek.com/share/y7u8lfhzd89plukot0

---

## Prompt 1 — Servidor BLE básico

**Objetivo:** Generar el código base del servidor BLE con las características fundamentales del enunciado.

**Prompt enviado:**
> Genera un sketch completo de Arduino para ESP32 que implemente un servidor BLE con:
> - Un servicio con UUID '4fafc201-1fb5-459e-8fcc-c5c9c331914b'
> - Una característica de lectura que devuelva el valor del ADC en GPIO36
> - Una característica de escritura para controlar el LED integrado en GPIO2
> - Mensajes por Serial al conectar y desconectar un cliente BLE
> - Comentarios explicando cada bloque de código

**Resultado:** Código funcional generado correctamente.

**Problemas encontrados:**
- GPIO36 no disponible en ESP32-S3 → cambiado a GPIO4
- Nombre del dispositivo 'ESP32-BLE-Lab3' → cambiado a 'ESP32-S3_BERNAT'

**Acción tomada:** Ajuste manual de pines y nombre antes de compilar.

---

## Prompt 2 — Notificaciones automáticas ADC

**Objetivo:** Añadir notificaciones automáticas cada 1 segundo usando el descriptor BLE2902.

**Prompt enviado:**
> Modifica el código anterior para que la característica ADC envíe una
> notificación automática al cliente cada 1 segundo sin polling.
> Usa el descriptor BLE2902.

**Resultado:** Implementado correctamente con `MyCCCDCallbacks` y flag `notificationsEnabled`.

**Problemas encontrados:** Ninguno. Compiló y funcionó en el primer intento.

**Acción tomada:** —

---

## Prompt 3 — Característica JSON

**Objetivo:** Añadir una tercera característica que devuelva el estado completo del sistema en formato JSON.

**Prompt enviado:**
> Añade una tercera característica de lectura que devuelva un string JSON:
> {"adc": valor, "led": true/false, "uptime": segundos_desde_arranque}

**Resultado:** Función `updateJsonCharacteristic()` implementada sin librerías externas.

**Problemas encontrados:** Ninguno.

**Acción tomada:** Se aprovechó que la función también se llama desde `LedCharacteristicCallbacks` para mantener el JSON sincronizado al cambiar el LED.

---

## Prompt 4 — Ejercicio A: BLE + WiFi AP simultáneo

**Objetivo:** Combinar el servidor BLE con un Access Point WiFi para control dual del LED.

**Prompt enviado:**
> Combina el sketch BLE anterior con un servidor WiFi en modo Access Point.
> El LED debe poder controlarse tanto desde BLE como desde una página web.
> Usa una variable global compartida para el estado del LED.

**Resultado:** Función `applyLed(state, source)` centraliza el control del LED para ambas interfaces. Servidor web en http://192.168.4.1 con interfaz visual.

**Problemas encontrados:**
- El navegador solicita `/favicon.ico` automáticamente, generando un error de handler no encontrado en el monitor serie. La IA no incluyó este handler.

**Acción tomada:** Se identificó como error inofensivo (no afecta al funcionamiento) y se documentó en el informe.

---

## Prompt 5 — Ejercicio C: Seguridad BLE con PIN

**Objetivo:** Proteger el control del LED via BLE con un PIN de 6 dígitos.

**Prompt enviado:**
> Añade al sketch una característica de autenticación BLE con PIN de 6 dígitos.
> El cliente debe escribir el PIN correcto en una nueva característica AUTH
> antes de poder controlar el LED.
> Si el PIN es incorrecto, el acceso queda bloqueado.
> El flag de autenticación debe resetearse al desconectar.

**Resultado:** Nueva característica AUTH (UUID `...26ab`, READ|WRITE) con `AuthCharacteristicCallbacks`. Flag `bleAuthenticated` reseteado en cada conexión. Respuestas: `OK`, `FAIL`, `AUTH_REQUIRED`.

**Problemas encontrados:** Ninguno. Compiló en el primer intento.

**Acción tomada:** Se añadió el campo `"auth"` al JSON para poder inspeccionar el estado de autenticación desde la característica JSON.

---

## Notas sobre el uso de la IA

- La IA generó código técnicamente correcto en todos los casos, pero requirió adaptaciones de hardware (GPIO) y de nombre de dispositivo que solo son posibles con conocimiento previo del ESP32-S3.
- El error del favicon (Prompt 4) y la limitación de seguridad del PIN en claro (Prompt 5, no advertida por la IA) demuestran que la verificación técnica del alumno es imprescindible.
- Para seguridad BLE real se requeriría BLE Pairing con Bonding, no un PIN a nivel de aplicación.
