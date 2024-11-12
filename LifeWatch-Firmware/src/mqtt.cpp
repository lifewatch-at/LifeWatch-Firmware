
#include "mqtt.h"

WiFiClient esp_client;
PubSubClient mqtt_client(esp_client);


void MQTT::mqttConnect() {
    while (!mqtt_client.connected()) {
        Serial.printf("Connecting to MQTT Broker as %s...\n", client_id.c_str());
        mqtt_client.setServer(MQTT_IPAD, MQTT_PORT);
        if (mqtt_client.connect(client_id.c_str(), MQTT_USR, MQTT_PWD)) {
            Serial.println("Connected to MQTT broker");
        } else {
            Serial.print("Failed to connect to MQTT broker, rc=");
            Serial.println(mqtt_client.state());
        }
    }
}


void MQTT::publish(const char *pub_topic, JsonDocument doc) {
    mqtt_client.beginPublish(pub_topic, measureJson(doc), true);
    //BufferingPrint bufferedClient(mqtt_client, 32);
    //serializeJson(doc, bufferedClient);
    //bufferedClient.flush();
    if (mqtt_client.endPublish()) {
        Serial.println("Packet sent successfully.");
    }
    else {
        Serial.println("Error while sending.");
    }
}

void MQTT::mqttSend(Device device) {
    if (mqtt_client.connected()) {
        publish(pub_topic, device.toJSON());
    }
    else {
        Serial.println("MQTT not connected.");
    }
}

void MQTT::callback(char* topic, byte* payload, unsigned int length) {
  JsonDocument doc;
  deserializeJson(doc, payload, length);
  strlcpy(name, doc["name"] | "default", sizeof(name));
}
