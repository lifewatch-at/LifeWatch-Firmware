#include <Arduino.h>

#include "json.h"
#include "mqtt.h"
#include "WiFi.h"

Telemetry tel("Raum111", WiFi.macAddress(), 12930834, 55);
Sensor temp("Temperature", 25.4, "C");
Sensor hum("Humidity", 44, "%");
Device device;
MQTT mqtt;

void initWiFi() {
	WiFi.mode(WIFI_STA);
	WiFi.begin(WIFI_SSID, WIFI_PWD);
	Serial.print("Connecting to WiFi ..");
	while (WiFi.status() != WL_CONNECTED) {
		Serial.print('.');
		delay(1000);
	}
}

void setup() {
	// put your setup code here, to run once:

	Serial.begin(115200);

	device.telemetry = &tel;
	device.add(&temp);
	device.add(&hum);

	initWiFi();
	mqtt.mqttConnect();
	mqtt.mqttSend(device);
}

void loop() {
	// put your main code here, to run repeatedly:
	
}