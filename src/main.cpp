/**
 * Ejercicio A — BLE + WiFi AP simultáneo
 * LED controlable desde BLE (característica WRITE) y desde página web (WiFi AP)
 * Variable global ledState compartida entre ambas interfaces
 */

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <WiFi.h>
#include <WebServer.h>

// ——— UUIDs BLE ———
#define SERVICE_UUID             "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_ADC_UUID  "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CHARACTERISTIC_LED_UUID  "beb5483e-36e1-4688-b7f5-ea07361b26a9"
#define CHARACTERISTIC_JSON_UUID "beb5483e-36e1-4688-b7f5-ea07361b26aa"

// ——— Pines ———
#define ADC_PIN 4

// ——— WiFi AP ———
const char* AP_SSID     = "ESP32-S3_BERNAT";
const char* AP_PASSWORD = "12345678";
WebServer server(80);

// ——— Variables globales compartidas ———
BLECharacteristic *pAdcCharacteristic;
BLECharacteristic *pLedCharacteristic;
BLECharacteristic *pJsonCharacteristic;
bool deviceConnected      = false;
bool ledState             = false;   // ← compartida entre BLE y WiFi
bool notificationsEnabled = false;
unsigned long startTime;

// ——— Función para actualizar JSON ———
void updateJsonCharacteristic() {
    unsigned long uptime = (millis() - startTime) / 1000;
    String json = "{\"adc\":" + String(analogRead(ADC_PIN)) +
                  ",\"led\":" + String(ledState ? "true" : "false") +
                  ",\"uptime\":" + String(uptime) + "}";
    pJsonCharacteristic->setValue(json.c_str());
}

// ——— Función para aplicar estado del LED ———
void applyLed(bool state, const char* source) {
    ledState = state;
    digitalWrite(LED_BUILTIN, state ? HIGH : LOW);
    updateJsonCharacteristic();
    Serial.printf("💡 LED %s via %s\n", state ? "ENCENDIDO" : "APAGADO", source);
}

// ——— Callbacks BLE ———
class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
        Serial.println("✅ Cliente BLE conectado");
    }
    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
        notificationsEnabled = false;
        Serial.println("❌ Cliente BLE desconectado");
        pServer->getAdvertising()->start();
    }
};

class LedCharacteristicCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string value = pCharacteristic->getValue();
        if (value == "1")      applyLed(true,  "BLE");
        else if (value == "0") applyLed(false, "BLE");
    }
};

class MyCCCDCallbacks : public BLEDescriptorCallbacks {
    void onWrite(BLEDescriptor* pDescriptor) {
        uint8_t* value = pDescriptor->getValue();
        if (pDescriptor->getLength() >= 2) {
            uint16_t cccd = (value[1] << 8) | value[0];
            notificationsEnabled = (cccd == 0x0001);
            Serial.printf("🔔 Notificaciones %s\n", notificationsEnabled ? "ACTIVADAS" : "DESACTIVADAS");
        }
    }
};

class JsonCharacteristicCallbacks : public BLECharacteristicCallbacks {
    void onRead(BLECharacteristic *pCharacteristic) {
        updateJsonCharacteristic();
    }
};

