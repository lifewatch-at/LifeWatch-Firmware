
#include "json.h"

JsonDocument Sensor::toJSON() {
    JsonDocument doc;
    doc["name"] = name;
    doc["value"] = value;
    doc["unit"] = unit;
    return doc;
}

JsonDocument Telemetry::toJSON() {
    JsonDocument doc;
    doc["id"] = id;
    doc["pretty_name"] = name;
    doc["battery"] = bat;
    return doc;
}

JsonDocument Device::toJSON() {
    JsonDocument doc;
    doc["telemetry"] = telemetry->toJSON();

    JsonArray entities = doc["entities"].to<JsonArray>();
    for (Sensor *sensor : _sensors) {
        entities.add(sensor->toJSON());
    }

    return doc;
}

void Device::add(Sensor *sensor) {
    _sensors.push_back(sensor);
}