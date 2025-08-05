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
		
		esp32_instruction instruction;
		instruction.type = ESP32_LAUNCH;
		instruction.payload_size = 0;

		sendInstructionPacket(&instruction);
	} else if (cmd.startsWith("servo_x:")) {
		float pos = cmd.substring(8).toFloat();
		Serial.printf("[CMD] TVC Servo X Position: %.2f\n", pos);

		esp32_instruction instruction;
		instruction.type = ESP32_TVC_SERVO_X_POS;
		instruction.payload_size = 4;
		memcpy(instruction.payload, &pos, sizeof(pos));

		sendInstructionPacket(&instruction);
	} else if (cmd.startsWith("servo_y:")) {
		float pos = cmd.substring(8).toFloat();
		Serial.printf("[CMD] TVC Servo Y Position: %.2f\n", pos);

		esp32_instruction instruction;
		instruction.type = ESP32_TVC_SERVO_Y_POS;
		instruction.payload_size = 4;
		memcpy(instruction.payload, &pos, sizeof(pos));

		sendInstructionPacket(&instruction);
	} else if (cmd.startsWith("tvc_deflect:")) {
		float deflectionX = cmd.substring(12, cmd.indexOf(",")).toFloat();
		float deflectionY = cmd.substring(cmd.indexOf(",") + 1).toFloat();
		Serial.printf("[CMD] TVC Deflection Position: %.2f, %.2f\n", deflectionX, deflectionY);

		esp32_instruction instruction;
		instruction.type = ESP32_TVC_DEFLECTION_POS;
		instruction.payload_size = 8;
		memcpy(instruction.payload, &deflectionX, sizeof(deflectionX));
		memcpy(instruction.payload + sizeof(deflectionX), &deflectionY, sizeof(deflectionY));

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
