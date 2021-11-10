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
    Serial.println(F("\r\nConnected. IP: "));
    Serial.println(WiFi.localIP());
}

void setup() {
    Serial.begin(115200);

    #if defined(__SAMD21G18A__)
    delay(5000);    // Delay for 5000 seconds to allow a serial connection to be established
    #endif

    initWiFi();
    
    IotexHelpers.setModuleLogLevel(generalLogModule, IotexLogLevel::INFO);
}

void loop() {  
    // Private key of the origin address
    const char pkString[] = SECRET_PRIVATE_KEY;
    
    // Convert the privte key and address hex strings to byte arrays
    uint8_t pkBytes[IOTEX_PRIVATE_KEY_SIZE];
    signer.str2hex(pkString, pkBytes, IOTEX_SIGNATURE_SIZE);

    // Message to sign as a byte array
    const uint8_t message[] = { 0xab, 0xcd };

    // Sign the message
    uint8_t signature[IOTEX_SIGNATURE_SIZE] = {0};
    signer.signMessage(message, sizeof(message), pkBytes, signature);

    // Print the signature
    Serial.print("Signature: ");
    IOTEX_INFO_BUF(generalLogModule, signature, IOTEX_SIGNATURE_SIZE);

    while(true)
    {
        delay(1000);
    }  
}