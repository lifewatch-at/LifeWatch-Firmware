#include <Arduino.h>
#include <esp_log.h>
#include "myDisplay_750.h"
#include <SPI.h>

// #include "json.h"
// #include "mqtt.h"
// #include "myWifi.h"

// MyWiFi myWifi;
// Telemetry tel("Raum111", WiFi.macAddress(), 12930834, 55);
// Sensor temp("Temperature", 25.4, "C");
// Sensor hum("Humidity", 44, "%");
// Device device;
// MQTT mqtt;
myDisplay_750 _display;

const char* TAG = "main";

void setup() {
	Serial.setDebugOutput(true);
	Serial.begin(115200);
	delay(1000);
	ESP_LOGI(TAG, "setup started.\n");
   _display.init();
    


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
	delay(1000);
}