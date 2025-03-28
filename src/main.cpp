#include <Arduino.h>
#include <esp_log.h>

#include "pins.h"
#include "myWifi.h"
#include "initSetup.h"
#include "rtc.h"
#include "myDisplay.h"
#include "module.h"
#include "mqtt.h"	
#include "BQ25792_Driver.h"


#define PARTIAL_REFRESHS 5

myDisplay _display;
BQ25792 charger(PWR_INT, -1);

const char* TAG = "main";

RTC_DATA_ATTR uint32_t	wake_cnt = 0;
RTC_DATA_ATTR uint8_t	refresh_count = PARTIAL_REFRESHS;
RTC_DATA_ATTR bool		rtcResync = false;
volatile bool waaaiit;


void delay_loop_while_setup_isnt_done_ISR() {
	waaaiit = false;
}

void wifi_routine() {
	if (!_setup.isRunning()) {
		if (!_wifi.init()) {_setup.init();}
	}
}

inline void rtc_routine() {
    if(!rtc.begin(&Wire)) {
        ESP_LOGE(TAG, "Couldn't find RTC");
        return;
    }
	setenv("TZ", _setup.getParam(PARAM_TZ_OFFSET).c_str(), 1);
	if (wake_cnt==0 || rtcResync) {
		rtcResync = false;
		wifi_routine();
		rtc.init();
	}
	else {
		rtc.clearAlarm(2);
		if (rtc.check()) {
			wifi_routine();
			rtc.sync();
		}
	}
	_display.updateTime(rtc.getH(), rtc.getM(), true);
}

inline void display_refresh(Device& device) {
	refresh_count++;

	char secc_1[15];
	sprintf(secc_1, "%3.1f deg C", device.getValueof(TEMP_NAME));
	char secc_2[15];
	sprintf(secc_2, "%3.1f %rH", device.getValueof(HUM_NAME));

	_display.updateTextbox(secc_1, secc_2, charger.batteryLow(), _setup.isRunning(), _wifi.getID());
	_display.updateBars("CO" , device.getValueof(CO_NAME) , 0,   80,
						"CO2", device.getValueof(CO2_NAME), 0, 1000,
						"NOx", device.getValueof(NOX_NAME), 0,  100);

	if (refresh_count >= PARTIAL_REFRESHS) {
		_display.refresh();
		refresh_count = 0;
	}
					
	_display.powerOff();
}

inline void gotosleep() {
	ESP_LOGI(TAG, "going to sleep...\n");
	wake_cnt++;
	pinMode(RTC_INT, INPUT);
	esp_sleep_enable_ext0_wakeup(GPIO_NUM_7, LOW);
	esp_deep_sleep_start();
	ESP_LOGE(TAG, "This shouldn't get printed");
}

inline void mqtt_publish(Device& device) {
	wifi_routine();
	if (!mqtt.connect()) {return;}
	Telemetry tel
	(	
		_wifi.getID(),
		_setup.getParam(PARAM_NAME),
		rtc.now().unixtime(),
		charger.getPercentage()
	);
	device.telemetry = &tel;
	mqtt.send(device);
	mqtt.loop();
}


void setup() {
	Serial.setDebugOutput(true);
	Serial.begin(115200);
	Wire.begin(MB_SDA, MB_SCL, 400000);
	Wire1.begin(CB_SDA, CB_SCL, 400000);
	ESP_LOGI(TAG, "setup started. wake count: %d", wake_cnt);
	ESP_LOGI(TAG, "ID: %s", _wifi.getID());
	_display.init(wake_cnt==0);
}

void loop() {
	Device device;
	rtc_routine();
	read_modules(device);
	display_refresh(device);
	mqtt_publish(device);
	
	if (_setup.isDone(rtcResync) || !_setup.isRunning()) {
		gotosleep();
	}
	else {
		attachInterrupt(digitalPinToInterrupt(RTC_INT), delay_loop_while_setup_isnt_done_ISR, FALLING);
		waaaiit = true;
		while (waaaiit) {
			delay(5000);
			ESP_LOGI(TAG, "honse");
		}
	}
}