#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

// Hidden open network - deauth client to reveal SSID from probe requests
const char* ssid = "Codefreeze_Kickme";

DNSServer dnsServer;
ESP8266WebServer webServer(80);

const char* portalPage = R"(
<!DOCTYPE html>
<html>
<head>
    <title>Challenge - Deauth Attack</title>
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
    <h1>Deauth Attack Complete!</h1>
    <p>You performed a full attack chain.</p>
    
    <div class="section">
        <h3>What You Did</h3>
        <ol>
            <li>Saw a hidden network in airodump-ng (BSSID visible, SSID shows as <code>&lt;length: N&gt;</code>)</li>
            <li>Saw a client (STATION) associated with that BSSID</li>
            <li>Sent deauth frames to kick the client</li>
            <li>Client reconnected, sending probe requests with the SSID</li>
            <li>Captured the SSID and connected!</li>
        </ol>
    </div>
    
    <div class="section">
        <h3>The Vulnerability</h3>
        <p>WPA2 management frames (including deauth) are <strong>not authenticated</strong>.</p>
        <p>Anyone in range can forge a deauth frame - no password needed.</p>
        <p>When clients reconnect to hidden networks, they reveal the SSID in probe requests.</p>
    </div>
    
    <div class="section">
        <h3>The Commands</h3>
        <p><code>airodump-ng wlan0mon</code> - see hidden network + connected client</p>
        <p><code>aireplay-ng -0 5 -a [AP_MAC] -c [CLIENT_MAC] wlan0mon</code> - send deauths</p>
        <p>Watch for SSID to appear in airodump or Wireshark probe requests</p>
    </div>
    
    <div class="section">
        <h3>WPA3 Fixes This</h3>
        <p>Protected Management Frames (802.11w) cryptographically sign deauth frames.</p>
        <p>Spoofed deauths are rejected. But WPA3 adoption is still limited.</p>
    </div>
</body>
</html>
)";

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
    Serial.println("\nDeauth Challenge AP starting...");

    // Hidden open AP - deauth client to see SSID in probe requests
    WiFi.softAP(ssid, NULL, 1, true);  // channel 1, hidden=true
    Serial.print("Hidden AP started. IP: ");
    Serial.println(WiFi.softAPIP());
    Serial.print("SSID (hidden): ");
    Serial.println(ssid);
    Serial.print("Password: ");
    Serial.println(password);

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
