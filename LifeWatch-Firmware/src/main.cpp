#include <Arduino.h>
#include <esp_log.h>

#include "myWifi.h"
#include "initSetup.h"
#include "rtc.h"

const char* TAG = "main";

RTC_DATA_ATTR uint32_t wake_cnt = 0;

void setup() {
	Serial.setDebugOutput(true);
	Serial.begin(115200);
	delay(2000);
	ESP_LOGI(TAG, "setup started.");
	ESP_LOGI(TAG, "ID: %s", _wifi.getID());

	/* connect to wifi or start initial config */ 
	while (1) {
		if (_wifi.init()) {
			break;
		}
		else {
			_setup.init();
		}
	}

	Wire.begin(MB_SDA, MB_SCL, 400000);

	/* check rtc and sync if needed */
	if (wake_cnt == 0) {
		rtc.init();
	}
	else {
		if (!rtc.check()) {
			rtc.sync();
		}
		rtc.clearAlarm();
	}

	ESP_LOGI(TAG, "time: %2d:%2d", rtc.getH(), rtc.getM());

	/* enable PCINT and go to deep sleep */
	ESP_LOGI(TAG, "setup done. going to sleep...\n");
	wake_cnt++;
	pinMode(RTC_INT, INPUT);
	esp_sleep_enable_ext0_wakeup(GPIO_NUM_7, LOW);
	esp_deep_sleep_start();
	ESP_LOGE(TAG, "This shouldn't get printed");
}

void loop() {}