#include <Arduino.h>
#include <esp_log.h>

#include "myDisplay.h"


myDisplay _display;

const char* TAG = "main";

void setup() {
	Serial.setDebugOutput(true);
	Serial.begin(115200);
	delay(1000);
	ESP_LOGI(TAG, "setup started.\n");

	_display.init();

	ESP_LOGI(TAG, "setup done.\n");
}

void loop() {
	ESP_LOGI(TAG, ".");
	delay(1000);
}