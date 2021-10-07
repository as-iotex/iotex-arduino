#include <Arduino.h>

#ifdef ESP32
    #include <WiFi.h>
#endif
#ifdef ESP8266
    #include <ESP8266WiFi.h>
    #include <ESP8266HTTPClient.h>
    #include <WiFiClient.h>
#endif

#include <map>
#include "secrets.h"
#include "iotex-client.h"

#include "helpers/client_helpers.h" // For printHex()

constexpr const char tIp[] = "gateway.iotexlab.io";
constexpr const char tBaseUrl[] = "iotexapi.APIService";
constexpr const int tPort = 10000;
constexpr const char wifiSsid[] = WIFI_SSID;
constexpr const char wifiPass[] = WIFI_PASS;
// Create a connection
Connection<Api> connection(tIp, tPort, tBaseUrl);

void printhex(uint8_t* data, int length)
{
    for (uint8_t i = 0; i < length; i++)
    {
        Serial.printf("%02x", data[i]);
    }
    Serial.println();
}

void printResult(iotex::ResultCode code)
{
    switch(code)
    {
        case iotex::ResultCode::SUCCESS:
            Serial.println(F("SUCCESS"));
            break;
        case iotex::ResultCode::ERROR_HTTP:
            Serial.println(F("ERROR_HTTP"));
            break;
        case iotex::ResultCode::ERROR_JSON_PARSE:
            Serial.println(F("ERROR_JSON_PARSE"));
            break;
        case iotex::ResultCode::ERROR_UNKNOWN:
            Serial.println(F("ERROR_UNKNOWN"));
            break;
        case iotex::ResultCode::ERROR_GRPC:
            Serial.println(F("ERROR_GRPC"));
            break;
        default:
            Serial.println(F("DEFAULT"));
            break;
    }
}

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

void setup() {
  Serial.begin(115200);

  while (!Serial) { delay(100); };
  // ^for the Arduino Leonardo/Micro only
  initWiFi();
}

void loop() {
    Serial.println(F("Press any key to initiate transfer"));
    while(!Serial.available()){}
    while(Serial.available()) { Serial.read(); }

    // VITA token address
    const char tokenAddress[] = "io1hp6y4eqr90j7tmul4w2wa8pm7wx462hq0mg4tw";

    const char pK[] = SECRET_PRIVATE_KEY;
    String destinationAddr = "destination-address-eth-representation";
    uint64_t value = 1; // Amount to transfer

    uint8_t pk[IOTEX_PRIVATE_KEY_SIZE];
    signer.str2hex(pK, pk, IOTEX_SIGNATURE_SIZE);
    uint8_t toAddress[ETH_ADDRESS_SIZE];
    signer.str2hex(destinationAddr.c_str(), toAddress, ETH_ADDRESS_SIZE);

    // Contruct the action
    ParameterValuesDictionary params;
    auto paramTo = abi::MakeParamString(destinationAddr);
    auto paramValue = abi::MakeParamUint(value);

    params.emplace(std::pair<IotexString, ParameterValue>("_to", paramTo));
    params.emplace(std::pair<IotexString, ParameterValue>("_value", paramValue));

    uint8_t data[IOTEX_CONTRACT_ENCODED_TRANSFER_SIZE] = {0};
    Xrc20Contract::generateCallDataForTransfer(toAddress, value, data);
    char callData[IOTEX_CONTRACT_ENCODED_TRANSFER_SIZE * 2 + 1] = {0};
    signer.hex2str(data, IOTEX_CONTRACT_ENCODED_TRANSFER_SIZE, callData, sizeof(callData));
    Serial.printf("Calling contract with data: 0x%s\n", callData);

    // Get the nonce
    Account originAccount(pk);
    AccountMeta accMeta;
    char originIotexAddr[IOTEX_ADDRESS_STRLEN] = {0};
    originAccount.getIotexAddress(originIotexAddr);
    connection.api.wallets.getAccount(originIotexAddr, accMeta);

    uint8_t hash[IOTEX_HASH_SIZE] = {0};
    ResultCode result = originAccount.sendExecutionAction(connection, atoi(accMeta.pendingNonce.c_str()), 20000000, "1000000000000", "0", tokenAddress, callData, hash);

    Serial.print("Result : ");
    printResult(result);
    if (result == iotex::ResultCode::SUCCESS)
    {
        Serial.print("Hash: ");
        printhex(hash, IOTEX_HASH_SIZE);
    }
}