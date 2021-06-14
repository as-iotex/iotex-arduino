#include <Arduino.h>

#include "api/wallet/wallets.h"
#include "connection/connection.h"

namespace
{
    constexpr const char tTransactionHash[] = "ac675a8440e6e30c3b9dcfb810b772e2ba9e2c1d281db0c1f4bee073b35f890c";
    constexpr const char tAccount[] = "io1xkx7y9ygsa3dlmvzzyvv8zm6hd6rmskh4dawyu";
    constexpr const char tIp[] = "gateway.iotexlab.io";
    constexpr const char tBaseUrl[] = "iotexapi.APIService";
    constexpr const int tPort = 10000;
}

using namespace Iotex;
using namespace Iotex::api;
using namespace Iotex::ResponseTypes;

std::string ResultString(ResultCode code)
{
    switch(code)
    {
        case ResultCode::SUCCESS:
            return "SUCESS";
        case ResultCode::ERROR_HTTP:
            return "ERROR_HTTP";
        case ResultCode::ERROR_JSON_PARSE:
            return "ERROR_JSON_PARSE";
        case ResultCode::ERROR_UNKNOWN:
            return "ERROR_UNKNOWN";
        default:
            return "UNKNOWN";
    }
}

void setup() {
  Serial.begin(115200);

  while (!Serial) { delay(100); };
  // ^for the Arduino Leonardo/Micro only
}

void loop() {
    // Create a connection
    Connection<Api> connection(tIp, tPort, tBaseUrl);

    // Try all endpoints
    // Print the result and the response data
    std::string balance;
    ResultCode result = connection.api.wallets.getBalance(tAccount, balance);
    Serial.print("\nGetbalance result : ");
    Serial.print(ResultString(result).c_str());
    Serial.print("\nBalance:");
    Serial.print(balance.c_str());

    AccountMeta accountMeta;
    result = connection.api.wallets.getAccount(tAccount, accountMeta);
    Serial.print("\nGetAccountMeta result : ");
    Serial.print(ResultString(result).c_str());
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


    ActionInfo_Transfer transaction;
    result = connection.api.wallets.getTransactionByHash(tTransactionHash, transaction);
    Serial.print("\nGetTransactionByHash result : ");
    Serial.print(ResultString(result).c_str());
    Serial.print("\nSignature: ");
    Serial.print(transaction.action.signature);

    // Wait some time before retrying
    delay(10000);
}