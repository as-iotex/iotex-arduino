#ifndef IOTEX_STORAGE_H
#define IOTEX_STORAGE_H

#include "IoTeXConstants.h"
#include "IoTeXResultCodes.h"
#include "stddef.h"
#include "stdint.h"

namespace iotex
{
class Storage
{
  public:
	// NOTE:
	// Store a byte array at storageId, or read a byte array from storageId
	// Storage id is either a path (c string pointer) or an EEPROM offset
	// (uint32_t pointer) For OS implementations, paths are used, so a pointer to
	// a path (c string pointer) is expected as storageId For Arduino, EEPROM is
	// used, so a pointer to an EEPROM offset (uint32_t pointer) is expected as
	// storageId

	virtual ResultCode savePrivateKey(void* storageId,
									  const uint8_t privateKey[IOTEX_PRIVATE_KEY_SIZE]);

	virtual ResultCode readPrivateKey(void* storageId, uint8_t privateKey[IOTEX_PRIVATE_KEY_SIZE]);

  private:
	static bool _init;
	static void Initialize();
};
} // namespace iotex

// Declare global object
extern iotex::Storage storage;

#endif