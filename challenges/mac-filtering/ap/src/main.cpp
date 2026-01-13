#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

extern "C" {
    #include "user_interface.h"
}

const char* ssid = "Codefreeze_MAC_Filtered";
const char* password = "letmein123";

// Whitelist - update with your client's MAC after flashing it!
// Format: lowercase, colon-separated
// Flash client first, check Serial output for its MAC
const char* ALLOWED_MAC = "aa:bb:cc:dd:ee:ff";  // UPDATE THIS!

DNSServer dnsServer;
ESP8266WebServer webServer(80);
WiFiEventHandler stationConnectedHandler;

const char* portalPage = R"(
<!DOCTYPE html>
<html>
<head>
    <title>Challenge - MAC Filtering</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: sans-serif; text-align: center; padding: 30px; background: #1a1a2e; color: #eee; }
        h1 { color: #e94560; }
        .section { background: #16213e; padding: 20px; border-radius: 8px; margin: 20px auto; max-width: 550px; text-align: left; }
        h3 { color: #e94560; margin-top: 0; }
        code { background: #0f3460; padding: 2px 8px; border-radius: 4px; }
    </style>
</head>
<body>
    <h1>MAC Filtering Bypassed!</h1>
    <p>You spoofed an allowed MAC address.</p>
    
    <div class="section">
        <h3>The Vulnerability</h3>
        <p>MAC filtering only allows "approved" devices to connect based on their hardware address.</p>
        <p>The problem: <strong>MAC addresses are broadcast in plaintext</strong> and trivially spoofed.</p>
    </div>
    
    <div class="section">
        <h3>The Attack</h3>
        <ol>
            <li>Sniff traffic with <code>airodump-ng</code></li>
            <li>See which MACs are connected to the target network</li>
            <li>Spoof your MAC to match an allowed one:</li>
        </ol>
        <p><code>sudo ip link set wlan0 down</code></p>
        <p><code>sudo ip link set wlan0 address XX:XX:XX:XX:XX:XX</code></p>
        <p><code>sudo ip link set wlan0 up</code></p>
        <p style="margin-top: 15px;">Or use <code>macchanger -m XX:XX:XX:XX:XX:XX wlan0</code></p>
    </div>
    
    <div class="section">
        <h3>Why It Fails</h3>
        <p>MAC addresses are:</p>
        <ul>
            <li>Transmitted unencrypted in every frame</li>
            <li>Changeable in software on most devices</li>
            <li>Not cryptographically verified</li>
        </ul>
        <p style="color: #888; font-size: 0.9em;">MAC filtering is access control theater - use WPA2/WPA3 with strong passwords instead.</p>
    </div>
</body>
</html>
)";

String macToString(const uint8_t* mac) {
    char buf[18];
    snprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return String(buf);
}

void onStationConnected(const WiFiEventSoftAPModeStationConnected& evt) {
    String clientMac = macToString(evt.mac);
    Serial.print("Client connected: ");
    Serial.println(clientMac);
    
    if (clientMac.equalsIgnoreCase(ALLOWED_MAC)) {
        Serial.println("  -> MAC is WHITELISTED, allowing connection");
    } else {
        Serial.println("  -> MAC is NOT whitelisted, kicking client...");
        // Deauth the client
        wifi_softap_deauth_station(0, (uint8*)evt.mac);
    }
}

void handleRoot() {
    webServer.send(200, "text/html", portalPage);
}

void handleNotFound() {
    webServer.sendHeader("Location", "http://192.168.4.1/", true);
    webServer.send(302, "text/plain", "");
}

void turnOffBuiltInLed() {
    pinMode(D4, OUTPUT);
    digitalWrite(D4, HIGH);
}

void setup() {
    turnOffBuiltInLed();

    Serial.begin(74880);
    Serial.println("\nMAC Filtering AP starting...");
    Serial.print("Allowed MAC: ");
    Serial.println(ALLOWED_MAC);
    Serial.println("(Update ALLOWED_MAC in code if needed)\n");

    WiFi.softAP(ssid, password);
    Serial.print("AP started. IP: ");
    Serial.println(WiFi.softAPIP());
    Serial.print("SSID: ");
    Serial.println(ssid);
    Serial.print("Password: ");
    Serial.println(password);

    // Register connection handler for MAC filtering
    stationConnectedHandler = WiFi.onSoftAPModeStationConnected(onStationConnected);

    dnsServer.start(53, "*", WiFi.softAPIP());

    webServer.on("/", handleRoot);
    webServer.onNotFound(handleNotFound);
    webServer.begin();
    Serial.println("Captive portal started");
}

void loop() {
    dnsServer.processNextRequest();
    webServer.handleClient();
}
