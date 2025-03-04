
#include "myWiFi.h"

bool MyWiFi::checkCfg() {
	uint32_t x = 0;
	uint32_t *p = (uint32_t *)cfgbuf.mac;
	for (uint32_t i = 0; i < sizeof(cfgbuf)/4; i++) x += p[i];
	ESP_LOGI(TAG, "RTC mem read: chk=%x x=%x ip=%08x mode=%d %s\n", cfgbuf.chk, x, cfgbuf.ip, cfgbuf.mode,x==0?"OK":"FAIL");
	if (x == 0 && cfgbuf.ip != 0) return true;
	p = (uint32_t *)cfgbuf.mac;
	for (uint32_t i = 0; i < sizeof(cfgbuf)/4; i++) Serial.printf(" %08x", p[i]);
	Serial.printf("\n");
	// bad checksum, init data
	for (uint32_t i = 0; i < 6; i++) cfgbuf.mac[i] = 0xff;
	cfgbuf.mode = 0; // chk err, reconfig
	cfgbuf.chl = 0;
	cfgbuf.ip = IPAddress(0, 0, 0, 0);
	cfgbuf.gw = IPAddress(0, 0, 0, 0);
	cfgbuf.msk = IPAddress(255, 255, 255, 0);
	cfgbuf.dns = IPAddress(0, 0, 0, 0);
	cfgbuf.seq = 100;
	return false;
}

void MyWiFi::writecfg(void) {
	// save new info
	uint8_t *bssid = WiFi.BSSID();
	for (int i = 0; i < sizeof(cfgbuf.mac); i++) cfgbuf.mac[i] = bssid[i];
	cfgbuf.chl = WiFi.channel();
	cfgbuf.ip = WiFi.localIP();
	cfgbuf.gw = WiFi.gatewayIP();
	cfgbuf.msk = WiFi.subnetMask();
	cfgbuf.dns = WiFi.dnsIP();
	// recalculate checksum
	uint32_t x = 0;
	uint32_t *p = (uint32_t *)cfgbuf.mac;
	for (uint32_t i = 0; i < sizeof(cfgbuf)/4-1; i++) x += p[i];
	cfgbuf.chk = -x;
	ESP_LOGI(TAG, "RTC mem write: chk=%x x=%x ip=%08x mode=%d\n", cfgbuf.chk, x, cfgbuf.ip, cfgbuf.mode);
}

/**
 * @brief initiates wifi
 * @return true if connected successfully
 */
bool MyWiFi::init() {
	// Read config from NVRAM
#ifdef FORCE_MODE
	if (checkCfg()) cfgbuf.mode = FORCE_MODE; // can only force if we got saved info
#else
	checkCfg();
#endif

	// Make sure Wifi settings in flash are off so it doesn't start automatically at next boot
	if (WiFi.getMode() != WIFI_OFF) {
		ESP_LOGW(TAG, "Wifi wasn't off!\n");
		WiFi.persistent(true);
		WiFi.mode(WIFI_OFF);
	}

	// Init Wifi in STA mode and connect
	WiFi.persistent(false);
	WiFi.mode(WIFI_STA);
	int m = cfgbuf.mode;
	bool ok;

	ssid = _setup.getParam("inputWifiSSID");
	pwd = _setup.getParam("inputWifiPWD");

	ESP_LOGI(TAG, "Trying to connect to: %s", ssid.c_str());

	WiFi.setHostname(getID());
	
	switch (cfgbuf.mode) {
		case 0:
		ok = WiFi.begin(ssid, pwd);
		break;
	case 1:
		ok = WiFi.begin(ssid, pwd, cfgbuf.chl, cfgbuf.mac);
		break;
	case 2:
		ok = WiFi.config(cfgbuf.ip, cfgbuf.gw, cfgbuf.msk, cfgbuf.dns);
		if (!ok) ESP_LOGW(TAG, "*** Wifi.config failed, mode=%d\n", m);
		ok = WiFi.begin(ssid, pwd);
		break;
	default:
		ok = WiFi.config(cfgbuf.ip, cfgbuf.gw, cfgbuf.msk, cfgbuf.dns);
		if (!ok) ESP_LOGW(TAG, "*** Wifi.config failed, mode=%d\n", m);
		ok = WiFi.begin(ssid, pwd, cfgbuf.chl, cfgbuf.mac);
		cfgbuf.mode = -1;
		break;
	}
	if (!ok) {
		ESP_LOGE(TAG, "*** Wifi.begin failed, mode=%d\n", m);
	}

	unsigned long lastms = millis();
	while (1) {
		if (WiFi.status() == WL_CONNECTED) {
			uint32_t wifiConnMs = millis();
			ESP_LOGI(TAG, "Wifi successfully connected in %dms using hostname: %s", wifiConnMs, WiFi.getHostname());
			writecfg();
			break;
		}
		else {
			if (millis()-lastms >= WIFI_TIMEOUT) {
				ESP_LOGE(TAG, "Timeout: couldn't connect to Wifi after %dms", WIFI_TIMEOUT);
				return false;
			}
			delay(1);
		}
	}

	if(MDNS.begin(getID())) {
		ESP_LOGI(TAG, "Successfully started mDNS");
	}
	
	return true;
}

void MyWiFi::disable() {
	WiFi.mode(WIFI_OFF);
}

time_t MyWiFi::getTime() {
	time_t now;
	struct tm timeinfo;

	if (!esp_sntp_enabled()) {
		esp_sntp_setoperatingmode(ESP_SNTP_OPMODE_POLL);
		sntp_set_sync_mode(SNTP_SYNC_MODE_IMMED);
		esp_sntp_setservername(0, NTP_SERVER_0);
		esp_sntp_setservername(1, NTP_SERVER_1);
		esp_sntp_setservername(2, NTP_SERVER_2);
		esp_sntp_init();
	}

	setenv("TZ", _setup.getParam("inputTZ").c_str(), 1);
	tzset();

	unsigned long lastms = millis();
	while (sntp_get_sync_status() != SNTP_SYNC_STATUS_COMPLETED) {
		if (millis()-lastms >= SNTP_TIMEOUT) {
			ESP_LOGE(TAG, "TIMEOUT: could not sync time...");
			break;
		}
	}

	if(!getLocalTime(&timeinfo)) {
		ESP_LOGW(TAG, "Failed to obtain time");
		return(0);
	}

	time(&now);
	return now;
}

const char * MyWiFi::getID() {
	unsigned char mac_base[6] = {0};

	if (esp_efuse_mac_get_default(mac_base) == ESP_OK) {
		char buffer[17];  // 10 characters for title + 3*2 characters for hex + 1 character for null terminator
		sprintf(buffer, "LifeWatch-%02X%02X%02X", mac_base[3], mac_base[4], mac_base[5]);
		id = buffer;
	}

	return id.c_str();
}

MyWiFi _wifi;