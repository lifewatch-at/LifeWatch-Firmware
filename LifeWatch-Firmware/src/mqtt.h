
#pragma once

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <StreamUtils.h>

#include "json.h"
#include "credentials.h"

class MQTT {
public:
    void mqttConnect();
    void publish(const char *topic, JsonDocument doc);
    void mqttSend(Device device);
    void callback(char* topic, byte* payload, unsigned int length);

private:
    IPAddress srvip = IPAddress(*MQTT_IPAD);
    uint16_t  srvport = MQTT_PORT;
    String mac = WiFi.macAddress();
    String client_id = "LifeWatch-" + String(mac[9]) + String(mac[10]) + String(mac[12]) + String(mac[13]) + String(mac[15]) + String(mac[16]);
    const char *pub_topic = PUB_TOPIC;

    char name[32] = "";

};