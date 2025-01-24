#include <Arduino.h>
#include <esp_log.h>
#include "myDisplay.h"
#include <SPI.h>

#include "Adafruit_SHT4x.h"
// #include "json.h"
// #include "mqtt.h"
// #include "myWifi.h"

// MyWiFi myWifi;
// Telemetry tel("Raum111", WiFi.macAddress(), 12930834, 55);
// Sensor temp("Temperature", 25.4, "C");
// Sensor hum("Humidity", 44, "%");
// Device device;
// MQTT mqtt;

myDisplay _display;
Adafruit_SHT4x temp_hum = Adafruit_SHT4x();
sensors_event_t humidity, temp;
short zaeler = 1;

char charBuf[6];
const char c[] = " C";
const char chum_einh[] = " %rH";

int stunden = 1; 
int minuten = 5;

const char* TAG = "main";

void setup() {
	Serial.setDebugOutput(true);
	Serial.begin(115200);
	ESP_LOGI(TAG, "setup started.\n");
   _display.init();

	// device.telemetry = &tel;
	// device.add(&temp);
	// device.add(&hum);

	// myWifi.init();
	// mqtt.mqttConnect();
	// mqtt.mqttSend(device);

	if (!temp_hum.begin()) {
		Serial.println("Couldn't find SHT4x");
		while (1) delay(1);
	}
	Serial.println("Found SHT4x sensor");

	temp_hum.setPrecision(SHT4X_HIGH_PRECISION);
	temp_hum.setHeater(SHT4X_NO_HEATER);
	
	ESP_LOGI(TAG, "setup done.\n");

}

void loop() {
	ESP_LOGI(TAG, ".");

	temp_hum.getEvent(&humidity, &temp);

	delay(1000);

	dtostrf(temp.temperature,7, 1, charBuf);
	strcat(charBuf, c);
	_display.updateSpace_1(charBuf);

	dtostrf(humidity.relative_humidity,7, 1, charBuf);
	strcat(charBuf, chum_einh);
	_display.updateSpace_2(charBuf);

	_display.updateTime(stunden, minuten, true);

	zaeler++;

	if (zaeler >= 10) { 
		_display.refresh(); 
		zaeler = 1;
	}

	delay(19000);
}