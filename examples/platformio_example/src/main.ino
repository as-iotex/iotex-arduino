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

#include "secrets.h"

using namespace Iotex;
using namespace Iotex::api;
using namespace Iotex::ResponseTypes;

namespace
{
    constexpr const char tTransactionHash[] = "ac675a8440e6e30c3b9dcfb810b772e2ba9e2c1d281db0c1f4bee073b35f890c";
    constexpr const char tAccount[] = "io1xkx7y9ygsa3dlmvzzyvv8zm6hd6rmskh4dawyu";
    // constexpr const char tIp[] = "95.217.43.96";
    constexpr const char tIp[] = "gateway.iotexlab.io";
    constexpr const char tBaseUrl[] = "iotexapi.APIService";
    constexpr const int tPort = 10000;
    constexpr const char wifiSsid[] = WIFI_SSID;
    constexpr const char wifiPass[] = WIFI_PASS;

    constexpr const char privateKeyStr[] = PRIVATE_KEY;

    void printhex(uint8_t* data, int length)
    {
        for (uint8_t i = 0; i < length; i++)
        {
            Serial.printf("%02x", data[i]);
        }
        Serial.println();
    }

    // Create a connection
    Connection<Api> connection(tIp, tPort, tBaseUrl);
}

void printResult(ResultCode code)
{
    switch(code)
    {
        case ResultCode::SUCCESS:
            Serial.println("SUCCESS");
            break;
        case ResultCode::ERROR_HTTP:
            Serial.println("ERROR_HTTP");
            break;
        case ResultCode::ERROR_JSON_PARSE:
            Serial.println("ERROR_JSON_PARSE");
            break;
        case ResultCode::ERROR_UNKNOWN:
            Serial.println("ERROR_UNKNOWN");
            break;
        default:
            Serial.println("DEFAULT");
            break;
    }
}

void initWiFi() {
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

void setup() {
  Serial.begin(115200);

  while (!Serial) { delay(100); };
  // ^for the Arduino Leonardo/Micro only
  initWiFi();

  // Create an account from an existing private key
  uint8_t privateKey[IOTEX_PRIVATE_KEY_SIZE];
  signer.str2hex(privateKeyStr, privateKey, sizeof(privateKey));
  Account existingAccount(privateKey);

  // Create a new account
  Account newAccount;

  // Get the details of the existing account and print them
  char iotexAddrBuf[IOTEX_ADDRESS_STRLEN] = {0};
  char ethAddrBuf[ETH_ADDRESS_STRLEN] = {0};
  uint8_t publicKeyBuf[IOTEX_PUBLIC_KEY_SIZE] = {0};
  existingAccount.getIotexAddress(iotexAddrBuf);
  existingAccount.getEthereumAddress(ethAddrBuf);
  existingAccount.getPublicKey(publicKeyBuf);
  
  Serial.print("\n\n ----------  Generating new account from existing private key ----------\n");
  Serial.printf("Account created from private key: %s\n", privateKeyStr);
  Serial.printf("Iotex address: %s\n", iotexAddrBuf);
  Serial.printf("Eth address: %s\n", ethAddrBuf);
  Serial.print("Public key: ");
  printhex(publicKeyBuf, sizeof(publicKeyBuf));
  Serial.print("\n\n ----------  Generating new random account ----------\n");

  // Get the details of the new account and print them
  uint8_t privateKeyBuf[IOTEX_PRIVATE_KEY_SIZE] = {0};
  newAccount.getIotexAddress(iotexAddrBuf);
  newAccount.getEthereumAddress(ethAddrBuf);
  newAccount.getPublicKey(publicKeyBuf);
  newAccount.getPrivateKey(privateKeyBuf);
  Serial.print("New account generated\nPrivate key: ");
  printhex(privateKeyBuf, sizeof(privateKeyBuf));
  Serial.printf("Iotex address: %s\n", iotexAddrBuf);
  Serial.printf("Eth address: %s\n", ethAddrBuf);
  Serial.print("Public key:");
  printhex(publicKeyBuf, sizeof(publicKeyBuf));
}

/**
 * For debugging internet connection. Just calls a simple endpoint to check if we have internet
 * Not used by default
 */ 
// void testHttp()
// {
//     WiFiClient client;
//     HTTPClient http;

//     String serverPath = "http://jsonplaceholder.typicode.com/todos/1 ";
//     // Your Domain name with URL path or IP address with path
//     http.begin(client, serverPath.c_str());
    
//     // Send HTTP GET request
//     int httpResponseCode = http.GET();

//     if (httpResponseCode>0) {
//         Serial.print("HTTP Response code: ");
//         Serial.println(httpResponseCode);
//         String payload = http.getString();
//         Serial.println(payload);
//       }
//       else {
//         Serial.print("Error code: ");
//         Serial.println(httpResponseCode);
//       }
//       // Free resources
//       http.end();
// }

void loop() {
    // Try all endpoints
    // Print the result and the response data
    std::string balance;
    ResultCode result = connection.api.wallets.getBalance(tAccount, balance);
    Serial.print("\nGetbalance result : ");
    printResult(result);
    if (result == ResultCode::SUCCESS)
    {
        Serial.print("\nBalance:");
        Serial.print(balance.c_str());
    }

    AccountMeta accountMeta;
    result = connection.api.wallets.getAccount(tAccount, accountMeta);
    Serial.print("\nGetAccountMeta result : ");
    printResult(result);
    if (result == ResultCode::SUCCESS)
    {
        Serial.print("\nAddress: ");
        Serial.print(accountMeta.address);
        Serial.print("\nBalance: ");
        Serial.print(accountMeta.balance);
        Serial.print("\nisContract: ");
        Serial.print(accountMeta.isContract);
        Serial.print("\nnonce: ");
        Serial.print(accountMeta.nonce.c_str());
        Serial.print("\npendingNonce: ");
        Serial.print(accountMeta.pendingNonce.c_str());
        Serial.print("\nnumActions: ");
        Serial.print(accountMeta.numActions.c_str());
    }


    ActionInfo_Transfer transaction;
    result = connection.api.wallets.getTransactionByHash(tTransactionHash, transaction);
    Serial.print("\nGetTransactionByHash result : ");
    printResult(result);
    if (result == ResultCode::SUCCESS)
    {
        Serial.print("\nSignature: ");
        Serial.print(transaction.action.signature);
    }

    // Wait some time before retrying
    delay(10000);
    Serial.println("\n\n---------- Waiting for some time before retrying ----------\n");
}