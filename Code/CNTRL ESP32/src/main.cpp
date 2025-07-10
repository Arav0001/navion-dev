#include <Arduino.h>

#include <WiFi.h>
#include <WebServer.h>

#define AP_SSID "NAVION-FC"
#define AP_PASSWORD "12345678"

WebServer server(80);

void handleRoot() {
  server.send(200, "text/html", R"rawliteral(
    <h1>NAVION Launch Control</h1>
    <button onclick="fetch('/launch')">Launch</button>
  )rawliteral");
}

void handleLaunch() {
  server.send(200, "text/plain", "Launch command sent.");
  Serial.println("Launch command received.");
}

void setup() {
	Serial.begin(115200);
  	WiFi.softAP(AP_SSID, AP_PASSWORD);
  	Serial.println("AP started at " + WiFi.softAPIP().toString());

  	server.on("/", handleRoot);
  	server.on("/launch", handleLaunch);
  	server.begin();
}

void loop() {
	server.handleClient();
}