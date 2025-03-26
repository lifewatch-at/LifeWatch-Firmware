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


myDisplay _display;
BQ25792 charger(PWR_INT, -1);

const char* TAG = "main";

RTC_DATA_ATTR uint32_t	wake_cnt = 0;
RTC_DATA_ATTR uint8_t	refresh_count = 0;
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

inline void display_refresh() {
	refresh_count++;

	char secc_1[15];
	sprintf(secc_1, "%3.1f deg C", temp);
	char secc_2[15];
	sprintf(secc_2, "%3.1f %rH", hum);

	_display.updateTextbox(secc_1, secc_2, charger.batteryLow(), _setup.isRunning(), _wifi.getID());
	_display.updateBars("CO" , co_ppm , 0, 80 ,
						"PM" , pm2_5  , 0, 500,
						"NOx", srawNox, 0, 100);

	if (refresh_count >= 5) {
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

inline void mqtt_publish() {
	wifi_routine();
	if (!mqtt.connect()) {return;}
	Telemetry tel
	(	
		_wifi.getID(),
		_setup.getParam(PARAM_NAME),
		rtc.now().unixtime(),
		charger.getPercentage()
	);
	Sensor temp_sensor (modules[0].name, temp  , modules[0].unit);
	Sensor hum_sensor  (modules[1].name, hum   , modules[1].unit);
	Sensor co2_sensor  (modules[2].name, co2   , modules[2].unit);
	Sensor co_sensor   (modules[4].name, co_ppm, modules[4].unit);
	Sensor pm_sensor   (modules[5].name, pm2_5 , modules[5].unit);
	Sensor tvoc_sensor (modules[6].name, (float)tvoc  , modules[6].unit);
	Sensor nox_sensor  (modules[8].name, (float)srawNox, modules[8].unit);
	Sensor noise_sensor(modules[9].name, noise_db, modules[9].unit);
	Device device;
	device.telemetry = &tel;
	device.add(&temp_sensor );
	device.add(&hum_sensor  );
	device.add(&co2_sensor  );
	device.add(&co_sensor   );
	device.add(&pm_sensor   );
	device.add(&tvoc_sensor );
	device.add(&nox_sensor  );
	device.add(&noise_sensor);
	mqtt.send(device);
	mqtt.loop();
}

void print_values(void) {
	ESP_LOGI(TAG, "\ntemp =  %5.1f deg C", temp);
	ESP_LOGI(TAG, "hum = \t%5.1f %rH", hum);
	ESP_LOGI(TAG, "PM = \t%5.1f ppm", pm2_5);
	ESP_LOGI(TAG, "CO = \t%5.1f ppm", co_ppm);
	ESP_LOGI(TAG, "noise = \t%d dB", noise_db);
	ESP_LOGI(TAG, "TVOC = \t%d ppb", tvoc);
	ESP_LOGI(TAG, "CO2 = \t%5.1f µg/m^3", co2);
	ESP_LOGI(TAG, "NOx = \t%d ppb\n", srawNox);
}

void setup() {
	Serial.setDebugOutput(true);
	Serial.begin(115200);
	Wire.begin(MB_SDA, MB_SCL, 400000);
	Wire1.begin(CB_SDA, CB_SCL, 400000);
	delay(2000);												// temporary delay to make debugging easier
	ESP_LOGI(TAG, "setup started. wake count: %d", wake_cnt);
	ESP_LOGI(TAG, "ID: %s", _wifi.getID());
	_display.init(wake_cnt==0);
}

void loop() {
	rtc_routine();
	module_check();
	display_refresh();
	print_values();
	mqtt_publish();
	
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