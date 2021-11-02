#include <Arduino.h>

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
#include "secrets.h"
#include "iotex-client.h"               // Include IoTex-client

constexpr const char tIp[] = "gateway.iotexlab.io";
constexpr const char tBaseUrl[] = "iotexapi.APIService";
constexpr const int tPort = 10000;
constexpr const char wifiSsid[] = SECRET_WIFI_SSID;
constexpr const char wifiPass[] = SECRET_WIFI_PASS;

// Create the IoTeX client connection
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
    Serial.println(F("Connected. IP: "));
    Serial.println(WiFi.localIP());
}

String readLineFromSerial()
{
    while (!Serial.available());
    if (Serial.available() > 0)
    {
        String ret = Serial.readStringUntil('\r');
        while (Serial.available()) Serial.read();  // Flush
        char buf[256] = {0};
        sprintf(buf, "Input (%d): %s\n", ret.length(), ret.c_str());
        Serial.print(buf);
        return ret;
    }
    else
    {
        while (Serial.available()) Serial.read();  // Flush
        return "";
    } 
}

void setup() {
    Serial.begin(115200);

    #if defined(__SAMD21G18A__)
    delay(5000);    // Delay for 5000 seconds to allow a serial connection to be established
    #endif

    initWiFi();

    Serial.println(F("Enter the iotex address: "));
    String accountStr = readLineFromSerial();
    iotex::ResponseTypes::AccountMeta accountMeta;
    iotex::ResultCode result = connection.api.wallets.getAccount(accountStr.c_str(), accountMeta);
    
    // Print the result and account metadata
    Serial.print("Result : ");
    printResult(result);
    if (result == iotex::ResultCode::SUCCESS)
    {
        Serial.print("Balance:");
        Serial.println(accountMeta.balance);
        Serial.println(F("Nonce: "));
        Serial.println(accountMeta.nonce.c_str());
        Serial.println(F("PendingNonce: "));
        Serial.println(accountMeta.pendingNonce.c_str());
        Serial.println(F("NumActions: "));
        Serial.println(accountMeta.numActions.c_str());
        Serial.println(F("IsContract: "));
        Serial.println(accountMeta.isContract ? "\"true\"" : "\"false\"");
    } 
}

void loop() {
}
