#ifndef IOTEX_CONTRACT_H
#define IOTEX_CONTRACT_H

#include <stdint.h>
#include <stddef.h>
#include <vector>
#include <map>
#include "abi/abi.h"

namespace Iotex
{
    typedef std::map<const IotexString, Iotex::abi::ParameterValue> ParameterValuesDictionary;
    class Contract
    {
        public:
            /**
             * \brief   Generates contract class from the abi in JSON format
             */ 
            Contract(IotexString& abiJson);
            Contract(std::vector<Iotex::abi::FunctionAbi>& functions);

            ResultCode generateCallData(const IotexString &functionName, ParameterValuesDictionary &params, IotexString &out);

            static int32_t generateBytesForParameter(Iotex::abi::ParameterValue param, Iotex::abi::EthereumTypeName type, std::vector<uint8_t>& data);
            static int32_t generateBytesForSimpleType(Iotex::abi::ParameterValue param, Iotex::abi::EthereumTypeName type, uint8_t buf[32]);

            static int32_t generateBytesForUint(const uint8_t *pVal, size_t size, uint8_t *out);
            static int32_t generateBytesForInt(const int8_t *pVal, size_t size, uint8_t* out);
            static int32_t generateBytesForAddress(const uint8_t *pVal, uint8_t* out);
            static int32_t generateBytesForBool(bool val, uint8_t* out);
            static int32_t generateBytesForStaticBytes(uint8_t* pVal, size_t size, uint8_t* out);
            static int32_t generateBytesForBytes(uint8_t* pVal, size_t size, uint8_t* out);
            static int32_t generateBytesForString(char* pVal, size_t size, uint8_t* out);
            static int32_t generateBytesForStaticArrayOfStaticElements(Iotex::abi::ParameterValue param, std::vector<uint8_t> &data);
            static int32_t generateBytesForDynamicArrayOfStaticElements(Iotex::abi::ParameterValue param, std::vector<uint8_t> &data);
            static int32_t generateBytesForStaticArrayOfDynamicElements(Iotex::abi::ParameterValue param, std::vector<uint8_t> &data);
            static void generateFunctionSelector(Iotex::abi::FunctionAbi &function, std::vector<uint8_t> &out);

            // Not supported:
            // Nested arrays
            // Tuple
            // Fixed

        private:
            std::vector<Iotex::abi::FunctionAbi> _functions;
    };
}

#endif



