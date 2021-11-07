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
#include "IoTeXClient.h"

const char tIp[] = "gateway.iotexlab.io";
const char tBaseUrl[] = "iotexapi.APIService";
const int tPort = 10000;
const char wifiSsid[] = SECRET_WIFI_SSID;
const char wifiPass[] = SECRET_WIFI_PASS;

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
    Serial.println(F("\r\nConnected. IP: "));
    Serial.println(WiFi.localIP());
}

void setup() {
    Serial.begin(115200);

    #if defined(__SAMD21G18A__)
    delay(5000);    // Delay for 5000 seconds to allow a serial connection to be established
    #endif

    initWiFi();

    const char accountStr[] = "io1xkx7y9ygsa3dlmvzzyvv8zm6hd6rmskh4dawyu";
    iotex::ResponseTypes::AccountMeta accountMeta;
    iotex::ResultCode result = connection.api.wallets.getAccount(accountStr, accountMeta);
    
    // Print the result
    Serial.print("Result : ");
    Serial.print(IotexHelpers.GetResultString(result));

    // If the query suceeded, print the account metadata
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
