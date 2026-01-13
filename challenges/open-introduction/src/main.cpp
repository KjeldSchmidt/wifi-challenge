#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

const char* ssid = "Codefreeze_Intro";

DNSServer dnsServer;
ESP8266WebServer webServer(80);

const char* portalPage = R"(
<!DOCTYPE html>
<html>
<head>
    <title>WiFi Security Challenge</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: sans-serif; text-align: center; padding: 30px; background: #1a1a2e; color: #eee; }
        h1 { color: #e94560; }
        .section { background: #16213e; padding: 20px; border-radius: 8px; margin: 20px auto; max-width: 600px; text-align: left; }
        h3 { color: #e94560; margin-top: 0; }
        code { background: #0f3460; padding: 2px 8px; border-radius: 4px; }
        ul { margin: 10px 0; padding-left: 20px; }
        li { margin: 8px 0; }
    </style>
</head>
<body>
    <h1>WiFi Security Challenge</h1>
    <p>Welcome! You connected to an open network - that was the easy one.</p>
    
    <div class="section">
        <h3>The Networks</h3>
        <table style="width: 100%; border-collapse: collapse; font-size: 0.95em;">
            <tr style="border-bottom: 1px solid #0f3460;">
                <td style="padding: 8px 0;"><code>Codefreeze_Intro</code></td>
                <td style="color: #4ade80;">Open (you're here!)</td>
            </tr>
            <tr style="border-bottom: 1px solid #0f3460;">
                <td style="padding: 8px 0;"><code>(Hidden)</code></td>
                <td>Open - find the SSID</td>
            </tr>
            <tr style="border-bottom: 1px solid #0f3460;">
                <td style="padding: 8px 0;"><code>Codefreeze_Secure_WiFi</code></td>
            </tr>
            <tr style="border-bottom: 1px solid #0f3460;">
                <td style="padding: 8px 0;"><code>Codefreeze_Find_Me</code></td>
                <td>Password: on the device</td>
            </tr>
            <tr style="border-bottom: 1px solid #0f3460;">
                <td style="padding: 8px 0;"><code>Codefreeze_MAC_Filtered</code></td>
                <td>Password: <code>letmein123</code> + MAC Address Filtering</td>
            </tr>
            <tr>
                <td style="padding: 8px 0;"><code>(Hidden)</code></td>
                <td>Open - deauth client to reveal SSID</td>
            </tr>
        </table>
    </div>
    
    <div class="section">
        <h3>Recommended Setup</h3>
        <ul>
            <li><strong>Linux</strong> - WiFi attacks work best here (monitor mode support)</li>
            <li><strong>Wireshark</strong> - packet capture and analysis</li>
            <li><strong>aircrack-ng</strong> - WiFi auditing suite (airodump-ng, aireplay-ng, aircrack-ng)</li>
            <li><strong>rockyou.txt</strong> - password wordlist for dictionary attacks</li>
        </ul>
        <p style="margin-top: 15px; font-size: 0.9em; color: #888;">
            macOS can do passive sniffing. Windows needs special hardware.
        </p>
    </div>
    
    <div class="section">
        <h3>Phone Apps</h3>
        <p>For signal strength tracking (finding hidden devices):</p>
        <ul>
            <li><strong>Android (tested):</strong> WiFiAnalyzer (open-source)</li>
            <li><strong>iOS (untested):</strong> Network Analyzer Lite</li>
        </ul>
    </div>
    
    <div class="section">
        <h3>Quick Reference</h3>
        <p><code>sudo airmon-ng start wlan0</code> - enable monitor mode</p>
        <p><code>sudo airodump-ng wlan0mon</code> - scan networks</p>
        <p><code>aircrack-ng -w rockyou.txt capture.cap</code> - crack WPA</p>
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