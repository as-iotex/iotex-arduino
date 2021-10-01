#include "protobuf/pb_api.h"
#include "cjson/cJSON.h"

using namespace Iotex;
using namespace Iotex::ResponseTypes;
using namespace Iotex::api;
using namespace Iotex::reflection;

namespace
{
    bool IsNumeric(const IotexString &s)
    {
        for (const char& c : s)
        {
            if (c < '0' || c > '9')
                return false;
        }
        return true;
    }

    ResultCode SetValueFromJsonObject(const cJSON* json, CppType type, void* pData, size_t max_size = 0)
    {
        // Validate type and size
        switch(type)
        {
            case CppType::OBJECT:
                if(!cJSON_IsObject(json))
                    {return ResultCode::ERROR_JSON_PARSE;}
                break;
            
            case CppType::BOOLEAN:
                if(!cJSON_IsBool(json))
                    {return ResultCode::ERROR_JSON_PARSE;}
                break;

            case CppType::C_STRING:
            case CppType::STRING:
                if(!cJSON_IsString(json) || (max_size > 0 && strlen(json->valuestring) > max_size))
                    {return ResultCode::ERROR_JSON_PARSE;}
                break;

            case CppType::UINT8:
            case CppType::UINT16:
            case CppType::UINT32:
            case CppType::UINT64:
            case CppType::BIGINT:
            case CppType::BYTES:
            case CppType::ARRAY:
                if(!cJSON_IsNumber(json))
                    {return ResultCode::ERROR_JSON_PARSE;}
                break;

            default:
                return ResultCode::ERROR_JSON_PARSE;
            }

        // Set the value
        switch (type)
        {
            case CppType::C_STRING:
                memcpy(pData, json->valuestring, strlen(json->valuestring));
                break;

            case CppType::STRING:
                *(IotexString *)pData = json->valuestring;
                break;

            case CppType::BOOLEAN:
                *(bool*)pData = json->valueint;
                break;

            case CppType::UINT8:
                *(uint8_t*)pData = json->valueint;
                break;

            case CppType::UINT16:
                *(uint16_t*)pData = json->valueint;
                break;

            case CppType::UINT32:
                *(uint32_t*)pData = json->valueint;
                break;

            case CppType::UINT64:
                *(uint64_t*)pData = json->valueint;
                break;
            
            case CppType::OBJECT:
            case CppType::BYTES:
            case CppType::ARRAY:
                break;
            }

        return ResultCode::SUCCESS;
    }
}

ResultCode GetBalanceResponse::fromJson(IotexString jsonString)
{
    ResultCode ret = ResultCode::SUCCESS;
    cJSON *data = cJSON_Parse(jsonString.c_str());
    if (data == NULL)
    {
        cJSON_Delete(data);
        return ResultCode::ERROR_JSON_PARSE;
    }

    // Convert
    const cJSON* accountMeta = cJSON_GetObjectItemCaseSensitive(data, "accountMeta");
    if (cJSON_IsObject(accountMeta))
    {
        const cJSON* balance = cJSON_GetObjectItemCaseSensitive(accountMeta, "balance");
        ret = SetValueFromJsonObject(balance, CppType::STRING, (void *)&(this->balance));
    }

    cJSON_Delete(data);

    return ret;
}

