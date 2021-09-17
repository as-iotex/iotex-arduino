#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include "Arduino.h"
#include "api/wallet/wallets.h"
#include "connection/connection.h"
#include "account/account.h"
#include "iotex/random/random.h"
#include "iotex/storage/storage.h"
#include "iotex/contract/contract.h"
#include "iotex/helpers/client_helpers.h"

#include "erc20Abi.h"
#include "addDataAbi.h"

using namespace Iotex;
using namespace Iotex::abi;

const uint32_t IOTEX_ACCOUNT_EEPROM_ADDRESS = 0;

extern Iotex::Connection<Iotex::Api> connection;
typedef void (*commandFxn)(void);
static std::map < int, std::pair<String, commandFxn>> functionsMap;

void printhex(uint8_t* data, int length)
{
    for (uint8_t i = 0; i < length; i++)
    {
        Serial.printf("%02x", data[i]);
    }
    Serial.println();
}

void printResult(Iotex::ResultCode code)
{
    switch(code)
    {
        case Iotex::ResultCode::SUCCESS:
            Serial.println("SUCCESS");
            break;
        case Iotex::ResultCode::ERROR_HTTP:
            Serial.println("ERROR_HTTP");
            break;
        case Iotex::ResultCode::ERROR_JSON_PARSE:
            Serial.println("ERROR_JSON_PARSE");
            break;
        case Iotex::ResultCode::ERROR_UNKNOWN:
            Serial.println("ERROR_UNKNOWN");
            break;
        case Iotex::ResultCode::ERROR_GRPC:
            Serial.println("ERROR_GRPC");
            break;
        default:
            Serial.println("DEFAULT");
            break;
    }
}

String readLineFromSerial()
{
    while (!Serial.available());
    if (Serial.available() > 0)
    {
        String ret = Serial.readStringUntil('\r');
        while (Serial.available()) Serial.read();  // Flush
        Serial.printf("Input (%d): %s\n", ret.length(), ret.c_str());
        return ret;
    }
    else
    {
        while (Serial.available()) Serial.read();  // Flush
        return "";
    } 
}

void GetBalance()
{
    IotexString balance;
    Serial.println("Executing command GetBalance");
    Serial.println("Enter the iotex address: ");
    String accountStr = readLineFromSerial();
    Iotex::ResultCode result = connection.api.wallets.getBalance(accountStr.c_str(), balance);
    Serial.print("Result : ");
    printResult(result);
    if (result == Iotex::ResultCode::SUCCESS)
    {
        Serial.print("Balance:");
        Serial.println(balance.c_str());
    }
}

void GetAccountMeta()
{
    IotexString balance;
    Serial.println("Executing command GetAccountMeta");
    Serial.println("Enter the iotex address: ");
    String accountStr = readLineFromSerial();
    Iotex::ResponseTypes::AccountMeta accountMeta;
    Iotex::ResultCode result = connection.api.wallets.getAccount(accountStr.c_str(), accountMeta);
    Serial.print("Result : ");
    printResult(result);
    if (result == Iotex::ResultCode::SUCCESS)
    {
        Serial.print("Balance:");
        Serial.println(accountMeta.balance);
        Serial.println("Nonce: ");
        Serial.println(accountMeta.nonce.c_str());
        Serial.println("PendingNonce: ");
        Serial.println(accountMeta.pendingNonce.c_str());
        Serial.println("NumActions: ");
        Serial.println(accountMeta.numActions.c_str());
        Serial.println("IsContract: ");
        Serial.println(accountMeta.isContract ? "\"true\"" : "\"false\"");
    }
}

void GetTransactionByHash()
{
    std::string balance;
    Serial.println("Executing command GetTransactionByHash");
    Serial.println("Enter the hash: ");
    String input = readLineFromSerial();
    Iotex::ResponseTypes::ActionInfo_Transfer data;
    Iotex::ResultCode result = connection.api.wallets.getTransactionByHash(input.c_str(), data);
    Serial.print("Result : ");
    printResult(result);
    if (result == Iotex::ResultCode::SUCCESS)
    {
        Serial.print("Signature:");
        Serial.println(data.action.signature);
    }
}

