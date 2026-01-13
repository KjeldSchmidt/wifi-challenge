#include <Arduino.h>
#include <ESP8266WiFi.h>

const char* ssid = "Codefreeze_MAC_Filtered";
const char* password = "letmein123";

const unsigned long CONNECT_DURATION_MS = 5000;
const unsigned long DISCONNECT_DURATION_MS = 5000;

void turnOffBuiltInLed() {
    pinMode(D4, OUTPUT);
    digitalWrite(D4, HIGH);
}

void setup() {
    turnOffBuiltInLed();

    Serial.begin(74880);
    Serial.println("\nMAC Filtering Client starting...");
    
    WiFi.mode(WIFI_STA);
    
    // Print MAC address - copy this to the AP's ALLOWED_MAC!
    Serial.println("==========================================");
    Serial.print("THIS CLIENT'S MAC: ");
    Serial.println(WiFi.macAddress());
    Serial.println("Copy this to the AP's ALLOWED_MAC constant!");
    Serial.println("==========================================\n");
    
    Serial.print("Will connect to: ");
    Serial.println(ssid);
}

void loop() {
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
        Serial.println("Connection failed (AP not running or MAC not whitelisted)");
    }

    // Stay connected
    while (millis() - start < CONNECT_DURATION_MS) {
        delay(100);
    }

    Serial.println("Disconnecting...");
    WiFi.disconnect();
    
    delay(DISCONNECT_DURATION_MS);
}
