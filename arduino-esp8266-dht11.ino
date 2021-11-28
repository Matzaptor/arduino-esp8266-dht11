#include "ArduinoJson.h";

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
ESP8266WebServer server(80);

// DHT Temperature & Humidity Sensor
// Unified Sensor Library Example
// Written by Tony DiCola for Adafruit Industries
// Released under an MIT license.

// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 2     // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment the type of sensor in use:
#define DHTTYPE    DHT11     // DHT 11
//#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

// See guide for details on sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview

DHT_Unified dht(DHTPIN, DHTTYPE);

const char* ssid   = "Matzaptor Wi-Fi";
const char* password = "abcde12345";
const char* hostname = "ESP8266_DHT11";

void initWiFi() {
	WiFi.mode(WIFI_STA);
	WiFi.hostname(hostname);
	WiFi.begin(ssid, password);
	Serial.print("Connecting to WiFi ..");
	while (WiFi.status() != WL_CONNECTED) {
		Serial.print('.');
		delay(1000);
	}
	Serial.println(WiFi.localIP());
	//The ESP8266 tries to reconnect automatically when the connection is lost
	WiFi.setAutoReconnect(true);
	WiFi.persistent(true);
}
void handle_NotFound() {
	server.send(404, "text/plain", "404 - Not found");
}

DynamicJsonDocument me(4096);
void handle_index() {
	String response = "";
	serializeJson(me, response);
	server.send(200, "application/json", response);
}

uint32_t delayMS = 0;
void setup() {
	Serial.begin(9600);

	initWiFi();
	server.onNotFound(handle_NotFound);
	server.on("/", handle_index);
	server.begin();

	// Initialize device.
	dht.begin();

	me["status"] = true;

	Serial.println(F("DHTxx Unified Sensor Example"));
	// Print temperature sensor details.
	sensor_t sensor;

	dht.temperature().getSensor(&sensor);
	me["response"]["sensors"]["temperature"]["name"] = sensor.name;
	me["response"]["sensors"]["temperature"]["version"] = sensor.version;
	me["response"]["sensors"]["temperature"]["sensor_id"] = sensor.sensor_id;
	me["response"]["sensors"]["temperature"]["max_value"] = sensor.max_value;
	me["response"]["sensors"]["temperature"]["min_value"] = sensor.min_value;
	me["response"]["sensors"]["temperature"]["resolution"] = sensor.resolution;

	// Print humidity sensor details.
	dht.humidity().getSensor(&sensor);
	me["response"]["sensors"]["humidity"]["name"] = sensor.name;
	me["response"]["sensors"]["humidity"]["version"] = sensor.version;
	me["response"]["sensors"]["humidity"]["sensor_id"] = sensor.sensor_id;
	me["response"]["sensors"]["humidity"]["max_value"] = sensor.max_value;
	me["response"]["sensors"]["humidity"]["min_value"] = sensor.min_value;
	me["response"]["sensors"]["humidity"]["resolution"] = sensor.resolution;

	delayMS = sensor.min_delay / 1000;
}

sensors_event_t event;
uint32_t lastUpdate = 0;
void loop() {
	if (lastUpdate = 0 || millis() > lastUpdate + delayMS) {
		dht.temperature().getEvent(&event);
		me["response"]["sensors"]["temperature"]["value"] = event.temperature;
		me["response"]["sensors"]["temperature"]["ts"] = millis();
		dht.humidity().getEvent(&event);
		me["response"]["sensors"]["humidity"]["value"] = event.relative_humidity;
		me["response"]["sensors"]["humidity"]["ts"] = millis();
	}

	server.handleClient();
}
