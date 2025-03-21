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
	ESP_LOGI(TAG, "localtime: %2d:%2d", rtc.getH(), rtc.getM());
}

inline void display_setup() {
	_display.init(wake_cnt==0);

	char secc_1[15];
	sprintf(secc_1, "%3.1f deg C", temp);
	char secc_2[15];
	sprintf(secc_2, "%3.1f %rH", hum);
	
	_display.updateEverything(rtc.getH(), rtc.getM(), 
								secc_1, secc_2, 
								"CO", co_ppm, 0, 15,
								"PM", pm2_5, 0, 100,
								"NOx", srawNox, 0, 100);
	_display.deepSleep();
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
	mqtt.connect();
	Telemetry tel(_wifi.getID(), _setup.getParam(PARAM_NAME), constrain(map(charger.getVBAT()*100,700,840,0,100),0,100));
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
	delay(2000);
	ESP_LOGI(TAG, "setup started. wake count: %d", wake_cnt);
	ESP_LOGI(TAG, "ID: %s", _wifi.getID());
	wifi_loop();
	Wire.begin(MB_SDA, MB_SCL, 400000);
	Wire1.begin(CB_SDA, CB_SCL, 400000);
	rtc_loop();
	module_check();
	display_setup();
	print_values();
	mqtt_publish();
	ESP_LOGI(TAG, "setup done.");
	gotosleep();
}

void loop() {}