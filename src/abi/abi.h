#ifndef IOTEX_ABI_H
#define IOTEX_ABI_H

#include "helpers/client_helpers.h"
#include "result_codes.h"
#include "helpers/json_helper.h"
#include "extern/cjson/cJSON.h"
#include <vector>

namespace iotex
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
            TUPLE_STATIC,
            TUPLE_DYNAMIC,
            UNKNOWN
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
            "bytes",
            "",
            "",
            "",
            ""
        };

        // Json objects

        class InputOutputAbi
        {
            public:
                IotexString name;
                EthereumTypeName type;
                uint32_t size_bytes;
                EthereumTypeName arrayType;
                uint32_t arrayTypeSizeBytes = 0;
                uint32_t arraySize;

                static ResultCode getTypeAndSizeFromString(IotexString &str, InputOutputAbi& out);
                bool IsDynamic() 
                {
                    bool isDynamic = false;
                    if (type == EthereumTypeName::BYTES_DYNAMIC)
                        isDynamic = true;
                    else if (type == EthereumTypeName::STRING)
                        isDynamic = true;
                    else if (type == EthereumTypeName::ARRAY_DYNAMIC)
                        isDynamic = true;
                    else if (type == EthereumTypeName::TUPLE_DYNAMIC)
                        isDynamic = true;
                    else if (type == EthereumTypeName::ARRAY_STATIC)
                    {
                        if (
                            arrayType == EthereumTypeName::BYTES_DYNAMIC
                            || arrayType == EthereumTypeName::STRING
                            || arrayType == EthereumTypeName::ARRAY_DYNAMIC 
                            || arrayType == EthereumTypeName::TUPLE_DYNAMIC
                        )
                        isDynamic = true;
                    }
                    return isDynamic;
                }

                bool isInteger() const
                {
                    return (
                        type == EthereumTypeName::UINT
                        || type == EthereumTypeName::INT
                    );
                }

            private:
                static ResultCode getSizeFromStringAndCheckIfArray(IotexString& str, InputOutputAbi& out);
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
                    int8_t int8;
                    int16_t int16;
                    int32_t int32;
                    int64_t int64;
                    IotexString *uint256;
                    bool boolean;
                    IotexString *string;
                    uint8_t *bytes;
                    ParameterValue* elements;   // Array or tuple elements
                } value;
                int32_t size = -1;
                bool isBigInt = false;
                EthereumTypeName type = EthereumTypeName::UNKNOWN;  // Only used for arrays and tuples. Needs to be set by the user
            
                bool isDynamic()
                {
                    return (
                        type == EthereumTypeName::BYTES_DYNAMIC ||
                        type == EthereumTypeName::STRING        ||
                        type == EthereumTypeName::ARRAY_DYNAMIC ||
                        type == EthereumTypeName::TUPLE_DYNAMIC 
                        );
                }
        };
    }
}

#endif