// ——— Página web HTML ———
String buildHtmlPage() {
    String color  = ledState ? "#FFD700" : "#444";
    String estado = ledState ? "ENCENDIDO" : "APAGADO";
    return R"(<!DOCTYPE html>
<html lang='es'>
<head>
  <meta charset='UTF-8'>
  <meta name='viewport' content='width=device-width, initial-scale=1'>
  <title>ESP32-S3 Control LED</title>
  <style>
    body { font-family: Arial, sans-serif; background: #1a1a2e; color: #eee;
           display: flex; flex-direction: column; align-items: center;
           justify-content: center; min-height: 100vh; margin: 0; }
    h1   { color: #1A6B8A; }
    .led { width: 80px; height: 80px; border-radius: 50%;
           background: )" + color + R"(; margin: 20px auto;
           box-shadow: 0 0 30px )" + color + R"(; }
    .btn { padding: 14px 40px; font-size: 18px; border: none; border-radius: 8px;
           cursor: pointer; margin: 8px; }
    .on  { background: #FFD700; color: #1a1a2e; }
    .off { background: #555;    color: #eee;    }
    p    { color: #aaa; font-size: 14px; }
  </style>
</head>
<body>
  <h1>ESP32-S3 Control LED</h1>
  <div class='led'></div>
  <h2>Estado: )" + estado + R"(</h2>
  <form action='/led' method='GET'>
    <button class='btn on'  name='state' value='1'>Encender</button>
    <button class='btn off' name='state' value='0'>Apagar</button>
  </form>
  <p>También controlable por BLE (nRF Connect)</p>
  <p>)" + String(analogRead(ADC_PIN)) + R"( — ADC GPIO4</p>
</body>
</html>)";
}

// ——— Handlers del servidor web ———
void handleRoot() {
    server.send(200, "text/html", buildHtmlPage());
}

void handleLed() {
    if (server.hasArg("state")) {
        String state = server.arg("state");
        if (state == "1")      applyLed(true,  "WiFi Web");
        else if (state == "0") applyLed(false, "WiFi Web");
    }
    // Redirige de vuelta a la página principal
    server.sendHeader("Location", "/");
    server.send(303);
}

// ——— Setup ———
void setup() {
    startTime = millis();
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n🔌 BLE + WiFi AP — ESP32-S3_BERNAT");

    // LED
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    // ADC
    pinMode(ADC_PIN, INPUT);
    analogReadResolution(12);

    // ——— WiFi Access Point ———
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    IPAddress ip = WiFi.softAPIP();
    Serial.print("📶 WiFi AP activo — IP: ");
    Serial.println(ip);

    server.on("/",    handleRoot);
    server.on("/led", handleLed);
    server.begin();
    Serial.println("🌐 Servidor web iniciado en http://192.168.4.1");

    // ——— BLE ———
    BLEDevice::init("ESP32-S3_BERNAT");
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    BLEService *pService = pServer->createService(SERVICE_UUID);

    // Característica ADC — READ | NOTIFY
    pAdcCharacteristic = new BLECharacteristic(
        CHARACTERISTIC_ADC_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
    );
    pService->addCharacteristic(pAdcCharacteristic);
    BLE2902 *pBLE2902 = new BLE2902();
    pBLE2902->setCallbacks(new MyCCCDCallbacks());
    pBLE2902->setNotifications(false);
    pAdcCharacteristic->addDescriptor(pBLE2902);

    // Característica LED — WRITE
    pLedCharacteristic = new BLECharacteristic(
        CHARACTERISTIC_LED_UUID,
        BLECharacteristic::PROPERTY_WRITE
    );
    pService->addCharacteristic(pLedCharacteristic);
    pLedCharacteristic->setCallbacks(new LedCharacteristicCallbacks());

    // Característica JSON — READ
    pJsonCharacteristic = new BLECharacteristic(
        CHARACTERISTIC_JSON_UUID,
        BLECharacteristic::PROPERTY_READ
    );
    pService->addCharacteristic(pJsonCharacteristic);
    pJsonCharacteristic->setCallbacks(new JsonCharacteristicCallbacks());
    updateJsonCharacteristic();

    pService->start();

    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);
    BLEDevice::startAdvertising();

    Serial.println("📡 BLE advertising iniciado");
    Serial.println("——————————————————————————");
    Serial.println("📱 Conecta al WiFi 'ESP32-S3_BERNAT' (pass: 12345678)");
    Serial.println("🌐 Abre http://192.168.4.1 para controlar el LED");
    Serial.println("🔵 O usa nRF Connect con BLE");
}

// ——— Loop ———
void loop() {
    // Atender peticiones web
    server.handleClient();

    // Notificaciones BLE cada 1 segundo
    static unsigned long lastNotify = 0;
    if (deviceConnected && notificationsEnabled && millis() - lastNotify >= 1000) {
        int adc = analogRead(ADC_PIN);
        pAdcCharacteristic->setValue(adc);
        pAdcCharacteristic->notify();
        updateJsonCharacteristic();
        Serial.printf("🔔 NOTIFY ADC: %d (%.2fV) | LED: %s\n",
            adc, (adc / 4095.0) * 3.3, ledState ? "ON" : "OFF");
        lastNotify = millis();
    }

    delay(10);
}