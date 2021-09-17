#include "abi/abi.h"
#include "protobuf/pb_api.h"

using namespace Iotex;
using namespace Iotex::reflection;
using namespace Iotex::json;

ResultCode Iotex::abi::parseContract(const IotexString &contractAbi, std::vector<FunctionAbi> &out)
{
    ResultCode ret = ResultCode::SUCCESS;
    cJSON *data = cJSON_Parse(contractAbi.c_str());
    if (data == NULL)
    {
        IOTEX_DEBUG_F("Iotex::abi::parseContract(): Cannot parse ABI JSON. Wrong format\r\n");
        cJSON_Delete(data);
        return ResultCode::ERROR_JSON_PARSE;
    }

    if (!cJSON_IsArray(data))
    {
        IOTEX_DEBUG_F("Iotex::abi::parseContract(): Provided ABI is not a JSON array\r\n");
        cJSON_Delete(data);
        return ResultCode::ERROR_JSON_PARSE;
    }
    
    const cJSON* element = NULL; 
    cJSON_ArrayForEach(element, data)
    {
        // Only parse functions
        const cJSON *typeJson = cJSON_GetObjectItemCaseSensitive(element, "type");
        IotexString type;
        ret = SetValueFromJsonObject(typeJson, CppType::STRING, (void *)&(type));
        if (ret != ResultCode::SUCCESS)
        {
            IOTEX_DEBUG_F("Iotex::abi::parseContract(): Error parsing element\r\n");
            cJSON_Delete(data);
            return ResultCode::ERROR_JSON_PARSE;
        }
        
        if (type == "function")
        {
            FunctionAbi function;
            if(parseFunction(element, function) != ResultCode::SUCCESS)
            {
                return ResultCode::ERROR_JSON_PARSE;
            }
            out.push_back(std::move(function));
        }
    }

    if (ret != ResultCode::SUCCESS)
    {
        cJSON_Delete(data);
    }

    return ret;
}

ResultCode Iotex::abi::parseFunction(const cJSON* data, FunctionAbi& out)
{
    ResultCode ret = ResultCode::SUCCESS;

    const cJSON *constantJson = cJSON_GetObjectItemCaseSensitive(data, "constant");
    ret = SetValueFromJsonObject(constantJson, CppType::BOOLEAN, (void *)&(out.constant));

    const cJSON *nameJson = cJSON_GetObjectItemCaseSensitive(data, "name");
    ret = SetValueFromJsonObject(nameJson, CppType::STRING, (void *)&(out.name));
    
    // Inputs
    const cJSON *inputs = cJSON_GetObjectItemCaseSensitive(data, "inputs");
    if (!cJSON_IsArray(inputs))
    {
        return ResultCode::ERROR_JSON_PARSE;
    }

    const cJSON* inputJson = NULL; 
    cJSON_ArrayForEach(inputJson, inputs)
    {
        InputOutputAbi input;
        if(parseInputOutput(inputJson, input) != ResultCode::SUCCESS)
        {
            return ResultCode::ERROR_JSON_PARSE;
        }
        out.inputs.push_back(std::move(input));
    }

    // Outputs
    const cJSON* outputJson = NULL; 
    const cJSON *outputs = cJSON_GetObjectItemCaseSensitive(data, "outputs");
    if (!cJSON_IsArray(outputs))
    {
        return ResultCode::ERROR_JSON_PARSE;
    }
    cJSON_ArrayForEach(outputJson, outputs)
    {
        InputOutputAbi output;
        if( parseInputOutput(outputJson, output) != ResultCode::SUCCESS)
        {
            return ResultCode::ERROR_JSON_PARSE;
        }
        out.outputs.push_back(std::move(output));
    }

    const cJSON *payableJson = cJSON_GetObjectItemCaseSensitive(data, "payable");
    ret = SetValueFromJsonObject(payableJson, CppType::BOOLEAN, (void *)&(out.payable));

    const cJSON *mutabilityJson = cJSON_GetObjectItemCaseSensitive(data, "stateMutability");
    ret = SetValueFromJsonObject(mutabilityJson, CppType::STRING, (void *)&(out.stateMutability));

    return ResultCode::SUCCESS;
}

