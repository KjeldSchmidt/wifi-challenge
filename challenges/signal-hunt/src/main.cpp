#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

const char* ssid = "Codefreeze_Find_Me";
const char* password = "OnTheSticker";  // Written on the device!

DNSServer dnsServer;
ESP8266WebServer webServer(80);

const char* portalPage = R"(
<!DOCTYPE html>
<html>
<head>
    <title>Found!</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: sans-serif; text-align: center; padding: 30px; background: #1a1a2e; color: #eee; }
        h1 { color: #4ade80; font-size: 3em; }
        .section { background: #16213e; padding: 20px; border-radius: 8px; margin: 20px auto; max-width: 500px; text-align: left; }
        h3 { color: #e94560; margin-top: 0; }
        code { background: #0f3460; padding: 2px 8px; border-radius: 4px; }
    </style>
</head>
<body>
    <h1>You found me!</h1>
    <p>You tracked down the hidden access point and read the password off the sticker.</p>
    
    <div class="section">
        <h3>Default Passwords on Stickers</h3>
        <p>Most home routers ship with a <strong>unique default password printed on a sticker</strong> on the device.</p>
        <p>This is convenient but means: <strong>physical access = WiFi access</strong>.</p>
        <p>Anyone who can see your router (visitors, maintenance workers, burglars) can photograph the sticker.</p>
        <p style="color: #888; font-size: 0.9em;">Always change default passwords!</p>
    </div>
    
    <div class="section">
        <h3>The Technique</h3>
        <p>WiFi signal strength (RSSI in dBm) indicates proximity:</p>
        <ul>
            <li><strong>-30 dBm</strong> - excellent, very close</li>
            <li><strong>-50 dBm</strong> - good</li>
            <li><strong>-70 dBm</strong> - weak</li>
            <li><strong>-90 dBm</strong> - barely detectable</li>
        </ul>
        <p>Apps like <code>WiFi Analyzer</code> show this in real-time.</p>
    </div>
    
    <div class="section">
        <h3>Physical Access = Game Over</h3>
        <p>Finding network hardware matters because:</p>
        <ul>
            <li>Factory reset → default credentials</li>
            <li>Ethernet ports → direct network access</li>
            <li>USB/serial ports → firmware extraction</li>
        </ul>
    </div>
    
    <div class="section">
        <h3>Remember WPS?</h3>
        <p>Many routers had a <strong>WPS button</strong> - press it to allow passwordless connections for 2 minutes.</p>
        <p>Even worse: <strong>WPS PIN mode</strong> used an 8-digit code that could be brute-forced in hours 
           (only ~11,000 effective combinations due to bad design).</p>
        <p>Tools like <code>Reaver</code> and <code>Bully</code> exploited this.</p>
        <p style="color: #888; font-size: 0.9em;">Most modern routers have WPS disabled or rate-limited now.</p>
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
    Serial.println("Signal Hunt AP starting...");

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
    Serial.println("Captive portal started - hide me!");
}

void loop() {
    dnsServer.processNextRequest();
    webServer.handleClient();
}
