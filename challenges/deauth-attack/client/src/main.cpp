#include <Arduino.h>
#include <ESP8266WiFi.h>

const char* ssid = "Codefreeze_Kickme";

const unsigned long RECONNECT_DELAY_MS = 3000;
const unsigned long STATUS_INTERVAL_MS = 5000;

unsigned long lastStatusPrint = 0;
bool wasConnected = false;

void turnOffBuiltInLed() {
    pinMode(D4, OUTPUT);
    digitalWrite(D4, HIGH);
}

void setup() {
    turnOffBuiltInLed();

    Serial.begin(74880);
    Serial.println("\nDeauth Challenge Client starting...");
    Serial.print("Target SSID: ");
    Serial.println(ssid);
    Serial.println("Will stay connected until deauth'd.\n");

    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(false);  // We'll handle reconnection manually
    
    Serial.println("Connecting...");
    WiFi.begin(ssid);
}

void loop() {
    bool isConnected = (WiFi.status() == WL_CONNECTED);
    
    // Detect disconnection (likely from deauth!)
    if (wasConnected && !isConnected) {
        Serial.println("\n!!! DISCONNECTED (deauth?) !!!");
        Serial.println("Waiting before reconnect...");
        delay(RECONNECT_DELAY_MS);
        Serial.println("Reconnecting... (probe request reveals SSID!)");
        WiFi.begin(ssid);
    }
    
    // Detect new connection
    if (!wasConnected && isConnected) {
        Serial.print("Connected! IP: ");
        Serial.println(WiFi.localIP());
        Serial.println("Staying connected... waiting to be kicked.");
    }
    
    wasConnected = isConnected;
    
    // Periodic status
    if (millis() - lastStatusPrint > STATUS_INTERVAL_MS) {
        if (isConnected) {
            Serial.print("Still connected. RSSI: ");
            Serial.print(WiFi.RSSI());
            Serial.println(" dBm");
        } else {
            Serial.println("Not connected. Trying...");
            WiFi.begin(ssid);
        }
        lastStatusPrint = millis();
    }
    
    delay(100);
}
