
#pragma once

#include <WiFi.h>
#include <time.h>

#include "credentials.h"
#include "config.h"

#define WIFI_TIMEOUT 5000

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
private:
    const char* TAG = "WIFI";

    const char *ssid = WIFI_SSID;
    const char *pass = WIFI_PWD;

    const char* ntpServer = NTP_SERVER;
    const long  gmtOffset_sec = GMT_OFFSET;
    const int   daylightOffset_sec = DAYLIGHT_OFFSET;
};