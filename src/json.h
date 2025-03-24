
#pragma once

#include <Arduino.h>
#include <vector>

#include <ArduinoJson.h>


class Telemetry {
public:
    Telemetry(String id, String name, time_t timestamp, int bat) : id(id), name(name), timestamp(timestamp), bat(bat) {};

    JsonDocument toJSON();
private:
    String id;
    String name;
    time_t timestamp;
    int bat;
};

class Sensor {
public:
    Sensor() {};
    Sensor(String name, float value, String unit) : name(name), value(value), unit(unit) {};
    void fill(String name, float value, String unit) {
        this->name = name;
        this->value = value;
        this->unit = unit; 
    }
    bool hasName(const char* _name) { if (name==_name) {return true;} return false;}
    float getValue() {return value;}
    JsonDocument toJSON();
private:
    String name;
    float value;
    String unit;
};

class Device {
public:
    JsonDocument toJSON();
    void add(Sensor sensor);
    float getValueof(const char* sensor);

    Telemetry* telemetry;
    std::vector<Sensor> _sensors;
};
