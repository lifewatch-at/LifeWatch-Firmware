/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Based on https://RandomNerdTutorials.com/esp32-esp8266-input-data-html-form/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/
#include "initSetup.h"

AsyncWebServer server(80);

const char* PARAMS[] {
	PARAM_WIFI_SSID,
	PARAM_WIFI_PWD,
	PARAM_MQTT_USER,
	PARAM_MQTT_PWD,
	PARAM_TZ_OFFSET,
	PARAM_NAME
};

const char* TAGS = "initSetup";
const int nrofParams = sizeof(PARAMS) / sizeof(PARAMS[0]);

bool fsinit() {
	if (!LittleFS.begin(true)){
		ESP_LOGE(TAGS, "An Error has occurred while mounting LittleFS");
		return false;
	}
	return true;
}

void notFound(AsyncWebServerRequest *request) {
  	request->send(404, "text/plain", "Not found");
}

String readFile(fs::FS &fs, const char * path){
	if (!fsinit()) {
		return String();
	}

	ESP_LOGI(TAGS, "Reading file: %s\r", path);
	File file = fs.open(path, "r");
	if(!file || file.isDirectory()){
		ESP_LOGW(TAGS, "- empty file or failed to open file");
		return String();
	}
	ESP_LOGV(TAGS, "- read from file:");
	String fileContent;
	while(file.available()){
		fileContent+=String((char)file.read());
	}
	file.close();
	ESP_LOGV(TAGS, "%s\n", fileContent);
	return fileContent;
}

void writeFile(fs::FS &fs, const char * path, const char * message){
	ESP_LOGI(TAGS, "Writing file: %s\r", path);
	File file = fs.open(path, "w");
	if(!file){
		ESP_LOGE(TAGS, "- failed to open file for writing");
		return;
	}
	if(file.print(message)){
		ESP_LOGI(TAGS, "- file written");
	} else {
		ESP_LOGE(TAGS, "- write failed");
	}
	file.close();
}

// Replaces placeholder with stored values
String processor(const String& var){
	JsonDocument doc;
	deserializeJson(doc, readFile(LittleFS, "/credentials.json")); 

	for (int i=0;i<nrofParams;i++) {
		if (var == PARAMS[i]) {
			return doc[PARAMS[i]];
		}
	}

	return String();
}

void Setup::init() {
	volatile bool done = false;
	
	// Initialize LittleFS
	if (!fsinit()) {
		ESP_LOGE(TAGS, "ERROR: setup break");
		return;
	}
	
	// Start WiFi AP
	const char* id = _wifi.getID();
	ESP_LOGI(TAGS, "Starting AP using SSID: %s", id);
	_wifi.mode(WIFI_OFF);
	_wifi.mode(WIFI_AP);
	_wifi.softAP(id, password);
	delay(100);
	_wifi.softAPConfig(apip, apip, IPAddress(255,255,255,0));
	
	
	// Send web page with input fields to client
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send(200, "text/html", readFile(LittleFS, "/setup.html"), processor);
	});
	
	// parse values of input fields and save them to a JSON
	server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
		String inputMessage;
		JsonDocument doc;
		deserializeJson(doc, readFile(LittleFS, "/credentials.json"));

		for (int i=0;i<nrofParams;i++) {
			if (request->hasParam(PARAMS[i])) {
				inputMessage = request->getParam(PARAMS[i])->value();
				if (inputMessage != "") {
					doc[PARAMS[i]] = inputMessage;
				}
				else {
					ESP_LOGV(TAGS, "empty line. skipping...");
				}
			}
		}
		
		char buffer[200];
		serializeJson(doc,buffer);
		ESP_LOGI(TAGS, "JSON: %s", buffer);
		writeFile(LittleFS, "/credentials.json", buffer);

		ESP_LOGV(TAGS, "%s\n", inputMessage);
		request->send(200, "text/text", inputMessage);
	});

	// finish initial setup routine on pressing "Done" button
	server.on("/done", HTTP_GET, [&done] (AsyncWebServerRequest *request) {
		ESP_LOGI(TAGS, "Received setup done...");
		done = true;
	});

	server.on("/reset", HTTP_GET, [] (AsyncWebServerRequest *request) {
		ESP_LOGI(TAGS, "Received reset input...");
	});

	server.onNotFound(notFound);
	server.begin();
	ESP_LOGI(TAGS, "Webserver started on %s", _wifi.softAPIP().toString());

	unsigned long lastms = millis();
	while (1) {
		if (millis()-lastms >= 10000) {
			ESP_LOGV(TAGS, "setup loop");
			lastms = millis();
		}
		if (done) {
			server.end();
			_wifi.mode(WIFI_OFF);
			break;
		}
	}
}

String Setup::getParam(String param) {
	JsonDocument doc;
	ESP_LOGV(TAGS, "reading param: %s", param);
	deserializeJson(doc, readFile(LittleFS, "/credentials.json"));
	return doc[param];
}

void Setup::print() {
	ESP_LOGI(TAGS, "credentials: %s", readFile(LittleFS, "/credentials.json").c_str());
}

Setup _setup;