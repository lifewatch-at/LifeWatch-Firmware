
#pragma once

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <StreamUtils.h>
#include <ESPmDNS.h>

#include "myWifi.h"
#include "json.h"
#include "initSetup.h"

#define MDNS_TIMEOUT 5000
#define MQTT_TIMEOUT 5000
#define SERVER_TIMEOUT 1000

#define MQTT_HOST	"LifeWatch-Server"
#define MQTT_PORT	1883
#define PUB_ROOT	"LifeWatch/pub/"
#define SUB_ROOT	"LifeWatch/sub/"

class MQTT {
public:
	bool connect();
	void send(Device device);
	void loop();
	
	const char* TAG = "MQTT";
	JsonDocument response;
	
private:
	void publish(const char *topic, JsonDocument doc);

	IPAddress srvip;
	uint16_t  srvport = MQTT_PORT;
	const char *client_id = _wifi.getID();
	String mqtt_user;
	String mqtt_pwd;
	char subTopic[32] = SUB_ROOT;	// 15 characters for root + 17 for id 
	char pubTopic[32] = PUB_ROOT;	// 15 characters for root + 17 for id
};

extern MQTT mqtt;