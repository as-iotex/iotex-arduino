#ifdef OS

#include "storage/storage.h"
#include "helpers/client_helpers.h"

#include <cstring>
#include <stdio.h>

using namespace iotex;

// Define global object
Storage storage;

void Storage::Initialize()
{
}

ResultCode Storage::savePrivateKey(void* storageId,
								   const uint8_t privateKey[IOTEX_PRIVATE_KEY_SIZE])
{
	FILE* pFile;
	char* filePath = (char*)storageId;
	pFile = fopen(filePath, "wb");
	if(pFile == NULL)
	{
		perror("Failed: ");
		return ResultCode::ERROR_STORAGE_OPEN;
	}
	fwrite(privateKey, 1, IOTEX_PRIVATE_KEY_SIZE, pFile);
	fclose(pFile);

	return ResultCode::SUCCESS;
}

ResultCode Storage::readPrivateKey(void* storageId, uint8_t privateKey[IOTEX_PRIVATE_KEY_SIZE])
{
	memset(privateKey, 0, IOTEX_PRIVATE_KEY_SIZE);
	FILE* pFile;
	pFile = fopen((char*)storageId, "rb");
	if(pFile == NULL)
	{
		return ResultCode::ERROR_STORAGE_OPEN;
	}

	size_t readSize = fread(privateKey, 1, IOTEX_PRIVATE_KEY_SIZE, pFile);
	fclose(pFile);

	return (readSize == IOTEX_PRIVATE_KEY_SIZE) ? ResultCode::SUCCESS :
													ResultCode::ERROR_STORAGE_BAD_READ;
}

#endif