void CreateAccount()
{
    std::string balance;
    Serial.println("Executing command CreateAccount");
    Serial.println("Enter the private key (empty to generate a new one): ");
    String input = readLineFromSerial();
    uint8_t privKey[IOTEX_PRIVATE_KEY_SIZE] = {0};
    if(input.length() == IOTEX_PRIVATE_KEY_SIZE*2)
    {
        Serial.println("Generating acconunt from provided private key");
        signer.str2hex(input.c_str(), privKey, IOTEX_PRIVATE_KEY_SIZE);
    }
    else
    {
        Serial.println("Generating a new private key");
        randomGenerator.fillRandom(privKey, sizeof(privKey));
    }
    uint8_t buffer[IOTEX_PUBLIC_KEY_SIZE] = {0};
    Iotex::Account account(privKey);
    Serial.println("Account created:");
    Serial.print("Private key: ");
    account.getPrivateKey(buffer);
    printhex(buffer, IOTEX_PRIVATE_KEY_SIZE);
    Serial.print("Public key: ");
    account.getPublicKey(buffer);
    printhex(buffer, IOTEX_PUBLIC_KEY_SIZE);
    Serial.print("Iotex address: ");
    account.getIotexAddress((char*)buffer);
    Serial.println((char*)buffer);
    Serial.print("Eth address: ");
    account.getEthereumAddress((char*)buffer);
    Serial.println((char*)buffer);

    Serial.println("Save this private key to EEPROM? (y/n):");
    input = readLineFromSerial();
    if(input == "y" || input == "yes")
    {
        account.getPrivateKey(buffer);
        storage.savePrivateKey((void *)&IOTEX_ACCOUNT_EEPROM_ADDRESS, buffer);
        Serial.println("Private key saved");
    }
}

void ReadPrivateKeyFromEeprom()
{
    uint8_t buffer[IOTEX_PRIVATE_KEY_SIZE];
    storage.readPrivateKey((void *)&IOTEX_ACCOUNT_EEPROM_ADDRESS, buffer);
    Serial.println("Executing command ReadPrivateKeyFromEeprom");
    Serial.println("Private key: ");
    printhex(buffer, sizeof(buffer));
}

void SignMessage()
{
    Serial.println("Executing command SignMessage");
    Serial.println("Enter the message to sign (max 124 bytes) as a hex string (without x0 prefix). Eg. acbd: ");
    String input = readLineFromSerial();
    uint8_t msgBuf[124] = {0};
    size_t msgSize = input.length() / 2;
    signer.str2hex(input.c_str(), msgBuf, sizeof(msgBuf));
    Serial.println("Enter the private key: ");
    input = readLineFromSerial();
    uint8_t pk[IOTEX_PRIVATE_KEY_SIZE];
    signer.str2hex(input.c_str(), pk, IOTEX_SIGNATURE_SIZE);
    Serial.print("Signing message ");
    printhex(msgBuf, msgSize);
    Serial.print("Using private key ");
    printhex(pk, IOTEX_PRIVATE_KEY_SIZE);

    uint8_t signature[IOTEX_SIGNATURE_SIZE] = {0};
    signer.signMessage(msgBuf, msgSize, pk, signature);
    Serial.print("Signature: ");
    printhex(signature, IOTEX_SIGNATURE_SIZE);
}