ResultCode GetAccountResponse::fromJson(IotexString jsonString)
{
    ResultCode ret = ResultCode::SUCCESS;
    
    // Clear fields
    memset(this->accountMeta.address, 0, sizeof(this->accountMeta.address));
    memset(this->accountMeta.balance, 0, sizeof(this->accountMeta.balance));

    cJSON *data = cJSON_Parse(jsonString.c_str());
    if (data == NULL)
    {
        cJSON_Delete(data);
        return ResultCode::ERROR_JSON_PARSE;
    }
    const cJSON* accountMeta = cJSON_GetObjectItemCaseSensitive(data, "accountMeta");
    if(!cJSON_IsObject(accountMeta))
    {
        cJSON_Delete(data);
        return ResultCode::ERROR_JSON_PARSE;
    }
    const cJSON* address = cJSON_GetObjectItemCaseSensitive(accountMeta, "address");
    ret = SetValueFromJsonObject(address, CppType::C_STRING, (void *)&(this->accountMeta.address), IOTEX_ADDRESS_STRLEN);
    if (ret != ResultCode::SUCCESS)
    {
        cJSON_Delete(data);
        return ret;
    }

    const cJSON *balance = cJSON_GetObjectItemCaseSensitive(accountMeta, "balance");
    ret = SetValueFromJsonObject(balance, CppType::C_STRING, (void *)&(this->accountMeta.balance), IOTEX_MAX_BALANCE_STRLEN);
    if (ret != ResultCode::SUCCESS)
    {
        cJSON_Delete(data);
        return ret;
    }
    
    const cJSON *nonce = cJSON_GetObjectItemCaseSensitive(accountMeta, "nonce");
    ret = SetValueFromJsonObject(nonce, CppType::STRING, (void *)&(this->accountMeta.nonce));
    if (ret != ResultCode::SUCCESS)
    {
        cJSON_Delete(data);
        return ret;
    }
    
    const cJSON *pendingNonce = cJSON_GetObjectItemCaseSensitive(accountMeta, "pendingNonce");
    ret = SetValueFromJsonObject(pendingNonce, CppType::STRING, (void *)&(this->accountMeta.pendingNonce));
    if (ret != ResultCode::SUCCESS)
    {
        cJSON_Delete(data);
        return ret;
    }
    
    const cJSON *numActions = cJSON_GetObjectItemCaseSensitive(accountMeta, "numActions");
    ret = SetValueFromJsonObject(numActions, CppType::STRING, (void *)&(this->accountMeta.numActions));
    if (ret != ResultCode::SUCCESS)
    {
        cJSON_Delete(data);
        return ret;
    }

    const cJSON *isContract = cJSON_GetObjectItemCaseSensitive(accountMeta, "isContract");
    ret = SetValueFromJsonObject(isContract, CppType::BOOLEAN, (void *)&(this->accountMeta.isContract));
    if (ret != ResultCode::SUCCESS)
    {
        cJSON_Delete(data);
        return ret;
    }

    cJSON_Delete(data);

    return ret;
}

