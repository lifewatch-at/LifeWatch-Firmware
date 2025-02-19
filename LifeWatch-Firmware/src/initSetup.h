
#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include "myWifi.h"
#include <AsyncTCP.h>
#include <LittleFS.h>
#include <ESPAsyncWebServer.h>

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>LifeWatch Config</title>
    <style>
        body { font-family: Arial, sans-serif; text-align: center; }
        input { display: block; width: auto; margin: 5px auto; padding: 5px; }
        .btn { background: #28a745; color: white; padding: 10px 20px; border: none; cursor: pointer; margin: 5px; }
        .button-container { display: flex; justify-content: center; gap: 10px; margin-top: 10px; }
    </style>
    <script>
        function submitMessage(message) {
            alert(message);
            setTimeout(() => location.reload(), 500);
        }
    </script>
</head>
<body>
    <h2>LifeWatch Config</h2>
    <form action="/get" target="hidden-form">
        <input type="text" name="inputWifiSSID" placeholder="WiFi SSID (%inputWifiSSID%)">
        <input type="text" name="inputWifiPWD" placeholder="WiFi Password">
        <input type="text" name="inputMQTTuser" placeholder="MQTT User (%inputMQTTuser%)">
        <input type="text" name="inputMQTTpwd" placeholder="MQTT Password">
        <input type="number" name="inputTZ" placeholder="Timezone (%inputTZ%)">
        <div class="button-container">
            <input type="submit" class="btn" value="Save" onclick="submitMessage('Saved to LifeWatch!')">
            <input type="submit" class="btn" value="Done" onclick="submitMessage('Finished Setup. LifeWatch will connect to WiFi shortly...')" formaction="/done">
        </div>
    </form>
    <iframe style="display:none" name="hidden-form"></iframe>
</body>
</html>)rawliteral";

class Setup {
public:
    void initSetup();
    String getWiFiSSID();
    String getWiFiPWD();
    void printSetup();
private:
    const char* password = "LifeWatch";
};

extern Setup _setup;
