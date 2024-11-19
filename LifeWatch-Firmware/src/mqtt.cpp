
#include "mqtt.h"

WiFiClient esp_client;
PubSubClient mqtt_client(esp_client);


void MQTT::mqttConnect() {
	while (!mqtt_client.connected()) {
		ESP_LOGI(TAG, "Connecting to MQTT Broker as %s...\n", client_id.c_str());
		mqtt_client.setServer(MQTT_IPAD, MQTT_PORT);
		if (mqtt_client.connect(client_id.c_str(), MQTT_USR, MQTT_PWD)) {
			ESP_LOGI(TAG, "Connected to MQTT broker\n");
		} else {
			ESP_LOGE(TAG, "Failed to connect to MQTT broker, rc=%d\n", mqtt_client.state());
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