ResultCode Iotex::abi::parseInputOutput(const cJSON* data, InputOutputAbi& out)
{
    ResultCode ret = ResultCode::SUCCESS;

    // Name
    const cJSON *nameJson = cJSON_GetObjectItemCaseSensitive(data, "name");
    ret = SetValueFromJsonObject(nameJson, CppType::STRING, (void *)&(out.name));
    if (ret != ResultCode::SUCCESS)
    {
        IOTEX_DEBUG_F("Iotex::abi::parseInputOutput(): Error parsing name\r\n");
        return ret;
    }

    // Type
    const cJSON *typeJson = cJSON_GetObjectItemCaseSensitive(data, "type");
    IotexString type;
    ret = SetValueFromJsonObject(typeJson, CppType::STRING, (void *)&(type));
    if (ret != ResultCode::SUCCESS)
    {
        IOTEX_DEBUG_F("Iotex::abi::parseInputOutput(): Error parsing type\r\n");
        return ret;
    }
    ret = InputOutputAbi::getTypeAndSizeFromString(type, out.type, out.size_bytes);
    if (ret != ResultCode::SUCCESS)
    {
        IOTEX_DEBUG_F("Iotex::abi::parseInputOutput(): Error parsing type %s\r\n", type.c_str());
        return ret;
    }
    if (
        out.type == EthereumTypeName::ARRAY_DYNAMIC
        || out.type == EthereumTypeName::ARRAY_STATIC
        || out.type == EthereumTypeName::TUPLE
    )
    {
        // TODO Parse element types
    }

    return ResultCode::SUCCESS;
}

ResultCode Iotex::abi::InputOutputAbi::getTypeAndSizeFromString(IotexString &str, EthereumTypeName& type, uint32_t& size)
{
    ResultCode ret = ResultCode::SUCCESS;
    if (str.length() == 0)
    {
        return ResultCode::ERROR_BAD_PARAMETER;
    }
    if (str[0] == '(')
    {
        type = EthereumTypeName::TUPLE;
    }
    else if (0 == memcmp(str.c_str(), EthereumTypeNameToStringLT[EthereumTypeName::UINT], strlen(EthereumTypeNameToStringLT[EthereumTypeName::UINT])))
    {
        type = EthereumTypeName::UINT;
        ret = getSizeFromStringAndCheckIfArray(str, type, size);
    }
    else if (0 == memcmp(str.c_str(), EthereumTypeNameToStringLT[EthereumTypeName::INT], strlen(EthereumTypeNameToStringLT[EthereumTypeName::INT])))
    {
        type = EthereumTypeName::INT;
        ret = getSizeFromStringAndCheckIfArray(str, type, size);
    }
    else if (0 == memcmp(str.c_str(), EthereumTypeNameToStringLT[EthereumTypeName::ADDRESS], strlen(EthereumTypeNameToStringLT[EthereumTypeName::ADDRESS])))
    {
        type = EthereumTypeName::ADDRESS;
        ret = getSizeFromStringAndCheckIfArray(str, type, size);
    }
    else if (0 == memcmp(str.c_str(), EthereumTypeNameToStringLT[EthereumTypeName::BOOL], strlen(EthereumTypeNameToStringLT[EthereumTypeName::BOOL])))
    {
        type = EthereumTypeName::BOOL;
        ret = getSizeFromStringAndCheckIfArray(str, type, size);
    }
    else if (0 == memcmp(str.c_str(), EthereumTypeNameToStringLT[EthereumTypeName::BYTES_STATIC], strlen(EthereumTypeNameToStringLT[EthereumTypeName::BYTES_STATIC])))
    {
        if (str.length() == strlen(EthereumTypeNameToStringLT[EthereumTypeName::BYTES_STATIC]))
        {
            type = EthereumTypeName::BYTES_DYNAMIC;
        }
        else
        {
            type = EthereumTypeName::BYTES_STATIC;
        }
        ret = getSizeFromStringAndCheckIfArray(str, type, size);
    }
    else if (0 == memcmp(str.c_str(), EthereumTypeNameToStringLT[EthereumTypeName::STRING], strlen(EthereumTypeNameToStringLT[EthereumTypeName::STRING])))
    {
        type = EthereumTypeName::STRING;
        ret = getSizeFromStringAndCheckIfArray(str, type, size);
    }

    return ret;
}

