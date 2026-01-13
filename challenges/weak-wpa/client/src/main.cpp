#include <Arduino.h>
#include <ESP8266WiFi.h>

const char* ssid = "Codefreeze_Secure_WiFi";
const char* password = "sunshine1";

const unsigned long CONNECT_DURATION_MS = 5000;
const unsigned long DISCONNECT_DURATION_MS = 5000;

void turnOffBuiltInLed() {
    pinMode(D4, OUTPUT);
    digitalWrite(D4, HIGH);
}

void setup() {
    turnOffBuiltInLed();

    Serial.begin(74880);
    Serial.println("Weak WPA Client starting...");
    Serial.print("Will connect to: ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);
}

void loop() {
    // Connect - generates 4-way handshake that can be captured
    Serial.println("Connecting...");
    WiFi.begin(ssid, password);

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

    // Stay connected
    while (millis() - start < CONNECT_DURATION_MS) {
        delay(100);
    }

    // Disconnect - next connect will generate another handshake
    Serial.println("Disconnecting...");
    WiFi.disconnect();
    
    delay(DISCONNECT_DURATION_MS);
}
