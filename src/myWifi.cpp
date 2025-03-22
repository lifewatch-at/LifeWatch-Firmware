
// https://blog.voneicken.com/2018/lp-wifi-esp32-2/

#include "myWifi.h"

bool MyWiFi::checkCfg() {
	uint32_t x = 0;
	uint32_t *p = (uint32_t *)cfgbuf.mac;
	for (uint32_t i = 0; i < sizeof(cfgbuf)/4; i++) x += p[i];
	ESP_LOGI(TAG, "RTC mem read: chk=%x x=%x ip=%08x mode=%d %s", cfgbuf.chk, x, cfgbuf.ip, cfgbuf.mode,x==0?"OK":"FAIL");
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
	uint8_t *bssid = BSSID();
	for (int i = 0; i < sizeof(cfgbuf.mac); i++) cfgbuf.mac[i] = bssid[i];
	cfgbuf.chl = channel();
	cfgbuf.ip = localIP();
	cfgbuf.gw = gatewayIP();
	cfgbuf.msk = subnetMask();
	cfgbuf.dns = dnsIP();
	// recalculate checksum
	uint32_t x = 0;
	uint32_t *p = (uint32_t *)cfgbuf.mac;
	for (uint32_t i = 0; i < sizeof(cfgbuf)/4-1; i++) x += p[i];
	cfgbuf.chk = -x;
	ESP_LOGI(TAG, "RTC mem write: chk=%x x=%x ip=%08x mode=%d", cfgbuf.chk, x, cfgbuf.ip, cfgbuf.mode);
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

	if (isConnected()) {
		ESP_LOGW(TAG, "WiFi already connected!");
		return true;
	}

	// Make sure Wifi settings in flash are off so it doesn't start automatically at next boot
	if (getMode() != WIFI_OFF) {
		ESP_LOGW(TAG, "Wifi wasn't off!");
		persistent(true);
		mode(WIFI_OFF);
	}

	// Init Wifi in STA mode and connect
	persistent(false);
	mode(WIFI_STA);
	int m = cfgbuf.mode;
	bool ok;

	ssid = _setup.getParam(PARAM_WIFI_SSID);
	pwd = _setup.getParam(PARAM_WIFI_PWD);

	ESP_LOGI(TAG, "Trying to connect to: %s", ssid.c_str());

	setHostname(getID());
	
	switch (cfgbuf.mode) {
		case 0:
		ok = begin(ssid, pwd);
		break;
	case 1:
		ok = begin(ssid, pwd, cfgbuf.chl, cfgbuf.mac);
		break;
	case 2:
		ok = config(cfgbuf.ip, cfgbuf.gw, cfgbuf.msk, cfgbuf.dns);
		if (!ok) ESP_LOGW(TAG, "*** Wifi.config failed, mode=%d", m);
		ok = begin(ssid, pwd);
		break;
	default:
		ok = config(cfgbuf.ip, cfgbuf.gw, cfgbuf.msk, cfgbuf.dns);
		if (!ok) ESP_LOGW(TAG, "*** Wifi.config failed, mode=%d", m);
		ok = begin(ssid, pwd, cfgbuf.chl, cfgbuf.mac);
		cfgbuf.mode = -1;
		break;
	}
	if (!ok) {
		ESP_LOGE(TAG, "*** Wifi.begin failed, mode=%d", m);
	}

	unsigned long lastms = millis();
	while (1) {
		if (status() == WL_CONNECTED) {
			uint32_t wifiConnMs = millis();
			ESP_LOGI(TAG, "Wifi successfully connected in %dms using hostname: %s", wifiConnMs, getHostname());
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

time_t MyWiFi::getTime() {
	time_t now;
	struct tm timeinfo;

	ESP_LOGI(TAG, "Retrieving time...");

	configTzTime(_setup.getParam(PARAM_TZ_OFFSET).c_str(), NTP_SERVER_0, NTP_SERVER_1, NTP_SERVER_2);

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
		sprintf(id, "LifeWatch-%02X%02X%02X", mac_base[3], mac_base[4], mac_base[5]);
	}
	return id;
}

MyWiFi _wifi;