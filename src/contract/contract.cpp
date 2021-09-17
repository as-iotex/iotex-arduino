#include "contract/contract.h"
#include "signer/signer.h"
#include <vector>
#include <cmath>

using namespace Iotex;
using namespace std;
using namespace Iotex::abi;


static void EndianSwap(uint8_t* pData, uint64_t size)
{
    if (size < 2) return;
    
    uint8_t tmp;
    uint8_t* lastByte = pData + size - 1;
    
    for (int i = 0; i < size / 2; i++)
    {
        tmp = *(lastByte - i);
        *(lastByte - i) = *(pData + i);
        *(pData + i) = tmp;
    }
}

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION
////////////////////////////////////////////////////////////////////////////////

Iotex::Contract::Contract(IotexString& abiJson)
{
    parseContract(abiJson, _functions);
}

Iotex::Contract::Contract(std::vector<Iotex::abi::FunctionAbi>& functions)
{
    _functions = functions;
}

ResultCode Iotex::Contract::generateCallData(const IotexString& functionName, ParameterValuesDictionary& params, IotexString &out)
{
    std::vector<uint8_t> contractData;

    for (auto& function : _functions)
    {
        if (function.name == functionName)
        {
            uint32_t tailSizes[function.inputs.size()] = {0};
            size_t headsSize = 0;
            std::vector<std::vector<uint8_t>> staticHeadsOrDynamicTails;

            // Function selector
            generateFunctionSelector(function, contractData);
            ConvertToHexString(contractData, out);
            // IOTEX_DEBUG_F("Iotex::Contract::generateCallData:Function selector:\n%s\n", out.c_str());
            
            // Params
            for (int inputIdx = 0; inputIdx < function.inputs.size(); inputIdx++)
            {
                auto &input = function.inputs.at(inputIdx);
                uint32_t remainingInputs = function.inputs.size() - inputIdx;

                // IOTEX_DEBUG_F("Iotex::Contract::generateCallData: Generating bytes for parameter %s\n", input.name.c_str());

                // Validate user passed this key in ParameterValuesDictionary
                if(params.find(input.name) == params.end())
                {
                    // IOTEX_DEBUG_F("Iotex::Contract::generateCallData Parameter %s not supplied by user\n", input.name.c_str());
                    return ResultCode::ERROR_BAD_PARAMETER;
                }

                if (input.type == EthereumTypeName::UINT)
                {
                    uint8_t buf[32];
                    uint32_t bytes;
                    std::vector<uint8_t> data;
                    if (params.at(input.name).size <= 8 && params.at(input.name).isBigInt == false)
                    {
                        bytes  = generateBytesForUint((const uint8_t*)&(params.at(input.name).value.uint8), params.at(input.name).size, buf);
                    }
                    else
                    {
                        // TODO For BigInt data is in the bytes union element
                    }

                    for (int i = 0; i < bytes; i++)
                    {
                        data.push_back(buf[i]);
                    }
                    tailSizes[inputIdx] = bytes;
                    headsSize += bytes;

                    IotexString valString;
                    ConvertToHexString(data, valString);
                    // IOTEX_DEBUG_F("Bytes: %s\n", valString.c_str());
                    staticHeadsOrDynamicTails.push_back(std::move(data));
                }

                if (input.type == EthereumTypeName::INT)
                {
                    uint8_t buf[32];
                    std::vector<uint8_t> data;
                    int32_t bytes = generateBytesForInt((int8_t*)params.at(input.name).value.bytes, input.size_bytes, buf);
                    for (int i = 0; i < bytes; i++)
                    {
                        data.push_back(buf[i]);
                    }

                    tailSizes[inputIdx] = bytes;
                    headsSize += bytes;

                    IotexString valString;
                    ConvertToHexString(data, valString);
                    // IOTEX_DEBUG_F("Bytes: %s\n", valString.c_str());
                    staticHeadsOrDynamicTails.push_back(std::move(data));
                }
                
                if (input.type == EthereumTypeName::BOOL)
                {
                    uint8_t buf[32];
                    std::vector<uint8_t> data;
                    generateBytesForBool(params.at(input.name).value.boolean, buf);
                    for (int i = 0; i < sizeof(buf); i++)
                    {
                        data.push_back(buf[i]);
                    }

                    tailSizes[inputIdx] = 32;
                    headsSize += 32;

                    IotexString valString;
                    ConvertToHexString(data, valString);
                    // IOTEX_DEBUG_F("Bytes: %s\n", valString.c_str());
                    staticHeadsOrDynamicTails.push_back(std::move(data));
                }
                
                if (input.type == EthereumTypeName::ADDRESS)
                {
                    uint8_t buf[32];
                    std::vector<uint8_t> data;
                    int32_t bytes = generateBytesForAddress(params.at(input.name).value.bytes, buf);
                    for (int i = 0; i < sizeof(buf); i++)
                    {
                        data.push_back(buf[i]);
                    }

                    tailSizes[inputIdx] = bytes;
                    headsSize += bytes;

                    IotexString valString;
                    ConvertToHexString(data, valString);
                    // IOTEX_DEBUG_F("Bytes: %s\n", valString.c_str());
                    staticHeadsOrDynamicTails.push_back(std::move(data));
                }

                if (input.type == EthereumTypeName::BYTES_DYNAMIC)
                {
                    auto &currentParam = params.at(input.name);
                    size_t size = currentParam.size;
                    size_t n32ByteGroups = ceil((float)size / 32.0) + 1; // +1 because we have to add the size as an encoded uint
                    uint8_t buf[n32ByteGroups*32];
                    std::vector<uint8_t> data;
                    int32_t bytes = generateBytesForBytes((uint8_t*)currentParam.value.bytes, size, buf);
                    for (int i = 0; i < sizeof(buf); i++)
                    {
                        data.push_back(buf[i]);
                    }
                    
                    tailSizes[inputIdx] = bytes;
                    headsSize += 32;

                    IotexString valString;
                    ConvertToHexString(data, valString);
                    // IOTEX_DEBUG_F("Tail bytes: %s\n", valString.c_str());

                    staticHeadsOrDynamicTails.push_back(std::move(data));
                }

                if (input.type == EthereumTypeName::STRING)
                {
                    auto &currentParam = params.at(input.name);
                    size_t length = currentParam.value.string->length();
                    size_t n32ByteGroups = ceil((float)length / 32.0) + 1;  // +1 because we have to add the size as an encoded uint
                    uint8_t buf[n32ByteGroups*32];
                    std::vector<uint8_t> data;
                    int32_t bytes = generateBytesForBytes((uint8_t*)currentParam.value.string->c_str(), length, buf);
                    for (int i = 0; i < sizeof(buf); i++)
                    {
                        data.push_back(buf[i]);
                    }

                    tailSizes[inputIdx] = bytes;
                    headsSize += 32;

                    IotexString valString;
                    ConvertToHexString(data, valString);
                    // IOTEX_DEBUG_F("Tail bytes: %s\n", valString.c_str());

                    staticHeadsOrDynamicTails.push_back(std::move(data));
                }
            } // for (int inputIdx = 0; inputIdx < function.inputs.size(); inputIdx++)

            // Create the dynamic value headers and move to the ouput buffer
            for (int i = 0; i < staticHeadsOrDynamicTails.size(); i++)
            {
                size_t bytesUntilThisTail = headsSize;
                auto &current = staticHeadsOrDynamicTails[i];
                for (int x = 0; x < i; x++)
                {
                    bytesUntilThisTail += tailSizes[x];
                }

                // If static, copy the tail
                if (!function.inputs[i].IsDynamic())
                {
                    contractData.insert(contractData.end(), current.begin(), current.end());
                }
                // If dynamic, add the header
                else
                {
                    uint8_t head[32] = {0};
                    generateBytesForUint((uint8_t *)&bytesUntilThisTail, sizeof(bytesUntilThisTail), head);
                    for (auto b = 0; b < 32; b++)
                    {
                        contractData.push_back(head[b]);
                    }
                }
            }

            // Now copy the dynamic tails
            for (int i = 0; i < staticHeadsOrDynamicTails.size(); i++)
            {   
                auto &current = staticHeadsOrDynamicTails[i];
                if (function.inputs[i].IsDynamic())
                {
                    contractData.insert(contractData.end(), current.begin(), current.end());
                }
            }

            // 
            ConvertToHexString(contractData, out);
        }
    }

    return ResultCode::SUCCESS;
}

