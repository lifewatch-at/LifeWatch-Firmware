#include <Arduino.h>
#include <esp_log.h>

#include "pins.h"
#include "myWifi.h"
#include "initSetup.h"
#include "rtc.h"
#include "myDisplay.h"
#include "module.h"
#include "mqtt.h"	


myDisplay _display;

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
	_display.init();

	// char *secc_1;
	// sprintf(secc_1, "%3.1f deg C", &temp);
	// char *secc_2;
	// sprintf(secc_2, "%3.1f %rH", &hum);
	
	_display.updateEverything(rtc.getH(), rtc.getM(), 
								"ligma", "BALLS", 
								"CO", co_ppm, 0, 15,
								"TVOC", tvoc, 0, 1000,
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
	mqtt.mqttConnect();
	Telemetry tel("gggg","tgthdgfd",1234567,66);
	Sensor temp_sensor(modules[0].name, temp  , modules[0].unit);
	Sensor hum_sensor (modules[1].name, hum   , modules[1].unit);
	Sensor co_sensor  (modules[4].name, co_ppm, modules[4].unit);
	Sensor pm_sensor  (modules[5].name, pm2_5 , modules[5].unit);
	Sensor tvoc_sensor(modules[6].name, (float)tvoc  , modules[6].unit);
	Sensor nox_sensor (modules[8].name, (float)srawNox, modules[8].unit);
	Device device;
	device.telemetry = &tel;
	device.add(&temp_sensor);
	device.add(&hum_sensor );
	device.add(&co_sensor  );
	device.add(&pm_sensor  );
	device.add(&tvoc_sensor);
	device.add(&nox_sensor );
	String ssdf;
	serializeJson(device.toJSON(),ssdf);
	ESP_LOGI(TAG, "%s", ssdf.c_str());
	mqtt.mqttSend(device);
}

void setup() {
	Serial.setDebugOutput(true);
	Serial.begin(115200);
	delay(2000);
	ESP_LOGI(TAG, "setup started. wake count: %d", wake_cnt);
	ESP_LOGI(TAG, "ID: %s", _wifi.getID());
	wifi_loop();
	Wire.begin(MB_SDA, MB_SCL, 400000);
	Wire1.begin(8, 9, 400000);
	rtc_loop();
	delay(1000);
	module_check();
	display_setup();
	mqtt_publish();
	ESP_LOGI(TAG, "setup done.");
	gotosleep();
}

void loop() {
	// module_check();

	// // char *secc_1;
	// // sprintf(secc_1, "%3.1f deg C", &temp);
	// // char *secc_2;
	// // sprintf(secc_2, "%3.1f %rH", &hum);
	
	// _display.updateEverything(69, 11, 
	// 							"ligma", "balls", 
	// 							"CO", co_ppm, 0, 15,
	// 							"TVOC", tvoc, 0, 1000,
	// 							"CO2", srawNox, 0, 100);

	// Serial.print("\ntemp:\t");
	// Serial.println(temp);
	// Serial.print("hum:\t");
	// Serial.println(hum);
	// Serial.print("TVOC:\t");
	// Serial.println(tvoc);
	// Serial.print("NOx:\t");
	// Serial.println(srawNox);
	// Serial.print("pm2_5:\t");
	// Serial.println(pm2_5);
	// Serial.print("CO:\t");
	// Serial.println(co_ppm);
	// Serial.print("\n=================================\n");

	// delay(10000);
}