// https://arduinojson.org/v6/how-to/use-arduinojson-with-pubsubclient/

#include "mqtt.h"

WiFiClient esp_client;
PubSubClient mqtt_client(esp_client);

volatile bool gotResponse = false;


void callback(char* topic, byte* payload, unsigned int length) {
	ESP_LOGI(_mqtt.TAG, "received message from topic: %s", topic);
	deserializeJson(_mqtt.response, payload, length);
	gotResponse = true;

	// TODO: parse the received json document
}

void MQTT::connect() {
	unsigned long lastms = millis();

	while (mdns_init()!=ESP_OK) {
		if (millis()-lastms >= 1000) {
			ESP_LOGE(TAG, "Timeout: failed starting mDNS client");
			break;
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

	mqtt_user = _setup.getParam("inputMQTTuser");
	mqtt_pwd = _setup.getParam("inputMQTTpwd");

	lastms = millis();

	while (!mqtt_client.connected()) {
		ESP_LOGI(TAG, "Connecting to MQTT Broker as %s...\n", client_id);
		mqtt_client.setServer(srvip, MQTT_PORT);
		mqtt_client.setCallback(callback);
		if (mqtt_client.connect(client_id, mqtt_user.c_str(), mqtt_pwd.c_str())) {
			ESP_LOGI(TAG, "Connected to MQTT broker\n");
		} else {
			ESP_LOGE(TAG, "Failed to connect to MQTT broker, rc=%d\n", mqtt_client.state());
		}

		if (millis()-lastms >= MQTT_TIMEOUT) {
			ESP_LOGE(TAG, "Timeout: MQTT");
			break;
		}
	}

	mqtt_client.subscribe(strcat(SUB_ROOT, client_id));
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

void MQTT::send(Device device) {
	if (mqtt_client.connected()) {
		publish(strcat(PUB_ROOT, client_id), device.toJSON());
	}
	else {
		ESP_LOGW(TAG, "MQTT not connected.\n");
	}
}

void MQTT::loop() {
	ESP_LOGD(TAG, "Waiting for LifeWatch server response...");
	unsigned long lastms = millis();
	while(!gotResponse) {
		mqtt_client.loop();

		if (millis()-lastms >= MQTT_TIMEOUT) {
			ESP_LOGW(TAG, "No response from LifeWatch server after %dms", MQTT_TIMEOUT);
			break;
		}
	}
}

MQTT _mqtt;