void Iotex::Contract::ConvertToHexString(std::vector<uint8_t>& data, IotexString& out)
{
    out = "";
    char charBuf[3] = {0};
    for (int i = 0; i < data.size(); i++)
    {
        sprintf(charBuf, "%02x", data[i]);
        out += charBuf;
    }
}

int32_t Iotex::Contract::generateBytesForUint(const uint8_t *pVal, size_t size, uint8_t* out)
{
    // IOTEX_DEBUG_F("Iotex::Contract::generateBytesForUint of size %d\n", size);
    if (size > 32 || out == nullptr)
        return -1;
    size_t paddingBytes = 32 - size;
    memset(out, 0, paddingBytes);
    for (size_t i = 0; i < size; i++)
    {
        // Swap endianness when copying
        out[paddingBytes + i] = pVal[size-i-1];
    }
    return 32;
}

int32_t Iotex::Contract::generateBytesForAddress(const uint8_t *pVal, uint8_t* out)
{
    // IOTEX_DEBUG_F("Iotex::Contract::generateBytesForAddress\n");
    uint8_t address[20];
    memcpy(address, pVal, 20);
    EndianSwap(address, 20);
    return generateBytesForUint(address, 20, out);
}

int32_t Iotex::Contract::generateBytesForInt(const int8_t *pVal, size_t size, uint8_t* out)
{
    // IOTEX_DEBUG_F("Iotex::Contract::generateBytesForInt of size %d\n", size);
    if (size > 256 || out == nullptr || (size>8 && size%8))
        return -1;
    size_t paddingBytes = 32 - size;
    memset(out, 0xFF, paddingBytes);
    for (size_t i = 0; i < size; i++)
    {
        // Swap endianness when copying
        out[paddingBytes + i] = pVal[size-i-1];
    }
    return 32;
}

