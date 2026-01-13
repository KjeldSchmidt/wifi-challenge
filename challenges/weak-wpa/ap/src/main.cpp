#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

const char* ssid = "Codefreeze_Secure_WiFi";
const char* password = "sunshine1";  // Weak! In rockyou.txt wordlist

DNSServer dnsServer;
ESP8266WebServer webServer(80);

const char* portalPage = R"(
<!DOCTYPE html>
<html>
<head>
    <title>Challenge #3 - Weak WPA</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: sans-serif; text-align: center; padding: 50px; background: #1a1a2e; color: #eee; }
        h1 { color: #e94560; }
        .hint { background: #16213e; padding: 20px; border-radius: 8px; margin: 20px auto; max-width: 500px; }
        code { background: #0f3460; padding: 2px 8px; border-radius: 4px; }
    </style>
</head>
<body>
    <h1>Challenge #3: Weak WPA Password</h1>
    <p>You cracked the password!</p>
    <div class="hint">
        <h3>The Vulnerability</h3>
        <p>WPA2 is cryptographically strong, but <strong>weak passwords</strong> defeat it entirely.</p>
        <h4>The Attack:</h4>
        <ol style="text-align: left; display: inline-block;">
            <li>Capture the 4-way handshake (when client connects)</li>
            <li>Run dictionary attack with aircrack-ng or hashcat</li>
            <li>Common passwords crack in seconds</li>
        </ol>
        <p style="margin-top: 15px;">Password was: <code>sunshine1</code></p>
        <p>Tools: <code>airodump-ng</code>, <code>aircrack-ng</code>, <code>hashcat</code></p>
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
    Serial.println("Weak WPA AP starting...");

    WiFi.softAP(ssid, password);
    Serial.print("AP started. IP: ");
    Serial.println(WiFi.softAPIP());
    Serial.print("SSID: ");
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
