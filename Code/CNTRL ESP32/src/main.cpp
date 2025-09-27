#include <Arduino.h>

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <LittleFS.h>

#include <ArduinoJson.h>

#include "stm32_i2c.h"

#define AP_SSID "NAVION-FC"
#define AP_PASSWORD "12345678"

AsyncWebServer server(80);
AsyncWebSocket webSocket("/socket");

void handleCommand(AsyncWebSocketClient *client, String cmd);
void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

void sendRocketData(AsyncWebSocket *server);

unsigned long last_send_time = 0;
const unsigned long send_interval_ms = 50;

rocket_data r_data;

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

	pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
	requestStatePacket();
	
	unsigned long now = millis();
    if (now - last_send_time >= send_interval_ms) {
		last_send_time = now;

		sendRocketData(&webSocket);
	}
}

void sendRocketData(AsyncWebSocket *server) {
	JsonDocument doc;

    doc["T_plus"] = r_data.T_plus;
    
    JsonObject flags = doc["flags"].to<JsonObject>();
	flags["calibrated"] = r_data.flags.calibrated;
    flags["armed"] = r_data.flags.armed;
    flags["ignition"] = r_data.flags.ignition;
    flags["apogee"] = r_data.flags.apogee;
    flags["touchdown"] = r_data.flags.touchdown;

	doc["state"] = r_data.state;
    
    doc["vbat"] = r_data.vbat;

    JsonObject acc = doc["acc"].to<JsonObject>();
    acc["x"] = r_data.acc.x;
    acc["y"] = r_data.acc.y;
    acc["z"] = r_data.acc.z;

    JsonObject gyr = doc["gyr"].to<JsonObject>();
    gyr["x"] = r_data.gyr.x;
    gyr["y"] = r_data.gyr.y;
    gyr["z"] = r_data.gyr.z;

    JsonObject mag = doc["mag"].to<JsonObject>();
    mag["x"] = r_data.mag.x;
    mag["y"] = r_data.mag.y;
    mag["z"] = r_data.mag.z;

    JsonObject quat = doc["quat"].to<JsonObject>();
    quat["w"] = r_data.quat.w;
    quat["x"] = r_data.quat.x;
    quat["y"] = r_data.quat.y;
    quat["z"] = r_data.quat.z;

    doc["temperature"] = r_data.temperature;
    doc["pressure"] = r_data.pressure;
    doc["altitude"] = r_data.altitude;
    doc["v_velocity"] = r_data.v_velocity;
    doc["v_accel"] = r_data.v_accel;

    JsonObject tvc = doc["tvc"].to<JsonObject>();
    tvc["x"] = r_data.tvc.x;
    tvc["y"] = r_data.tvc.y;

    JsonObject pyro = doc["pyro"].to<JsonObject>();
    pyro["motor"] = r_data.pyro.motor;
    pyro["parachute"] = r_data.pyro.parachute;

    char buffer[512];
    size_t len = serializeJson(doc, buffer);

    server->textAll(buffer, len);
}

void handleCommand(AsyncWebSocketClient *client, String cmd) {
	if (cmd == "arm") {
		Serial.println("[CMD] Arm command received");
		
		esp32_instruction instruction;
		instruction.type = ESP32_ARM;
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
