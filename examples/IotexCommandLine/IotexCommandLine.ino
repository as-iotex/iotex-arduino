#include <Arduino.h>
#include "IoTeXClient.h"

#ifdef ESP32
    #include <WiFi.h>
#endif
#ifdef ESP8266
    #include <ESP8266WiFi.h>
    #include <ESP8266HTTPClient.h>
    #include <WiFiClient.h>
#endif
#ifdef __SAMD21G18A__
    #include <WiFiNINA.h>
#endif

#include <map>
#include "commands.h"
#include "secrets.h"


constexpr const char tIp[] = "gateway.iotexlab.io";
constexpr const char tBaseUrl[] = "iotexapi.APIService";
constexpr const int tPort = 10000;
constexpr const char wifiSsid[] = SECRET_WIFI_SSID;
constexpr const char wifiPass[] = SECRET_WIFI_PASS;

// Create a connection
Connection<Api> connection(tIp, tPort, tBaseUrl);

void initWiFi() 
{
    #if defined(ESP8266) || defined(ESP32)
    WiFi.mode(WIFI_STA);
    #endif
    WiFi.begin(wifiSsid, wifiPass);
    Serial.print(F("Connecting to WiFi .."));
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(1000);
    }
    Serial.println(F("\r\nConnected. IP: "));
    Serial.println(WiFi.localIP());
}

void showMenu()
{
    Serial.println(F("\nSelect command"));
    for (auto &function : functionsMap)
    {
        Serial.print(String(function.first) + " - " + function.second.first + "\n");
    }
    String input = readLineFromSerial();
    int cmdIndex = input.toInt();
    if (cmdIndex == 0 || cmdIndex > 10) // TODO max command index
    {
        Serial.println("Unrecognized command" + input);
    }
    else
    {
        (*(functionsMap.at(cmdIndex).second))();
    }
}

void setup() {
    Serial.begin(115200);

    #if defined(__SAMD21G18A__)
    delay(5000);    // Delay for 5000 seconds to allow a serial connection to be established
    #endif

    initWiFi();

    initMenu();

    IotexHelpers.setModuleLogLevel(generalLogModule, IotexLogLevel::INFO);
}

void loop() {
    showMenu();
}