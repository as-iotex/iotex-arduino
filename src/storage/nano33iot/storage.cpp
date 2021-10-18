#ifdef __SAMD21G18A__

#include "helpers/client_helpers.h"
#include "storage/storage.h"
#include "FlashAsEEPROM.h"

using namespace iotex;

// Define global object
Storage storage;
bool Storage::_init = false;

void Storage::Initialize()
{
}

ResultCode Storage::savePrivateKey(void* storageId, const uint8_t privateKey[IOTEX_PRIVATE_KEY_SIZE])
{       
    int startIndex = *((uint32_t *)storageId);
    for (int i = 0; i < IOTEX_PRIVATE_KEY_SIZE; i++)
    {
        EEPROM.write(startIndex + i, privateKey[i]);
    }
    EEPROM.commit();
    return ResultCode::SUCCESS;
}

ResultCode Storage::readPrivateKey(void *storageId, uint8_t privateKey[IOTEX_PRIVATE_KEY_SIZE])
{
    if (!EEPROM.isValid())
    {
        IOTEX_DEBUG("Storage::readPrivateKey: No private keys are stored");
        return ResultCode::ERROR_STORAGE_EMPTY;
    }
        
    int startIndex = *((uint32_t*)storageId);
    for (int i = 0; i < IOTEX_PRIVATE_KEY_SIZE; i++)
    {
        privateKey[i] = EEPROM.read(startIndex + i);
    }
    return ResultCode::SUCCESS;
}

#endif