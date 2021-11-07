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
    Serial.println(F("Connected. IP: "));
    Serial.println(WiFi.localIP());
}

void setup() {
    Serial.begin(115200);

    #if defined(__SAMD21G18A__)
    delay(5000);    // Delay for 5000 seconds to allow a serial connection to be established
    #endif

    initWiFi();
}

void loop() {
    // VITA token address
    const char tokenAddress[] = "io1hp6y4eqr90j7tmul4w2wa8pm7wx462hq0mg4tw";
    // Private key of the origin address
    const char pK[] = SECRET_PRIVATE_KEY;

    // Convert the privte key and address hex strings to byte arrays
    uint8_t pk[IOTEX_PRIVATE_KEY_SIZE];
    signer.str2hex(pK, pk, IOTEX_SIGNATURE_SIZE);

    // Create the account 
    iotex::Account originAccount(pk);
    iotex::ResponseTypes::AccountMeta accMeta;
    char addressStr[IOTEX_ADDRESS_STRLEN+1] = {0};
    originAccount.getIotexAddress(addressStr);
    // Convert ETH address to byte array
    char ethAddressStr[ETH_ADDRESS_STRLEN+1] = {0};
    originAccount.getEthereumAddress(ethAddressStr);
    uint8_t address[ETH_ADDRESS_SIZE] = {0};
    signer.str2hex(ethAddressStr, address, sizeof(address));

    // Get the account nonce
    ResultCode result = connection.api.wallets.getAccount(addressStr, accMeta);
    if (result != ResultCode::SUCCESS)
    {
        Serial.print("Error getting account meta: ");
        Serial.println(IotexHelpers.GetResultString(result));
    }
    int nonce = atoi(accMeta.pendingNonce.c_str());

    // Generate call data
    uint8_t data[IOTEX_CONTRACT_ENCODED_TOTALSUPPLY_SIZE] = {0};
    Xrc20Contract::generateCallDataForBalanceOf(address, data);
    char callData[IOTEX_CONTRACT_ENCODED_TOTALSUPPLY_SIZE * 2 +1] = {0};
    Serial.print("Calling contract with data: 0x%");
    signer.hex2str(data, sizeof(data), callData, sizeof(callData));
    Serial.println(callData);

    uint8_t hash[IOTEX_HASH_SIZE] = {0};
    result = originAccount.sendExecutionAction(connection, nonce, 20000000, "1000000000000", "0", tokenAddress, callData, hash);

    Serial.print("Result : ");
    Serial.println(IotexHelpers.GetResultString(result));
    if (result == iotex::ResultCode::SUCCESS)
    {
        Serial.print("Hash: ");
        IOTEX_INFO_BUF(generalLogModule, hash, IOTEX_HASH_SIZE);
    }

    Serial.println("Program finished");

    while (true)
    {
        delay(1000);
    }
}