
#pragma once

#include <WiFi.h>
#include <time.h>
#include "esp_sntp.h"

#include "initSetup.h"

#define WIFI_TIMEOUT 5000
#define SNTP_TIMEOUT 5000

#define NTP_SERVER_0	"pool.ntp.org"
#define NTP_SERVER_1	"de.pool.ntp.org"
#define NTP_SERVER_2	"time.nist.gov"

static RTC_DATA_ATTR struct {
	byte mac[6];
	byte mode;
	byte chl;
	uint32_t ip;
	uint32_t gw;
	uint32_t msk;
	uint32_t dns;
	uint16_t seq;
	uint32_t chk;
} cfgbuf;

class MyWiFi {
public:
	bool checkCfg();
	void writecfg(void);
	void disable();
	void init();
	time_t getTime();
	const char * getID();
	
	private:
	const char* TAG = "WIFI";
	
	const char *ssid = "";
	const char *pwd = "";

	String id = "";
};

extern MyWiFi _wifi;