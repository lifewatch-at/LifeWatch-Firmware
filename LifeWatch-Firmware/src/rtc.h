
#pragma once

#include <Arduino.h>
#include <RTClib.h>
#include <time.h>

#include "myWifi.h"

class RTC : public RTC_DS3231 {
public:
    void init();
    void sync();
    bool check();
    tm* local();
    uint8_t getM();
    uint8_t getH();
private:
    const char* TAG = "RTC";
};

extern RTC rtc;