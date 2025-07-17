#include <Arduino.h>

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <LittleFS.h>

#define AP_SSID "NAVION-FC"
#define AP_PASSWORD "12345678"

AsyncWebServer server(80);
AsyncWebSocket webSocket("/socket");

void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
	String msg = "";
	
	switch (type) {
		case WS_EVT_CONNECT:
			Serial.printf("WebSocket client connected: %u\n", client->id());
			break;
		case WS_EVT_DISCONNECT:
			Serial.printf("WebSocket client disconnected: %u\n", client->id());
			break;
		case WS_EVT_DATA:
			for (size_t i = 0; i < len; i++) msg += (char)data[i];

			Serial.printf("[WS] Received from client %u: %s\n", client->id(), msg.c_str());

			if (msg == "launch") {
				Serial.println("[CMD] Launch command received");
				client->text("Launch command acknowledged");
			} else {
				Serial.printf("[CMD] Unknown command: %s\n", msg.c_str());
			}

			break;
		default:
			break;
	}
}

void setup() {
	Serial.begin(115200);

	if (!LittleFS.begin()) {
		Serial.println("LittleFS mount failed");
		return;
	}

  	WiFi.softAP(AP_SSID, AP_PASSWORD);
  	Serial.println("AP started at " + WiFi.softAPIP().toString());

	server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

	webSocket.onEvent(onWebSocketEvent);
	server.addHandler(&webSocket);

  	server.begin();
}

void loop() {
	// Nothing to do here
}