ResultCode Iotex::abi::InputOutputAbi::getSizeFromStringAndCheckIfArray(IotexString& str, EthereumTypeName& type, uint32_t& size)
{
    if (
        (type == EthereumTypeName::UINT || type == EthereumTypeName::INT)
        && strlen(EthereumTypeNameToStringLT[type]) == str.length())
    {
        size = 32;
        return ResultCode::SUCCESS;
    }

    const char *pCurr = str.c_str() + strlen(EthereumTypeNameToStringLT[type]);
    
    if (
        type == EthereumTypeName::UINT 
        || type == EthereumTypeName::INT
        || type == EthereumTypeName::BYTES_STATIC
        || type == EthereumTypeName::ARRAY_STATIC
    )
    {
        char sizeBuf[4] = {0};  // Max size for int is 256
        for (int i = 0; i < 4; i++)
        {
            if (*pCurr == '[')
                break;

            if (*pCurr < '0' || *pCurr > '9')
            {
                return ResultCode::ERROR_BAD_PARAMETER;
            }
            sizeBuf[i] = *pCurr++;
            if (pCurr == str.c_str() + str.length())
            {
                break;
            }
        }
        size = atoi(sizeBuf)/8;
    }

    if (pCurr != str.c_str() + str.length())
    {
        // String doesn't end after the size. This is either an array or a bigger than 256 type
        if (*pCurr++ == '[')
        {
            if (*pCurr == ']')
            {
                type = EthereumTypeName::ARRAY_DYNAMIC;
            }
            else
            {
                char buf[24] = {0};
                for (int i = 0; i < str.length(); i++)
                {
                    if (*pCurr == ']')
                        break;
                    buf[i] = pCurr[i];
                    ++pCurr;
                }
                if (*pCurr != ']')
                {
                    // Missing closing ']'
                    return ResultCode::ERROR_BAD_PARAMETER;
                }
            }
        }
    }

    return ResultCode::SUCCESS;
}

void Iotex::abi::FunctionAbi::getSignature(IotexString &out)
{
    out = name;
    out += '(';

    for (int i = 0; i < inputs.size(); i++)
    {
        const auto input = inputs[i];
        out += EthereumTypeNameToStringLT[input.type];
        switch (input.type)
        {
            case EthereumTypeName::INT:
            case EthereumTypeName::UINT:
                {
                    // For function selector the size is required even if the parameter is "uint" or "int"
                    char buf[32] = {0};
                    sprintf(buf, "%d", input.size_bytes*8);
                    out += buf;
                    break;
                }
            case EthereumTypeName::BYTES_STATIC:
                {
                    // For function selector the size is required even if the parameter is "uint" or "int"
                    char buf[32] = {0};
                    sprintf(buf, "%d", input.size_bytes);
                    out += buf;
                    break;
                }

            case EthereumTypeName::BOOL:
            case EthereumTypeName::BYTES_DYNAMIC:
            case EthereumTypeName::ADDRESS:
            case EthereumTypeName::STRING:
                break;

            default:
                IOTEX_DEBUG_F("Type not supported\r\n");
        }

        if (i < inputs.size() - 1) out += ',';
    }
    out += ')';
}