void Iotex::Contract::generateBytesForBool(bool val, uint8_t* out)
{
    // IOTEX_DEBUG_F("Iotex::Contract::generateBytesForBool\n");
    memset(out, 0, 32);
    if(val)
        out[31] = 1;
}

int32_t Iotex::Contract::generateBytesForStaticBytes(uint8_t* pVal, size_t size, uint8_t* out)
{
    // IOTEX_DEBUG_F("Iotex::Contract::generateBytesForStaticBytes of size %d\n", size);
    // Get number of 32 byte groups
    size_t n32ByteGroups = ceil((float)size / 32.0);
    // Get padding bytes
    size_t paddingBytes = (size % 32) ? 32 - (size % 32) : 0;
    
    size_t totalSize = n32ByteGroups * 32;
    // Set padding bytes to 0
    memset(out, 0, totalSize);

    // Copy data bytes
    for (size_t i = 0; i < size; i++)
    {
        out[i] = pVal[i];
    }
    return totalSize;
}

int32_t Iotex::Contract::generateBytesForBytes(uint8_t* pVal, size_t size, uint8_t* out)
{
    // IOTEX_DEBUG_F("Iotex::Contract::generateBytesForBytes of size %d\n", size);
    // Encode length
    size_t encodedBytes = generateBytesForUint((const uint8_t *)&size, sizeof(size_t), out);

    // Encode actual bytes
    encodedBytes += generateBytesForStaticBytes(pVal, size, out + encodedBytes);

    return encodedBytes;
}

int32_t Iotex::Contract::generateBytesForString(char* pVal, size_t size, uint8_t* out)
{
    // IOTEX_DEBUG_F("Iotex::Contract::generateBytesForString of size %d\n", size);
    return generateBytesForBytes((uint8_t*) pVal, size, out);
}

void Iotex::Contract::generateFunctionSelector(Iotex::abi::FunctionAbi& function, std::vector<uint8_t>& out)
{
    IotexString functionSign;
    uint8_t hash[IOTEX_HASH_SIZE];
    function.getSignature(functionSign);
    signer.getHash((const uint8_t*) functionSign.c_str(), functionSign.length(), hash);
    for (int i = 0; i < 4; i++)
    {
        out.push_back(hash[i]);
    }
}
