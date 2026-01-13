#include <Arduino.h>
#include <ESP8266WiFi.h>

// Must match the AP's hidden SSID
const char* ssid = "Codefreeze_Secret_Network";

const unsigned long CONNECT_DURATION_MS = 5000;
const unsigned long DISCONNECT_DURATION_MS = 5000;

void turnOffBuiltInLed() {
    pinMode(D4, OUTPUT);
    digitalWrite(D4, HIGH);
}

void setup() {
    turnOffBuiltInLed();

    Serial.begin(74880);
    Serial.println("Hidden Network Client starting...");
    Serial.print("Will probe for SSID: ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);
}

void loop() {
    // Connect phase - sends probe requests revealing the hidden SSID
    Serial.println("Connecting to hidden network...");
    WiFi.begin(ssid);

    unsigned long start = millis();
    while (millis() - start < CONNECT_DURATION_MS) {
        if (WiFi.status() == WL_CONNECTED) {
            Serial.print("Connected! IP: ");
            Serial.println(WiFi.localIP());
            break;
        }
        delay(100);
    }

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Connection timeout (AP may not be running)");
    }

    // Stay connected for remaining time
    while (millis() - start < CONNECT_DURATION_MS) {
        delay(100);
    }

    // Disconnect phase
    Serial.println("Disconnecting...");
    WiFi.disconnect();
    
    delay(DISCONNECT_DURATION_MS);
}