void SendTransfer()
{
    Serial.println("Executing command SendTransfer");
    Serial.println("Enter the private key of the source address:");
    String input = readLineFromSerial();
    uint8_t pk[IOTEX_PRIVATE_KEY_SIZE];
    signer.str2hex(input.c_str(), pk, IOTEX_SIGNATURE_SIZE);
    Serial.print("Private key: ");
    printhex(pk, IOTEX_PRIVATE_KEY_SIZE);

    Serial.println("Enter the destination address:");
    String destinationAddr = readLineFromSerial();
    Serial.println("Destination addr: " + destinationAddr);

    Serial.println("Enter the amount in RAU: ");
    String amount = readLineFromSerial();
    Serial.println("Amount: " + amount);

    Iotex::Account originAccount(pk);
    Iotex::ResponseTypes::AccountMeta accMeta;
    char originIotexAddr[IOTEX_ADDRESS_STRLEN] = {0};
    originAccount.getIotexAddress(originIotexAddr);
    connection.api.wallets.getAccount(originIotexAddr, accMeta);
    uint8_t hash[IOTEX_HASH_SIZE] = {0};
    Iotex::ResultCode result = originAccount.sendTokenTransferAction(connection, atoi(accMeta.pendingNonce.c_str()), 20000000, "1000000000000", amount.c_str(), destinationAddr.c_str(), hash);
    Serial.print("Result : ");
    printResult(result);
    if (result == Iotex::ResultCode::SUCCESS)
    {
        Serial.print("Hash: ");
        printhex(hash, IOTEX_HASH_SIZE);
    }
}

void SendTokenTransfer()
{
    Serial.println("Executing command SendTokenTransfer");
    Serial.println("Enter the private key of the source address:");
    String input = readLineFromSerial();
    // String input = "privateKey"; // Uncomment to use harcoded
    uint8_t pk[IOTEX_PRIVATE_KEY_SIZE];
    signer.str2hex(input.c_str(), pk, IOTEX_SIGNATURE_SIZE);
    Serial.print("Private key: ");
    printhex(pk, IOTEX_PRIVATE_KEY_SIZE);

    Serial.println("Enter the destination address (eth representation):");
    String destinationAddr = readLineFromSerial();
    // String destinationAddr = "iotexAddressWithoutIoPrefix"; // Uncomment to use harcoded 
    Serial.println("Destination addr: " + destinationAddr);

    Serial.println("Enter the amount in VITA (max 2^64): ");
    String amount = readLineFromSerial();
    // String amount = "1000000000000000000";  // Uncomment to use harcoded 
    uint64_t nAmount = strtoull(amount.c_str(), NULL, 10);
    Serial.println("Amount: " + amount);

    Iotex::Account originAccount(pk);
    Iotex::ResponseTypes::AccountMeta accMeta;
    char originIotexAddr[IOTEX_ADDRESS_STRLEN] = {0};
    originAccount.getIotexAddress(originIotexAddr);
    connection.api.wallets.getAccount(originIotexAddr, accMeta);
    uint8_t hash[IOTEX_HASH_SIZE] = {0};

    // Contruct the action
    Iotex::ParameterValuesDictionary params;
    ParameterValue paramTo;
    uint8_t toAddress[ETH_ADDRESS_SIZE];
    signer.str2hex(destinationAddr.c_str(), toAddress, ETH_ADDRESS_SIZE);
    paramTo.value.bytes = toAddress;
    paramTo.size = ETH_ADDRESS_SIZE;
    params.emplace(std::pair<IotexString, ParameterValue>("_to", paramTo));    

    ParameterValue paramValue;
    paramValue.value.uint64 = nAmount;
    paramValue.size = sizeof(uint64_t);
    params.emplace(std::pair<IotexString, ParameterValue>("_value", paramValue));

    String abi = erc20AbiJson;
    const char vitaTokenAddress[] = "io1hp6y4eqr90j7tmul4w2wa8pm7wx462hq0mg4tw";
    Contract contract(abi);
    String callData = "";
    contract.generateCallData("transfer", params, callData);
    Serial.printf("Calling contract with data: 0x%s\r\n", callData.c_str());

    Iotex::ResultCode result = originAccount.sendExecutionAction(connection, atoi(accMeta.pendingNonce.c_str()), 20000000, "1000000000000", "0", vitaTokenAddress, callData, hash);

    Serial.print("Result : ");
    printResult(result);
    if (result == Iotex::ResultCode::SUCCESS)
    {
        Serial.print("Hash: ");
        printhex(hash, IOTEX_HASH_SIZE);
    }
}

