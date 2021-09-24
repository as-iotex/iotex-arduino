#ifndef IOTEX_STORAGE_H
#define IOTEX_STORAGE_H

#include "stdint.h"
#include "stddef.h"
#include "result_codes.h"
#include "constants.h"

namespace Iotex
{
    class Storage
    {
        public:

            // NOTE:
            // Store a byte array at storageId, or read a byte array from storageId
            // Storage id is either a path (c string pointer) or an EEPROM offset (uint32_t pointer) 
            // For OS implementations, paths are used, so a pointer to a path (c string pointer) is expected as storageId 
            // For Arduino, EEPROM is used, so a pointer to an EEPROM offset (uint32_t pointer) is expected as storageId

            virtual ResultCode savePrivateKey(void *storageId, const uint8_t privateKey[IOTEX_PRIVATE_KEY_SIZE]);

            virtual ResultCode readPrivateKey(void *storageId, uint8_t privateKey[IOTEX_PRIVATE_KEY_SIZE]);
        
        private:
            static bool _init;
            static void Initialize();
    };
}

// Declare global object
extern Iotex::Storage storage;

#endif