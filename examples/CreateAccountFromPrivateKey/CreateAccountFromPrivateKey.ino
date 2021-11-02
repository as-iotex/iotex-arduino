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

void setup() {
    Serial.begin(115200);

    #if defined(__SAMD21G18A__)
    delay(5000);    // Delay for 5000 seconds to allow a serial connection to be established
    #endif

    // Private key of the origin address
    const char pK[] = SECRET_PRIVATE_KEY;
    // Convert the privte key hex string byte array
    uint8_t pk[IOTEX_PRIVATE_KEY_SIZE];
    signer.str2hex(pK, pk, IOTEX_SIGNATURE_SIZE);

    // Create the account 
    iotex::Account account(pk);

    // Print account address and public/private key
    uint8_t buffer[IOTEX_PUBLIC_KEY_SIZE] = {0};
    Serial.println(F("Account created:"));
    Serial.print("Private key: ");
    account.getPrivateKey(buffer);
    printHex(buffer, IOTEX_PRIVATE_KEY_SIZE);
    Serial.print("Public key: ");
    account.getPublicKey(buffer);
    printHex(buffer, IOTEX_PUBLIC_KEY_SIZE);
    Serial.print("IoTeX address: ");
    account.getIotexAddress((char*)buffer);
    Serial.println((char*)buffer);
    Serial.print("Eth address: ");
    account.getEthereumAddress((char*)buffer);
    Serial.println((char*)buffer);
}

void loop() {
    delay(1000);
}