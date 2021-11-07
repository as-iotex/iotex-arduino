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
    // VITA token address
    const char tokenAddress[] = "io1hp6y4eqr90j7tmul4w2wa8pm7wx462hq0mg4tw";
    // Private key of the origin address
    const char pkString[] = SECRET_PRIVATE_KEY;
    // Destination address for the transfer (Ethereum address)
    String destAddrString = "0x5840bf8e5d3f5b66EE52B9b933bDAC9682E386D0";
    // Amount to transfer
    uint64_t value = 1000000000000000000;

    // Convert the privte key and address hex strings to byte arrays
    uint8_t pkBytes[IOTEX_PRIVATE_KEY_SIZE];
    signer.str2hex(pkString, pkBytes, IOTEX_SIGNATURE_SIZE);

    // Convert destination address string to byte array
    uint8_t destAddrBytes[ETH_ADDRESS_SIZE];
    signer.str2hex(destAddrString.c_str(), destAddrBytes, ETH_ADDRESS_SIZE);

    // Generate the contract data
    uint8_t data[IOTEX_CONTRACT_ENCODED_TRANSFER_SIZE] = {0};
    Xrc20Contract::generateCallDataForTransfer(destAddrBytes, value, data);
    char callData[IOTEX_CONTRACT_ENCODED_TRANSFER_SIZE * 2 + 1] = {0};
    signer.hex2str(data, IOTEX_CONTRACT_ENCODED_TRANSFER_SIZE, callData, sizeof(callData));
    Serial.print("Calling contract with data: 0x");
    Serial.println(callData);

    // Create an original account and get the nonce
    Account originAccount(pkBytes);
    AccountMeta accMeta;
    char originIotexAddr[IOTEX_ADDRESS_STRLEN] = {0};
    originAccount.getIotexAddress(originIotexAddr);
    connection.api.wallets.getAccount(originIotexAddr, accMeta);

    // Send the token transfer execution action
    uint8_t hash[IOTEX_HASH_SIZE] = {0};
    ResultCode result = originAccount.sendExecutionAction(connection, atoi(accMeta.pendingNonce.c_str()), 20000000, "1000000000000", "0", tokenAddress, callData, hash);

    Serial.print("Result : ");
    printResult(result);
    if (result == iotex::ResultCode::SUCCESS)
    {
        Serial.print("Hash: ");
        printHex(hash, IOTEX_HASH_SIZE);
    }

    Serial.println("Progrm finished");
    while(true)
    {
        delay(1000);
    }
}