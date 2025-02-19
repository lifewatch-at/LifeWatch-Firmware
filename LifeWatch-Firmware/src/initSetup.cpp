/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-esp8266-input-data-html-form/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/
#include "initSetup.h"

AsyncWebServer server(80);

const char* TAG = "initSetup";

const char* PARAM_WIFI_SSID = "inputWifiSSID";
const char* PARAM_WIFI_PWD = "inputWifiPWD";

const char* PARAM_MQTT_USER = "inputMQTTuser";
const char* PARAM_MQTT_PWD = "inputMQTTpwd";

const char* PARAM_TZ_OFFSET = "inputTZ";

// TODO: add mdns hostname resolution for MQTT server ip

void notFound(AsyncWebServerRequest *request) {
  	request->send(404, "text/plain", "Not found");
}

String readFile(fs::FS &fs, const char * path){
	ESP_LOGV(TAG, "Reading file: %s\r", path);
	File file = fs.open(path, "r");
	if(!file || file.isDirectory()){
		ESP_LOGW(TAG, "- empty file or failed to open file");
		return String();
	}
	ESP_LOGV(TAG, "- read from file:");
	String fileContent;
	while(file.available()){
		fileContent+=String((char)file.read());
	}
	file.close();
	ESP_LOGV(TAG, "%s\n", fileContent);
	return fileContent;
}

void writeFile(fs::FS &fs, const char * path, const char * message){
	ESP_LOGI(TAG, "Writing file: %s\r", path);
	File file = fs.open(path, "w");
	if(!file){
		ESP_LOGW(TAG, "- failed to open file for writing");
		return;
	}
	if(file.print(message)){
		ESP_LOGI(TAG, "- file written");
	} else {
		ESP_LOGE(TAG, "- write failed");
	}
	file.close();
}

// Replaces placeholder with stored values
String processor(const String& var){
	if(var == "inputWifiSSID"){
		return readFile(LittleFS, "/inputWifiSSID.txt");
	}
	if(var == "inputMQTTuser"){
		return readFile(LittleFS, "/inputMQTTuser.txt");
	}
	if(var == "inputTZ"){
		return readFile(LittleFS, "/inputTZ.txt");
	}	
	return String();
}

void Setup::initSetup() {
	// Initialize LittleFS
	if(!LittleFS.begin(true)){
		ESP_LOGE(TAG, "An Error has occurred while mounting LittleFS");
		return;
	}

	// Start WiFi AP
  	WiFi.mode(WIFI_AP);
  	WiFi.softAP(_wifi.getID(), password);
	WiFi.begin();

	
  	// Send web page with input fields to client
  	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send(200, "text/html", index_html, processor);
  	});

  	server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
		String inputMessage;

		// WiFi creds
		if (request->hasParam(PARAM_WIFI_SSID)) {
			inputMessage = request->getParam(PARAM_WIFI_SSID)->value();
			writeFile(LittleFS, "/inputWifiSSID.txt", inputMessage.c_str());
		}
		if (request->hasParam(PARAM_WIFI_PWD)) {
			inputMessage = request->getParam(PARAM_WIFI_PWD)->value();
			writeFile(LittleFS, "/inputWifiPWD.txt", inputMessage.c_str());
		}

		// MQTT creds
		if (request->hasParam(PARAM_MQTT_USER)) {
			inputMessage = request->getParam(PARAM_MQTT_USER)->value();
			writeFile(LittleFS, "/inputMQTTuser.txt", inputMessage.c_str());
		}
		if (request->hasParam(PARAM_MQTT_PWD)) {
			inputMessage = request->getParam(PARAM_MQTT_PWD)->value();
			writeFile(LittleFS, "/inputMQTTpwd.txt", inputMessage.c_str());
		}

		// TZ
		if (request->hasParam(PARAM_TZ_OFFSET)) {
			inputMessage = request->getParam(PARAM_TZ_OFFSET)->value();
			writeFile(LittleFS, "/inputTZ.txt", inputMessage.c_str());
		}

		else {
			inputMessage = "No message sent";
		}

		ESP_LOGV(TAG, "%s\n", inputMessage);
		request->send(200, "text/text", inputMessage);
	});

	server.on("/done", HTTP_GET, [] (AsyncWebServerRequest *request) {
		ESP_LOGI(TAG, "Received setup done...");
		delay(100);
		server.end();
		WiFi.mode(WIFI_OFF);
	});

	server.onNotFound(notFound);
	server.begin();
}

String Setup::getWiFiSSID() {
	return readFile(LittleFS, "/inputWifiSSID.txt");
}

String Setup::getWiFiPWD() {
	return readFile(LittleFS, "/inputWifiPWD.txt");
}

void Setup::printSetup() {
	ESP_LOGI(TAG, "WiFi SSID: %s", readFile(LittleFS, "/inputWifiSSID.txt"));
	ESP_LOGI(TAG, "WiFi Password: %s", readFile(LittleFS, "/inputWifiPWD.txt"));
	ESP_LOGI(TAG, "MQTT User: %s", readFile(LittleFS, "/inputMQTTuser.txt"));
	ESP_LOGI(TAG, "MQTT Password: %s", readFile(LittleFS, "/inputMQTTpwd.txt"));
	ESP_LOGI(TAG, "Timezone: %d", readFile(LittleFS, "/inputTZ.txt").toInt());
}

Setup _setup;