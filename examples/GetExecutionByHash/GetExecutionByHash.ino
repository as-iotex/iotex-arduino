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

    Serial.println(F("Enter the execution hash: "));
    String input = readLineFromSerial();
    iotex::ResponseTypes::ActionInfo_Execution data;
    iotex::ResultCode result = connection.api.wallets.getExecutionByHash(input.c_str(), data);
    
    // Print the transaction details
    Serial.print("Result : ");
    printResult(result);
    if (result == iotex::ResultCode::SUCCESS)
    {
        Serial.print("Sender public key: ");
        Serial.println(data.action.senderPublicKey);
        Serial.print("Signature: ");
        Serial.println(data.action.signature);
        Serial.print("Gas limit: ");
        Serial.println(data.action.core.gasLimit);
        Serial.print("Gas price: ");
        Serial.println(data.action.core.gasPrice);
        Serial.print("Nonce: ");
        Serial.println(data.action.core.nonce);
        Serial.print("Amount: ");
        Serial.println(data.action.core.execution.amount);
        Serial.print("Contract: ");
        Serial.println(data.action.core.execution.contract);
        Serial.print("Data: ");
        Serial.println(data.action.core.execution.data);
    }
}

void loop() {
}
