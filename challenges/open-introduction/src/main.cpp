#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

const char* ssid = "Codefreeze Wifi Challenge #1";

DNSServer dnsServer;
ESP8266WebServer webServer(80);

const char* portalPage = R"(
<!DOCTYPE html>
<html>
<head>
    <title>Welcome!</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: sans-serif; text-align: center; padding: 50px; }
        h1 { color: #333; }
    </style>
</head>
<body>
    <h1>Welcome to Codefreeze Wifi Challenge #1!</h1>
    <p>Good things are happening.</p>
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
    pinMode( D4, OUTPUT );
    digitalWrite( D4, HIGH );
}

void setup() {
    turnOffBuiltInLed();

    Serial.begin( 74880 );
    Serial.println("Alive!");

    WiFi.softAP(ssid);
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());

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