ResultCode GetActionsResponse_Transfer::fromJson(IotexString jsonString)
{
    ResultCode ret = ResultCode::SUCCESS;
    
    // Clear fields
    this->actionInfo.empty();
    this->total = "0";

    cJSON *data = cJSON_Parse(jsonString.c_str());
    if (data == NULL)
    {
        cJSON_Delete(data);
        return ResultCode::ERROR_JSON_PARSE;
    }

    // Set Total
    const cJSON* total = cJSON_GetObjectItemCaseSensitive(data, "total");
    ret = SetValueFromJsonObject(total, CppType::STRING, (void *)&(this->total));
    if (ret != ResultCode::SUCCESS)
    {
        cJSON_Delete(data);
        return ret;
    }

    // Set ActionInfo
    ActionInfo_Transfer actionInfoResponse;
    memset(actionInfoResponse.actHash, 0, sizeof(actionInfoResponse.actHash));
    memset(actionInfoResponse.blkHash, 0, sizeof(actionInfoResponse.blkHash));
    memset(actionInfoResponse.timestamp, 0, sizeof(actionInfoResponse.timestamp));
    memset(actionInfoResponse.sender, 0, sizeof(actionInfoResponse.sender));
    memset(actionInfoResponse.gasFee, 0, sizeof(actionInfoResponse.gasFee));
    memset(actionInfoResponse.action.senderPublicKey, 0, sizeof(actionInfoResponse.action.senderPublicKey));
    memset(actionInfoResponse.action.signature, 0, sizeof(actionInfoResponse.action.signature));
    memset(actionInfoResponse.action.core.gasPrice, 0, sizeof(actionInfoResponse.action.core.gasPrice));
    memset(actionInfoResponse.action.core.transfer.amount, 0, sizeof(actionInfoResponse.action.core.transfer.amount));
    memset(actionInfoResponse.action.core.transfer.recipient, 0, sizeof(actionInfoResponse.action.core.transfer.recipient));

    const cJSON *actionInfoArray = cJSON_GetObjectItemCaseSensitive(data, "actionInfo");
    if (!cJSON_IsArray(actionInfoArray))
    {
        cJSON_Delete(data);
        return ResultCode::ERROR_JSON_PARSE;
    }
    
    const cJSON *actionInfo = cJSON_GetArrayItem(actionInfoArray, 0);
    if (!cJSON_IsObject(actionInfo))
    {
        cJSON_Delete(data);
        return ResultCode::ERROR_JSON_PARSE;
    }

    // Action data and core
    const cJSON* action = cJSON_GetObjectItemCaseSensitive(actionInfo, "action");
    if (!cJSON_IsObject(action))
    {
        cJSON_Delete(data);
        return ResultCode::ERROR_JSON_PARSE;
    }
    const cJSON* core = cJSON_GetObjectItemCaseSensitive(action, "core");
    if (!cJSON_IsObject(core))
    {
        cJSON_Delete(data);
        return ResultCode::ERROR_JSON_PARSE;
    }
    const cJSON* transfer = cJSON_GetObjectItemCaseSensitive(core, "transfer");
    if (!cJSON_IsObject(transfer))
    {
        cJSON_Delete(data);
        return ResultCode::ERROR_JSON_PARSE;
    }
    
    // Action - Sender public key
    const cJSON* senderPubKey = cJSON_GetObjectItemCaseSensitive(action, "senderPubKey");
    ret = SetValueFromJsonObject(senderPubKey, CppType::C_STRING, (void *)&(actionInfoResponse.action.senderPublicKey), IOTEX_PUBLIC_KEY_STRLEN);
    if (ret != ResultCode::SUCCESS)
    {
        cJSON_Delete(data);
        return ret;
    }

    // Action - Signature
    const cJSON* signature = cJSON_GetObjectItemCaseSensitive(action, "signature");
    ret = SetValueFromJsonObject(signature, CppType::C_STRING, (void *)&(actionInfoResponse.action.signature), IOTEX_SIGNATURE_STRLEN);
    if (ret != ResultCode::SUCCESS)
    {
        cJSON_Delete(data);
        return ret;
    }

    // Action - core - version
    const cJSON* version = cJSON_GetObjectItemCaseSensitive(core, "version");
    ret = SetValueFromJsonObject(version, CppType::UINT32, (void *)&(actionInfoResponse.action.core.version));
    if (ret != ResultCode::SUCCESS)
    {
        cJSON_Delete(data);
        return ret;
    }
    
    // Action - core - nonce
    const cJSON* nonce = cJSON_GetObjectItemCaseSensitive(core, "nonce");
    // Nonce is string in the HTTP response, but uint64 in the protobuf
    // As a workaround, we get the string value and convert to int here
    IotexString buf;
    ret = SetValueFromJsonObject(nonce, CppType::STRING, (void*)&buf);
    if (ret != ResultCode::SUCCESS || !IsNumeric(buf))
    {
        cJSON_Delete(data);
        return ret;
    }
    actionInfoResponse.action.core.nonce = atol(buf.c_str());

    // Action - core - gasLimit
    const cJSON* gasLimit = cJSON_GetObjectItemCaseSensitive(core, "gasLimit");
    // GasLimit is string in the HTTP response, but uint64 in the protobuf
    // As a workaround, we get the string value and convert to int here
    ret = SetValueFromJsonObject(gasLimit, CppType::STRING, &buf);
    if (ret != ResultCode::SUCCESS)
    {
        cJSON_Delete(data);
        return ret;
    }
    actionInfoResponse.action.core.gasLimit = atol(buf.c_str());

    // Action - core - gasPrice
    const cJSON* gasPrice = cJSON_GetObjectItemCaseSensitive(core, "gasPrice");
    ret = SetValueFromJsonObject(gasPrice, CppType::C_STRING, (void *)&(actionInfoResponse.action.core.gasPrice), IOTEX_MAX_BALANCE_STRLEN);
    if (ret != ResultCode::SUCCESS)
    {
        cJSON_Delete(data);
        return ret;
    }

    // Action - core - transfer - amount
    const cJSON* amount = cJSON_GetObjectItemCaseSensitive(transfer, "amount");
    ret = SetValueFromJsonObject(amount, CppType::C_STRING, (void *)&(actionInfoResponse.action.core.transfer.amount), IOTEX_MAX_BALANCE_STRLEN);
    if (ret != ResultCode::SUCCESS)
    {
        cJSON_Delete(data);
        return ret;
    }

    // Action - core - transfer - recipient
    const cJSON* recipient = cJSON_GetObjectItemCaseSensitive(transfer, "recipient");
    ret = SetValueFromJsonObject(recipient, CppType::C_STRING, (void *)&(actionInfoResponse.action.core.transfer.recipient), IOTEX_ADDRESS_STRLEN);
    if (ret != ResultCode::SUCCESS)
    {
        cJSON_Delete(data);
        return ret;
    }
  
    // Action - core - transfer - payload
    const cJSON* payload = cJSON_GetObjectItemCaseSensitive(transfer, "payload");
    ret = SetValueFromJsonObject(payload, CppType::STRING, (void *)&(actionInfoResponse.action.core.transfer.payload));
    if (ret != ResultCode::SUCCESS)
    {
        cJSON_Delete(data);
        return ret;
    }

    // Action hash
    const cJSON* actHash = cJSON_GetObjectItemCaseSensitive(actionInfo, "actHash");
    ret = SetValueFromJsonObject(actHash, CppType::C_STRING, (void *)&(actionInfoResponse.actHash), IOTEX_HASH_STRLEN);
    if (ret != ResultCode::SUCCESS)
    {
        cJSON_Delete(data);
        return ret;
    }

    // Blk hash
    const cJSON* blkHash = cJSON_GetObjectItemCaseSensitive(actionInfo, "blkHash");
    ret = SetValueFromJsonObject(blkHash, CppType::C_STRING, (void *)&(actionInfoResponse.blkHash), IOTEX_HASH_STRLEN);
    if (ret != ResultCode::SUCCESS)
    {
        cJSON_Delete(data);
        return ret;
    }

    // Timestamp
    const cJSON* timestamp = cJSON_GetObjectItemCaseSensitive(actionInfo, "timestamp");
    ret = SetValueFromJsonObject(timestamp, CppType::C_STRING, (void *)&(actionInfoResponse.timestamp), IOTEX_TIMESTAMP_STRLEN);
    if (ret != ResultCode::SUCCESS)
    {
        cJSON_Delete(data);
        return ret;
    }

    // Blk height
    const cJSON* blkHeight = cJSON_GetObjectItemCaseSensitive(actionInfo, "blkHeight");
    ret = SetValueFromJsonObject(blkHeight, CppType::STRING, (void *)&(actionInfoResponse.blkHeight));
    if (ret != ResultCode::SUCCESS)
    {
        cJSON_Delete(data);
        return ret;
    }
    
    // Sender
    const cJSON* sender = cJSON_GetObjectItemCaseSensitive(actionInfo, "sender");
    ret = SetValueFromJsonObject(sender, CppType::C_STRING, (void *)&(actionInfoResponse.sender), IOTEX_ADDRESS_STRLEN);
    if (ret != ResultCode::SUCCESS)
    {
        cJSON_Delete(data);
        return ret;
    }
    
    // GasFee
    const cJSON* gasFee = cJSON_GetObjectItemCaseSensitive(actionInfo, "gasFee");
    ret = SetValueFromJsonObject(gasFee, CppType::C_STRING, (void *)&(actionInfoResponse.gasFee), IOTEX_MAX_BALANCE_STRLEN);
    if (ret != ResultCode::SUCCESS)
    {
        cJSON_Delete(data);
        return ret;
    }

    // Store
    this->actionInfo.push_back(actionInfoResponse);
    
    cJSON_Delete(data);

    return ret;
}

ResultCode SendExecutionResponse::fromJson(IotexString jsonString)
{
    ResultCode ret = ResultCode::SUCCESS;
    cJSON *data = cJSON_Parse(jsonString.c_str());
    if (data == NULL)
    {
        cJSON_Delete(data);
        return ResultCode::ERROR_JSON_PARSE;
    }

    // Convert
    memset(hash, 0, sizeof(hash));
    const cJSON *hash = cJSON_GetObjectItemCaseSensitive(data, "actionHash");
    ret = SetValueFromJsonObject(hash, CppType::C_STRING, (void *)&(this->hash), IOTEX_HASH_STRLEN);

    cJSON_Delete(data);

    return ret;
}