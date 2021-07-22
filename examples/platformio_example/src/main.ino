#include <Arduino.h>

#ifdef ESP32
    #include <WiFi.h>
#endif
#ifdef ESP8266
    #include <ESP8266WiFi.h>
    #include <ESP8266HTTPClient.h>
    #include <WiFiClient.h>
#endif

#include "api/wallet/wallets.h"
#include "connection/connection.h"
#include "account/account.h"
#include <map>
#include "secrets.h"
    #include "commands.h"

using namespace Iotex;
using namespace Iotex::api;
using namespace Iotex::ResponseTypes;

constexpr const char tIp[] = "gateway.iotexlab.io";
constexpr const char tBaseUrl[] = "iotexapi.APIService";
constexpr const int tPort = 10000;
constexpr const char wifiSsid[] = WIFI_SSID;
constexpr const char wifiPass[] = WIFI_PASS;
// Create a connection
Connection<Api> connection(tIp, tPort, tBaseUrl);

void initWiFi() 
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiSsid, wifiPass);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println("Connected. IP: ");
  Serial.println(WiFi.localIP());
}


void showMenu()
{
    Serial.println("\nSelect command");
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