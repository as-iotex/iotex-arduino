#ifndef IOTEX_ABI_H
#define IOTEX_ABI_H

#include "helpers/client_helpers.h"
#include "result_codes.h"
#include "helpers/json_helper.h"
#include "cjson/cJSON.h"
#include <vector>

namespace Iotex
{
    namespace abi
    {
        // Unused for now
        // Enums. Do not change order of elements as lookup tables depend on it
        // enum FunctionType
        // {
        //     ABI_FUNCTION_TYPE_FUNCTION,
        //     ABI_FUNCTION_TYPE_CONSTRUCTOR,
        //     ABI_FUNCTION_TYPE_RECEIVE,
        //     ABI_FUNCTION_TYPE_FALLBACK
        // };

        // enum FunctionStateMutability
        // {
        //     ABI_FUNCTION_SM_PURE,
        //     ABI_FUNCTION_SM_VIEW,
        //     ABI_FUNCTION_SM_PAYABLE,
        //     ABI_FUNCTION_SM_NONPAYABLE
        // };

        enum EthereumTypeName
        {
            UINT,
            INT,
            ADDRESS,
            BOOL,
            FIXED,
            UFIXED,
            BYTES_STATIC,
            STRING,
            BYTES_DYNAMIC,
            ARRAY_STATIC,
            ARRAY_DYNAMIC,
            TUPLE
        };

        // Enum to string lookup tables - Unused for now
        // static const char* functionTypeStringLT[] =
        // {
        //     "function",
        //     "constructor",
        //     "receive",
        //     "fallback"
        // };

        // static const char* functionStateMutabilityStringLT[] =
        // {
        //     "pure",
        //     "view",
        //     "payable",
        //     "nonpayable"
        // };

        static const char *EthereumTypeNameToStringLT[] =
        {
            "uint",
            "int",
            "address",
            "bool",
            "fixed",
            "ufixed",
            "bytes",
            "string",
            "bytes"
        };

        // Json objects

        class InputOutputAbi
        {
            public:
                IotexString name;
                EthereumTypeName type;
                uint32_t size_bytes;
                EthereumTypeName arrayType;
                uint32_t arraySize;

                static ResultCode getTypeAndSizeFromString(IotexString &str, EthereumTypeName& type, uint32_t& size);
                bool IsDynamic() 
                {
                    return (
                        type == EthereumTypeName::BYTES_DYNAMIC ||
                        type == EthereumTypeName::STRING        ||
                        type == EthereumTypeName::ARRAY_DYNAMIC );
                }

            private:
                static ResultCode getSizeFromStringAndCheckIfArray(IotexString& str, EthereumTypeName& type, uint32_t& size);
        };

        class FunctionAbi
        {
            public:
                bool constant;
                std::vector<InputOutputAbi> inputs; // Not present in fallback
                IotexString name;
                std::vector<InputOutputAbi> outputs; // Not present in constructor or fallback
                IotexString stateMutability;
                bool payable;

                void getSignature(IotexString &out);
        };

        // Parsing
        ResultCode parseContract(const IotexString &contractAbi, std::vector<FunctionAbi> &out);
        ResultCode parseFunction(const cJSON* data, FunctionAbi &out);
        ResultCode parseInputOutput(const cJSON* data, InputOutputAbi& out);

        // Utils
        class ParameterValue
        {
            public:
                union
                {
                    uint8_t uint8;
                    uint16_t uint16;
                    uint32_t uint32;
                    uint64_t uint64;
                    IotexString *uint256;
                    bool boolean;
                    char *c_string;
                    IotexString *string;
                    void *object;
                    uint8_t *bytes;
                } value;
                int32_t size = -1;
                bool isBigInt = false;
        };
    }
}

#endif

