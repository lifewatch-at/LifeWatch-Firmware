
#include "mqtt.h"

WiFiClient esp_client;
PubSubClient mqtt_client(esp_client);

void MQTT::mqttConnect() {
	unsigned long lastms = millis();

	ESP_LOGI(TAG, "Starting mDNS");
	while (mdns_init()!=ESP_OK) {
		if (millis()-lastms >= 1000) {
			ESP_LOGE(TAG, "Timeout: failed starting mDNS client");
		}
	}
	
	ESP_LOGI(TAG, "Resolving hostname: %s", MQTT_HOST);
	lastms = millis();
	while (1) {
		MDNS.queryHost(MQTT_HOST);
		if (srvip != IPAddress(0,0,0,0)) {
			ESP_LOGI(TAG, "Got IP: %s", srvip.toString());
			break;
		}

		if (millis()-lastms >= MDNS_TIMEOUT) {
			ESP_LOGE(TAG, "Timeout: failed resolving hostname");
			return;		// no need for trying to connect to nothing
		}
	}

	mqtt_user = _setup.getParam("inputMQTTuser").c_str();
	mqtt_pwd = _setup.getParam("inputMQTTpwd").c_str();

	lastms = millis();

	while (!mqtt_client.connected()) {
		ESP_LOGI(TAG, "Connecting to MQTT Broker as %s...\n", client_id);
		mqtt_client.setServer(srvip, MQTT_PORT);
		if (mqtt_client.connect(client_id, mqtt_user, mqtt_pwd)) {
			ESP_LOGI(TAG, "Connected to MQTT broker\n");
		} else {
			ESP_LOGE(TAG, "Failed to connect to MQTT broker, rc=%d\n", mqtt_client.state());
		}

		if (millis()-lastms >= MQTT_TIMEOUT) {
			ESP_LOGE(TAG, "Timeout: MQTT");
			break;
		}
	}
}


void MQTT::publish(const char *pub_topic, JsonDocument doc) {
	mqtt_client.beginPublish(pub_topic, measureJson(doc), true);
	BufferingPrint bufferedClient(mqtt_client, 32);
	serializeJson(doc, bufferedClient);
	bufferedClient.flush();
	if (mqtt_client.endPublish()) {
		ESP_LOGI(TAG, "Packet sent successfully.\n");
	}
	else {
		ESP_LOGE(TAG, "Error while sending.\n");
	}
}

void MQTT::mqttSend(Device device) {
	if (mqtt_client.connected()) {
		publish(pub_topic, device.toJSON());
	}
	else {
		ESP_LOGW(TAG, "MQTT not connected.\n");
	}
}

void MQTT::callback(char* topic, byte* payload, unsigned int length) {
	JsonDocument doc;
	deserializeJson(doc, payload, length);
	strlcpy(name, doc["name"] | "default", sizeof(name));
}
