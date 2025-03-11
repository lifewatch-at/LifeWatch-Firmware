#include <Arduino.h>
#include <esp_log.h>

#include "pins.h"
#include "myWifi.h"
#include "initSetup.h"
#include "rtc.h"

const char* TAG = "main";

RTC_DATA_ATTR uint32_t wake_cnt = 0;


inline void wifi_loop() {
	while (1) {
		if (_wifi.init()) {
			break;
		}
		else {
			_setup.init();
		}
	}
}

inline void rtc_loop() {
    if(!rtc.begin(&Wire)) {
        ESP_LOGE(TAG, "Couldn't find RTC");
        return;
    }
	setenv("TZ", _setup.getParam(PARAM_TZ_OFFSET).c_str(), 1);
	if (wake_cnt == 0) {
		rtc.init();
	}
	else {
		rtc.clearAlarm(2);
		if (rtc.check()) {
			rtc.sync();
		}
	}

	ESP_LOGI(TAG, "time: %2d:%2d", rtc.getH(), rtc.getM());
}

inline void gettosleep() {
	ESP_LOGI(TAG, "setup done. going to sleep...\n");
	wake_cnt++;
	pinMode(RTC_INT, INPUT);
	esp_sleep_enable_ext0_wakeup(GPIO_NUM_7, LOW);
	esp_deep_sleep_start();
	ESP_LOGE(TAG, "This shouldn't get printed");
}

void setup() {
	Serial.setDebugOutput(true);
	Serial.begin(115200);
	delay(2000);
	ESP_LOGI(TAG, "setup started.");
	ESP_LOGI(TAG, "ID: %s", _wifi.getID());
	wifi_loop();
	Wire.begin(MB_SDA, MB_SCL, 400000);
	rtc_loop();
	gettosleep();
}

void loop() {}