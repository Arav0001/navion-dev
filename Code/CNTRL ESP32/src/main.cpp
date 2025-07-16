#include <Arduino.h>

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <LittleFS.h>

#define AP_SSID "NAVION-FC"
#define AP_PASSWORD "12345678"

AsyncWebServer server(80);

void handleLaunch(AsyncWebServerRequest *request);

void setup() {
	Serial.begin(115200);

	if (!LittleFS.begin()) {
		Serial.println("LittleFS mount failed");
		return;
	}

  	WiFi.softAP(AP_SSID, AP_PASSWORD);
  	Serial.println("AP started at " + WiFi.softAPIP().toString());

	server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

	server.on("/launch", HTTP_GET, handleLaunch);

  	server.begin();
}

void loop() {
	// Nothing to do here
}

void handleLaunch(AsyncWebServerRequest *request) {
	Serial.println("[CMD] Launch command received");
	request->send(200, "text/plain", "Launch command sent.");
}