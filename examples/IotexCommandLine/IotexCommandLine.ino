#include <Arduino.h>
#include "iotex-client.h"

#ifdef ESP32
    #include <WiFi.h>
#endif
#ifdef ESP8266
    #include <ESP8266WiFi.h>
    #include <ESP8266HTTPClient.h>
    #include <WiFiClient.h>
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
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiSsid, wifiPass);
  Serial.print(F("Connecting to WiFi .."));
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(F("Connected. IP: "));
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

  while (!Serial) { delay(100); };
  // ^for the Arduino Leonardo/Micro only
  initWiFi();

  initMenu();
}

void loop() {
    showMenu();
}