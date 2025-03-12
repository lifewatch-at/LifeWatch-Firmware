#include <Arduino.h>
// #include <esp_log.h>

// #include "myWifi.h"
// #include "initSetup.h"
// #include "rtc.h"
// #include "myDisplay.h"
#include "module.h"

// const char* TAG = "main";

// RTC_DATA_ATTR uint32_t wake_cnt = 0;

// myDisplay mdisplay;

void setup() {
	// Serial.setDebugOutput(true);
	Serial.begin(115200);
	delay(2000);
	// ESP_LOGI(TAG, "setup started.");
	// ESP_LOGI(TAG, "ID: %s", _wifi.getID());

	// /* connect to wifi or start initial config */ 
	// while (1) {
	// 	if (_wifi.init()) {
	// 		break;
	// 	}
	// 	else {
	// 		_setup.init();
	// 	}
	// }

	// Wire.begin(MB_SDA, MB_SCL, 400000);

	// /* check rtc and sync if needed */
	// if (wake_cnt == 0) {
	// 	rtc.init();
	// }
	// else {
	// 	if (!rtc.check()) {
	// 		rtc.sync();
	// 	}
	// 	rtc.clearAlarm();
	// }

	// ESP_LOGI(TAG, "time: %2d:%2d", rtc.getH(), rtc.getM());

	// /* enable PCINT and go to deep sleep */
	// ESP_LOGI(TAG, "setup done. going to sleep...\n");
	// wake_cnt++;
	// pinMode(RTC_INT, INPUT);
	// esp_sleep_enable_ext0_wakeup(GPIO_NUM_7, LOW);
	// esp_deep_sleep_start();
	// ESP_LOGE(TAG, "This shouldn't get printed");

	//mdisplay.init();
	Serial.println("\ncall init");
	module_init();
}

void loop() {

	Serial.println("\nScanne nach I2C-Geräten...");
	int devicesFound = 0;

	for (uint8_t address = 1; address < 127; ++address) { // Alle möglichen I2C-Adressen prüfen
		Wire1.beginTransmission(address);
		uint8_t error = Wire1.endTransmission();

		if (error == 0) { // Gerät gefunden
		Serial.print("I2C-Gerät gefunden bei Adresse 0x");
		if (address < 16) Serial.print("0");
		Serial.println(address, HEX);
		++devicesFound;
		}
	}

	if (devicesFound == 0) {
		Serial.println("Keine I2C-Geräte gefunden.\n");
	} else {
		Serial.println("Scan abgeschlossen.\n");
	}

	module_check();

	Serial.print("temp = ");
	Serial.println(temp);
	Serial.print("hum = ");
	Serial.println(hum);
	Serial.print("pm2_5 = ");
	Serial.println(pm2_5);
	Serial.print("tvoc = ");
	Serial.println(tvoc);
	Serial.print("Nox = ");
	Serial.println(srawNox);
	Serial.print("\n================================\n");

	delay(3000);
}