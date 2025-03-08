
#include <DS3231.h>
#include <Wire.h>

#include "pins.h"
#include "myWiFi.h"

#pragma once

class RTC {
public:
    void init();
    void sync();
    void disable();
    int getM();
    int getH();
    bool check();
    void clearAlarm();
private:

    byte alarmDay = 0;
    byte alarmHour = 0;
    byte alarmMinute = 0;
    byte alarmSecond = 0;
    byte alarmBits = 0b01110000; // Alarm 2 every minute at 00
    bool alarmDayIsDay = false;
    bool alarmH12 = false;
    bool alarmPM = false;

    bool h12 = false;
    bool hPM = false;

    const char* TAG = "RTC";
};

extern RTC rtc;