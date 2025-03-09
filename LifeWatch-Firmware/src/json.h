
#pragma once

#include <Arduino.h>
#include <vector>

#include <ArduinoJson.h>


class Telemetry {
public:
    Telemetry(String id, String mac, time_t timestamp, int bat) : id(id), mac(mac), bat(bat), timestamp(timestamp) {};

    JsonDocument toJSON();
private:
    String id;
    String mac;
    time_t timestamp;
    int bat;
};

class Sensor {
public:
    Sensor(String name, float value, String unit) : name(name), value(value), unit(unit) {};

    JsonDocument toJSON();
private:
    String name;
    float value;
    String unit;
};

class Device {
public:
    JsonDocument toJSON();
    void add(Sensor* sensor);

    Telemetry* telemetry;
    std::vector<Sensor*> _sensors;
};