void AddData()
{
    Serial.println("Executing command AddData");
    Serial.println("Enter the private key of the source address:");
    String input = readLineFromSerial();
    // String input = "privateKey"; // Uncomment to use harcoded
    uint8_t pk[IOTEX_PRIVATE_KEY_SIZE];
    signer.str2hex(input.c_str(), pk, IOTEX_SIGNATURE_SIZE);
    Serial.print("Private key: ");
    printhex(pk, IOTEX_PRIVATE_KEY_SIZE);

    Serial.println("Enter the imei:");
    String imei = readLineFromSerial();
    // String imei = "012345678901234"; // Uncomment to use harcoded 
    Serial.println("Imei: " + imei);
    
    Serial.println("Enter the data (hex string without 0x prefix):");
    String data = readLineFromSerial();
    // String data = "01020304"; // Uncomment to use harcoded 
    Serial.println("Data: " + data);
    
    Serial.println("Enter the signature (hex string without 0x prefix):");
    String signature = readLineFromSerial();
    // String signature = "test-signature"; // Uncomment to use harcoded 
    Serial.println("Signature: " + signature);

    // Create the account
    Iotex::Account originAccount(pk);
    Iotex::ResponseTypes::AccountMeta accMeta;
    char originIotexAddr[IOTEX_ADDRESS_STRLEN] = {0};
    originAccount.getIotexAddress(originIotexAddr);
    connection.api.wallets.getAccount(originIotexAddr, accMeta);

    // Contruct the action
    Iotex::ParameterValuesDictionary params;
    ParameterValue paramImei;
    paramImei.value.string = &imei;
    paramImei.size = ETH_ADDRESS_SIZE;
    params.emplace(std::pair<IotexString, ParameterValue>("imei", paramImei));
    
    ParameterValue paramData;
    uint8_t dataBytes[data.length()/2];
    signer.str2hex(data.c_str(), dataBytes, sizeof(dataBytes));
    paramData.value.bytes = dataBytes;
    paramData.size = sizeof(dataBytes);
    params.emplace(std::pair<IotexString, ParameterValue>("data", paramData));
    
    ParameterValue paramSig;
    uint8_t sigBytes[signature.length()/2];
    signer.str2hex(signature.c_str(), sigBytes, sizeof(sigBytes));
    paramSig.value.bytes = sigBytes;
    paramSig.size = sizeof(sigBytes);
    params.emplace(std::pair<IotexString, ParameterValue>("signature", paramSig));

    // Create the contract
    String abi = addDataAbiJson;
    const char contractAddress[] = "io1n49gavyahsukdvvxxandkxephdx93n3atcrqur";
    Contract contract(abi);

    // Generate call data
    String callData = "";
    contract.generateCallData("addData", params, callData);
    Serial.printf("Calling contract with data: 0x%s\r\n", callData.c_str());

    uint8_t hash[IOTEX_HASH_SIZE] = {0};
    Iotex::ResultCode result = originAccount.sendExecutionAction(connection, atoi(accMeta.pendingNonce.c_str()), 20000000, "1000000000000", "0", contractAddress, callData, hash);

    Serial.print("Result : ");
    printResult(result);
    if (result == Iotex::ResultCode::SUCCESS)
    {
        Serial.print("Hash: ");
        printhex(hash, IOTEX_HASH_SIZE);
    }
}

void initMenu()
{
    functionsMap[1] = std::make_pair("GetBalance", GetBalance);
    functionsMap[2] = std::make_pair("GetAccountMeta", GetAccountMeta);
    functionsMap[3] = std::make_pair("GetTransactionByHash", GetTransactionByHash);
    functionsMap[4] = std::make_pair("CreateAccount", CreateAccount);
    functionsMap[5] = std::make_pair("ReadPrivateKeyFromEeprom", ReadPrivateKeyFromEeprom);
    functionsMap[6] = std::make_pair("SignMessage", SignMessage);
    functionsMap[7] = std::make_pair("SendTransfer", SendTransfer);
    functionsMap[8] = std::make_pair("SendERC20TokenTransfer", SendTokenTransfer);
    functionsMap[9] = std::make_pair("AddData", AddData);
}

#endif