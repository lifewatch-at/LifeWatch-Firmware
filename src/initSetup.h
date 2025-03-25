
#pragma once

#include <Arduino.h>
#include "myWifi.h"
#include <AsyncTCP.h>
#include <LittleFS.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

#define SETUP_PWD "LifeWatch"

#define PARAM_NAME		"inputName"
#define PARAM_WIFI_SSID	"inputWifiSSID"
#define PARAM_WIFI_PWD	"inputWifiPWD"
#define PARAM_MQTT_USER	"inputMQTTuser"
#define PARAM_MQTT_PWD	"inputMQTTpwd"
#define PARAM_TZ_OFFSET	"inputTZ"       // https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv

class Setup {
public:
    void init();
    String getParam(String param);
    void print();
    bool isRunning() {return _isRunning;}
    bool isDone();
private:
    const char* password = SETUP_PWD;
    IPAddress apip = IPAddress(10,0,0,1);
    bool _isRunning = false;
};

extern Setup _setup;
