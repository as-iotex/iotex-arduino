#include "account/account.h"

#include <vector>

using namespace Iotex;
using namespace std;

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION
////////////////////////////////////////////////////////////////////////////////

Account::Account()
{
    setDepsFromGlobals();
    // Generate a new private key
    _pRandomGenerator->fillRandom(_privateKey, sizeof(_privateKey));
    GenerateAddressesFromPrivateKey();
}

Account::Account(uint8_t privateKey[IOTEX_PRIVATE_KEY_SIZE])
{
    setDepsFromGlobals();
    // Copy the private key
    memcpy(_privateKey, privateKey, IOTEX_PRIVATE_KEY_SIZE);
    GenerateAddressesFromPrivateKey();
}

Account::Account(Random* pRandomGenerator, Encoder* pEncoder, Signer* pSigner, uint8_t privateKey[IOTEX_PRIVATE_KEY_SIZE])
{
    _pRandomGenerator = pRandomGenerator;
    _pSigner = pSigner;
    _pEncoder = pEncoder;
    
    if (privateKey)
    {
        memcpy(_privateKey, privateKey, IOTEX_PRIVATE_KEY_SIZE);
    }
    else
    {
        _pRandomGenerator->fillRandom(_privateKey, sizeof(_privateKey));
    }
    GenerateAddressesFromPrivateKey();
}





////////////////////////////////////////////////////////////////////////////////
// GETTERS
////////////////////////////////////////////////////////////////////////////////

void Account::getIotexAddress(char buffer[IOTEX_ADDRESS_STRLEN])
{
    
    memcpy(buffer, _iotexAddr.c_str(), IOTEX_ADDRESS_STRLEN);
}

void Account::getEthereumAddress(char buffer[IOTEX_ADDRESS_STRLEN])
{
    memcpy(buffer, _ethAddr.c_str(), IOTEX_ADDRESS_STRLEN);
}

void Account::getPublicKey(uint8_t buffer[IOTEX_PUBLIC_KEY_SIZE])
{
    memcpy(buffer, _publicKey, IOTEX_PUBLIC_KEY_SIZE);
}

void Account::getPrivateKey(uint8_t buffer[IOTEX_PRIVATE_KEY_SIZE])
{
    memcpy(buffer, _privateKey, IOTEX_PRIVATE_KEY_SIZE);
}




////////////////////////////////////////////////////////////////////////////////
// SIGNING
////////////////////////////////////////////////////////////////////////////////

void Account::signMessage(const uint8_t *message, size_t size, uint8_t signature[IOTEX_SIGNATURE_SIZE])
{
    _pSigner->signMessage(message, size, _privateKey, signature);
}

void Account::signTokenTransferAction(Iotex::ResponseTypes::Action_Transfer &transfer, uint8_t signature[IOTEX_SIGNATURE_SIZE])
{
    // TODO
}

ResultCode Account::sendTokeTransferAction(Iotex::ResponseTypes::Action_Transfer &transfer)
{
    // TODO
    return ResultCode::ERROR_UNKNOWN;
}




////////////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
////////////////////////////////////////////////////////////////////////////////

void Account::GenerateAddressesFromPrivateKey()
{
    // Get the corresponding public key
    _pSigner->getPublicKey((const uint8_t *)_privateKey, _publicKey);
    generateIotexAddress();
    generateEthAddress();
}

void Account::generateIotexAddress()
{
    // Apply keccak256 hash function to the public key, excluding the first byte
    uint8_t hash[IOTEX_HASH_SIZE] = {0};
    _pSigner->getHash((const uint8_t *)_publicKey + 1, sizeof(_publicKey) - 1, hash);

    // Encode the last 20 bytes of the hash using bech32
    vector<uint8_t> bytes(hash + sizeof(hash) - 20, hash + sizeof(hash));
    _pEncoder->bech32_encode(bytes, _iotexAddr);
}

void Account::generateEthAddress()
{
    // Apply keccak256 hash function to the public key, excluding the first byte
    uint8_t hash[IOTEX_HASH_SIZE] = {0};
    _pSigner->getHash((const uint8_t *)_publicKey + 1, sizeof(_publicKey) - 1, hash);
    char hashLast20[41] = {0};
    _pSigner->hex2str(hash + sizeof(hash) - 20, 20, hashLast20, 41);
    _ethAddr = "0x";
    _ethAddr += hashLast20;
}

void Account::setDepsFromGlobals()
{
    // Set dependencies to the global objects
    _pRandomGenerator = &randomGenerator;
    _pEncoder = &encoder;
    _pSigner = &signer;
}
