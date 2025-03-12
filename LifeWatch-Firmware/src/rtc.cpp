
#include "rtc.h"

void RTC::init() {
    disable32K();
    writeSqwPinMode(DS3231_OFF);
    
    sync();

    clearAlarm(1);
    clearAlarm(2);

    disableAlarm(1);
    
    if (!setAlarm2(DateTime(), DS3231_A2_PerMinute)) {
        ESP_LOGE(TAG, "couldn't set Alarm");
    }
}

void RTC::sync() {
    ESP_LOGI(TAG, "Syncing RTC with sntp...");
    adjust(DateTime(_wifi.getTime()));
}

/*!
    @return True if rtc power was lost or false if it is running
*/
bool RTC::check() {
    bool x = lostPower();
    ESP_LOGI(TAG, "RTC oscillator: %s", x==0 ? "OK":"FAIL");
    return x;
}

tm* RTC::local() {
    time_t t = now().unixtime();
    return localtime(&t);
}

uint8_t RTC::getM() {
    return local()->tm_min;
}

uint8_t RTC::getH() {
    return local()->tm_hour;
}

RTC rtc;