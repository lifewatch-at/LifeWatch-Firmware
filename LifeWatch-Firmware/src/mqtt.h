
#pragma once

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <StreamUtils.h>
#include <ESPmDNS.h>

#include "myWifi.h"
#include "json.h"
#include "initSetup.h"

#define MDNS_TIMEOUT 1000
#define MQTT_TIMEOUT 5000

#define MQTT_HOST	"LifeWatch-Server"
#define MQTT_PORT	1883
#define PUB_TOPIC	"LifeWatch"
#define SUB_TOPIC	""

class MQTT {
public:
	void mqttConnect();
	void publish(const char *topic, JsonDocument doc);
	void mqttSend(Device device);
	void callback(char* topic, byte* payload, unsigned int length);

private:
	IPAddress srvip;
	uint16_t  srvport = MQTT_PORT;
	String mac = WiFi.macAddress();
	const char *client_id = _wifi.getID();
	const char *pub_topic = PUB_TOPIC;

	const char* mqtt_user;
	const char* mqtt_pwd;

	char name[32] = "";
	const char* TAG = "MQTT";
};