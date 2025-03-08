
#include "rtc.h"

DS3231 myRTC(Wire);

void RTC::init() {
    ESP_LOGI(TAG, "Initialising RTC...");
    // set Utillities
    myRTC.enable32kHz(false);
    myRTC.enableOscillator(false, true, 0);

    // sync RTC with NTP
    sync();

    // make sure that Alarm 1 is off
    myRTC.turnOffAlarm(1);
    myRTC.setA1Time(1,1,0xFF,1,0b01100000, false, false, false);        // write nonsense to ensure that Alarm 1 wont block INT pin
    if (myRTC.checkIfAlarm(1)) {
        ESP_LOGW(TAG, "Failed to disable Alarm 1");
    }

    // now set Alarm 2
    myRTC.turnOffAlarm(2);
    myRTC.setA2Time(alarmDay, alarmHour, alarmMinute, alarmBits, alarmDayIsDay, alarmH12, alarmPM);
    myRTC.turnOnAlarm(2);
    if (!myRTC.checkIfAlarm(2)) {
        ESP_LOGW(TAG, "Failed to enable Alarm 2");
    }
}

void RTC::sync() {
    ESP_LOGI(TAG, "Syncing RTC with NTP");
    myRTC.setEpoch(_wifi.getTime());
}

void RTC::disable() {
    myRTC.turnOffAlarm(1);
    if (myRTC.checkIfAlarm(1)) {
        ESP_LOGW(TAG, "Failed to disable Alarm 1");
    }

    myRTC.turnOffAlarm(2);
    if (myRTC.checkIfAlarm(2)) {
        ESP_LOGW(TAG, "Failed to disable Alarm 2");
    }
}

int RTC::getM() {
    return myRTC.getMinute();
}

int RTC::getH() {
    return myRTC.getHour(h12, hPM);
}

bool RTC::check() {
    bool x = myRTC.oscillatorCheck();
    ESP_LOGI(TAG, "RTC Oscillator: %s", x==1 ? "OK":"FAIL");
    return x;
}

void RTC::clearAlarm() {
    myRTC.checkIfAlarm(2);
}

RTC rtc;