#ifndef IOTEX_CONTRACT_H
#define IOTEX_CONTRACT_H

#include <stdint.h>
#include <stddef.h>
#include <vector>
#include <map>
#include "abi/abi.h"

namespace iotex
{
    typedef std::map<const IotexString, iotex::abi::ParameterValue> ParameterValuesDictionary;
    class Contract
    {
        public:
            /**
             * \brief   Generates contract class from the abi in JSON format
             */ 
            Contract(IotexString& abiJson);
            Contract(std::vector<iotex::abi::FunctionAbi>& functions);

            ResultCode generateCallData(const IotexString &functionName, ParameterValuesDictionary &params, IotexString &out);

            static int32_t generateBytesForParameter(iotex::abi::ParameterValue param, iotex::abi::EthereumTypeName type, std::vector<uint8_t>& data);
            static int32_t generateBytesForSimpleType(iotex::abi::ParameterValue param, iotex::abi::EthereumTypeName type, uint8_t buf[32]);

            static int32_t generateBytesForUint(const uint8_t *pVal, size_t size, uint8_t *out);
            static int32_t generateBytesForInt(const int8_t *pVal, size_t size, uint8_t* out);
            static int32_t generateBytesForAddress(const uint8_t *pVal, uint8_t* out);
            static int32_t generateBytesForBool(bool val, uint8_t* out);
            static int32_t generateBytesForStaticBytes(uint8_t* pVal, size_t size, uint8_t* out);
            static int32_t generateBytesForBytes(uint8_t* pVal, size_t size, uint8_t* out);
            static int32_t generateBytesForString(char* pVal, size_t size, uint8_t* out);
            static int32_t generateBytesForStaticArrayOfStaticElements(iotex::abi::ParameterValue param, std::vector<uint8_t> &data);
            static int32_t generateBytesForDynamicArrayOfStaticElements(iotex::abi::ParameterValue param, std::vector<uint8_t> &data);
            static int32_t generateBytesForStaticArrayOfDynamicElements(iotex::abi::ParameterValue param, std::vector<uint8_t> &data);
            static void generateFunctionSelector(iotex::abi::FunctionAbi &function, std::vector<uint8_t> &out);

            // Not supported:
            // Nested arrays
            // Tuple
            // Fixed

        private:
            std::vector<iotex::abi::FunctionAbi> _functions;
    };
}

#endif



