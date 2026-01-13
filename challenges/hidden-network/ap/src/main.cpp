#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

// Hidden SSID - won't broadcast, but clients probe for it in plaintext!
const char* ssid = "Codefreeze_Secret_Network";

DNSServer dnsServer;
ESP8266WebServer webServer(80);

const char* portalPage = R"(
<!DOCTYPE html>
<html>
<head>
    <title>Challenge #2 - Hidden Network</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: sans-serif; text-align: center; padding: 50px; background: #1a1a2e; color: #eee; }
        h1 { color: #e94560; }
        .hint { background: #16213e; padding: 20px; border-radius: 8px; margin: 20px auto; max-width: 500px; }
        code { background: #0f3460; padding: 2px 8px; border-radius: 4px; }
    </style>
</head>
<body>
    <h1>Challenge #2: Hidden Network</h1>
    <p>You found the hidden network!</p>
    <div class="hint">
        <h3>The Vulnerability</h3>
        <p>Hidden SSIDs don't broadcast, but when YOUR device connects, 
           it sends <strong>probe requests in plaintext</strong> revealing the SSID.</p>
        <p>Once an attacker captures the SSID, they can:</p>
        <ul style="text-align: left; display: inline-block;">
            <li><strong>Connect directly</strong> - no password means anyone can join</li>
            <li><strong>Create an evil twin</strong> - fake AP intercepts real client traffic</li>
        </ul>
        <p style="margin-top: 15px;">Worse: your devices broadcast this SSID <em>everywhere you go</em>, 
           leaking it far beyond the AP's range.</p>
        <p>SSID: <code>Codefreeze_Secret_Network</code></p>
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
    Serial.println("Hidden Network AP starting...");

    // Create hidden AP: softAP(ssid, password, channel, hidden)
    // hidden=true means SSID won't appear in network scans
    WiFi.softAP(ssid, NULL, 1, true);
    Serial.print("Hidden AP started. IP: ");
    Serial.println(WiFi.softAPIP());
    Serial.print("SSID (hidden): ");
    Serial.println(ssid);

    // DNS server redirects all domains to us
    dnsServer.start(53, "*", WiFi.softAPIP());

    // Web server
    webServer.on("/", handleRoot);
    webServer.onNotFound(handleNotFound);
    webServer.begin();
    Serial.println("Captive portal started");
}

void loop() {
    dnsServer.processNextRequest();
    webServer.handleClient();
}
