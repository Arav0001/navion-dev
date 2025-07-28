#include <Arduino.h>

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <LittleFS.h>

#include "stm32_i2c.h"

#define AP_SSID "NAVION-FC"
#define AP_PASSWORD "12345678"

AsyncWebServer server(80);
AsyncWebSocket webSocket("/socket");

void handleCommand(AsyncWebSocketClient *client, String cmd);
void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

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

	initializeI2C();
}

void loop() {
	// Nothing to do here
}

void handleCommand(AsyncWebSocketClient *client, String cmd) {
	if (cmd == "launch") {
		Serial.println("[CMD] Launch command received");
		client->text("Launch command acknowledged");
		
		esp32_instruction instruction;
		instruction.type = ESP32_LAUNCH;
		instruction.payload_size = 0;

		sendInstructionPacket(&instruction);
	} else {
		Serial.printf("[CMD] Unknown command: %s\n", cmd.c_str());
	}
}

void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
	String cmd = "";
	
	switch (type) {
		case WS_EVT_CONNECT:
			Serial.printf("[WS] WebSocket client connected: %u\n", client->id());
			break;
		case WS_EVT_DISCONNECT:
			Serial.printf("[WS] WebSocket client disconnected: %u\n", client->id());
			break;
		case WS_EVT_DATA:
			for (size_t i = 0; i < len; i++) cmd += (char)data[i];

			Serial.printf("[WS] Received from client %u: %s\n", client->id(), cmd.c_str());

			handleCommand(client, cmd);
			break;
		default:
			break;
	}
}
