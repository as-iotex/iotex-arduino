#ifdef ESP32

#include "helpers/client_helpers.h"
#include "storage/storage.h"
#include "EEPROM.h"

using namespace Iotex;

// Define global object
Storage storage;
bool Storage::_init = false;

void Storage::Initialize()
{
    if (_init)
        return;
    EEPROM.begin(1024); // TODO This size is arbitrary, optimize it or let the user init the eeprom instead
    _init = true;
}

ResultCode Storage::savePrivateKey(void* storageId, const uint8_t privateKey[IOTEX_PRIVATE_KEY_SIZE])
{
    Initialize();
    int startIndex = *((uint32_t*) storageId);
    for (int i = 0; i < IOTEX_PRIVATE_KEY_SIZE; i++)
    {
        EEPROM.write(startIndex + i, privateKey[i]);
    }
    EEPROM.commit();
    return ResultCode::SUCCESS;
}

ResultCode Storage::readPrivateKey(void *storageId, uint8_t privateKey[IOTEX_PRIVATE_KEY_SIZE])
{
    Initialize();
    int startIndex = *((uint32_t*)storageId);
    for (int i = 0; i < IOTEX_PRIVATE_KEY_SIZE; i++)
    {
        privateKey[i] = EEPROM.read(startIndex + i);
    }
    return ResultCode::SUCCESS;
}

#endif