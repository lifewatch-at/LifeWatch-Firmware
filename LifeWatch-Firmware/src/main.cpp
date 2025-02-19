#include <Arduino.h>
#include <esp_log.h>

//#include "myWifi.h"
//#include "mqtt.h"
//#include "myDisplay.h"
#include "initSetup.h"

//Telemetry tel("Raum111", WiFi.macAddress(), 12930834, 55);
//Sensor temp("Temperature", 25.4, "C");
//Sensor hum("Humidity", 44, "%");
//Device device;
//MQTT mqtt;
//myDisplay _display;

const char* TAG = "main";

void setup() {
	Serial.setDebugOutput(true);
	Serial.begin(115200);
	delay(2000);
	ESP_LOGI(TAG, "setup started.\n");
	ESP_LOGI(TAG, "ID: %s", _wifi.getID());

	_setup.initSetup();
//	_display.init();

	// device.telemetry = &tel;
	// device.add(&temp);
	// device.add(&hum);

	// myWifi.init();
	// mqtt.mqttConnect();
	// mqtt.mqttSend(device);

	ESP_LOGI(TAG, "setup done.\n");
}

void loop() {
	ESP_LOGI(TAG, ".");

	delay(5000);
}