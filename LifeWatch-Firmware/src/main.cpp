#include <Arduino.h>
#include <esp_log.h>

#include "myWifi.h"
#include "initSetup.h"

const char* TAG = "main";

void setup() {
	Serial.setDebugOutput(true);
	Serial.begin(115200);
	delay(2000);
	ESP_LOGI(TAG, "setup started.");
	ESP_LOGI(TAG, "ID: %s", _wifi.getID());

	while (1) {
		if (_wifi.init()) {
			break;
		}
		else {
			_setup.init();
		}
	}

	ESP_LOGI(TAG, "setup done.\n");
}

void loop() {
	ESP_LOGI(TAG, ".");
	delay(5000);
}