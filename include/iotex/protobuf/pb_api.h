/**
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 **/

#ifndef PB_API_H
#define PB_API_H

#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>

#include "api/base.h"
#include "result_codes.h"

namespace Iotex 
{
  /**
   * @namespace		ResponseTypes	  Protobuf response messages mapped to c structs that might support reflection and JSON serialization/deserialization
   */
  namespace ResponseTypes
  {
    ////////////////////////////////////////////////////////////////////////////////
    // Reflection helpers
    ////////////////////////////////////////////////////////////////////////////////
    
    /**
     * @enum CppType  Variable types, for reflection purposes
     */
    enum class CppType
    {
        UINT8,
        UINT16,
        UINT32,
        UINT64,
        BIGINT,
        BOOLEAN,
        C_STRING,
        STRING,
        OBJECT,
        BYTES
    };
    
    ////////////////////////////////////////////////////////////////////////////////
    // PROTOBUF OBJECTS
    ////////////////////////////////////////////////////////////////////////////////

    struct AccountMeta
    {
        char address[IOTEX_ADDRESS_STRLEN + 1];
        char balance[IOTEX_MAX_BALANCE_STRLEN + 1];
        std::string nonce;
        std::string pendingNonce;
        std::string numActions;
        bool isContract;
    };
    
    enum Encoding 
    {
      IOTEX_PROTOBUF = 0,
      ETHEREUM_RLP = 1
    };

    struct Transfer
    {
      char amount[IOTEX_MAX_BALANCE_STRLEN + 1];
      char recipient[IOTEX_ADDRESS_STRLEN + 1];
      std::string payload;
    };

    struct ActionCore_Transfer
    {
      uint32_t version;
      uint64_t nonce;
      uint64_t gasLimit;
      char gasPrice[IOTEX_MAX_BALANCE_STRLEN + 1];
      uint32_t chainId;
      Transfer transfer;
    };

    struct Action_Transfer
    {
      ActionCore_Transfer core;
      char senderPublicKey[IOTEX_PUBLIC_KEY_STRLEN + 1];
      char signature[IOTEX_SIGNATURE_STRLEN + 1];
      Encoding encoding;
    };

    struct ActionInfo_Transfer
    {
      Action_Transfer action;
      char actHash[IOTEX_HASH_STRLEN + 1];
      char blkHash[IOTEX_HASH_STRLEN + 1];
      char timestamp[IOTEX_TIMESTAMP_STRLEN + 1];
      std::string blkHeight;
      char sender[IOTEX_ADDRESS_STRLEN + 1];
      char gasFee[IOTEX_MAX_BALANCE_STRLEN + 1];
    };

    struct BlockIdentifier
    {
        char hash[IOTEX_HASH_STRLEN + 1];
        std::string height;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // RESPONSE MESSAGES
    ////////////////////////////////////////////////////////////////////////////////

    struct GetBalanceResponse
    {
      ResultCode fromJson(std::string jsonString);
      const char *getBalance() { return balance.c_str(); }

      private:
        std::string balance;
    };

    struct GetAccountResponse
    {
      public:
        ResultCode fromJson(std::string jsonString);

      public:
        AccountMeta accountMeta;
        BlockIdentifier blockIdentifier;
    };

    struct GetActionsResponse_Transfer
    {
      public:
        ResultCode fromJson(std::string jsonString);

      public:
        std::string total;
        std::vector<ActionInfo_Transfer> actionInfo;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // JSON Serialization helpers (Initial work, currently unused and not needed)
    ////////////////////////////////////////////////////////////////////////////////

    struct JsonProp
    {
      const CppType type;
      const std::string name;

      JsonProp(CppType type, std::string name) : type(type),
                                                  name(name), 
                                                  _initialized(false)
      {}

      /* Setters */
      void setValue(const char* value)
      {
        if(_initialized && type == CppType::C_STRING)
        {
          free(_value.c_string);
        }
        _value.c_string = (char*) malloc(strlen(value));
        memcpy(_value.c_string, value, strlen(value));
        _initialized = true;
      }

      void setValue(uint64_t value)
      {
        _initialized = true;
        _value.uint64 = value;
      }
      
      void setValue(bool value)
      {
        _initialized = true;
        _value.boolean = value;
      }

      /* Getters */
      const char* getValueCString()
      {
        return _initialized ? _value.c_string : nullptr;
      }

      uint64_t getValueUint64()
      {
        return _value.uint64;
      }
      
      bool getValueBool()
      {
        return _value.boolean;
      }
      
      const uint8_t* getValueBytes()
      {
        return _value.bytes;
      }

      ~JsonProp()
      {
        if(_initialized && isHeapAllocated())
        {
          free(_value.object);
        }
      }          

      uint32_t getBytesCount()
      {
        return _bytesCount;
      }

      private:
        bool _initialized;
        union
        {
          uint8_t uint8;
          uint16_t uint16;
          uint32_t uint32;
          uint64_t uint64;
          bool boolean;
          char *c_string;
          std::string *string;
          void *object;
          uint8_t *bytes;
        } _value;

        uint32_t _bytesCount; // bytes count, only used for bytes type

        bool isHeapAllocated()
        {
          if (
            type == CppType::C_STRING || 
            type == CppType::STRING || 
            type == CppType::OBJECT || 
            type == CppType::BYTES
            )
          {
            return true;
          }
          else
            return false;
        }
    };

    /**
     * @struct  StringJsonProp  Struct that represents a string json property using std::string
     */
    struct StringJsonProp : JsonProp
    {
      StringJsonProp(std::string name) : 
        JsonProp(CppType::C_STRING, name)
        {}
    };

    struct Uint8JsonProp : JsonProp
    {
      Uint8JsonProp(std::string name) : 
        JsonProp(CppType::UINT8, name)
        {}
    };

    struct Uint64JsonProp : JsonProp
    {
      Uint64JsonProp(std::string name) : 
        JsonProp(CppType::UINT64, name)
        {}
    };
    
    struct BytesJsonProp : JsonProp
    {
      BytesJsonProp(std::string name) : 
        JsonProp(CppType::BYTES, name)
        {}
    };

    struct BoolJsonProp : JsonProp
    {
      BoolJsonProp(std::string name): 
        JsonProp(CppType::BOOLEAN, name)
        {}
    };
    
    struct AccountMetaJsonObject
    {
      public:
        StringJsonProp address;
        StringJsonProp balance;
        Uint64JsonProp nonce;
        Uint64JsonProp pendingNonce;
        Uint64JsonProp numActions;
        BoolJsonProp isContract;
        BytesJsonProp contractByteCode;

      public:
        AccountMetaJsonObject() :
          address("address"),
          balance("balance"),
          nonce("nonce"),
          pendingNonce("pendingNonce"),
          numActions("numActions"),
          isContract("isContract"),
          contractByteCode("contractByteCode")
          {}

          const char*  getAddress() { return address.getValueCString(); }
          const char* getBalance() { return balance.getValueCString(); }
          uint64_t getNonce() { return nonce.getValueUint64(); }
          uint64_t getPendingNonce() { return pendingNonce.getValueUint64(); }
          uint64_t getNumActions() { return numActions.getValueUint64(); }
          bool getIsContract() { return isContract.getValueBool(); }
          const uint8_t* getContractByteCode() { return contractByteCode.getValueBytes(); }
          const uint32_t getContractByteCodeSize() { return contractByteCode.getBytesCount(); }
    };

    struct BlockIdentifierJsonObject
    {
      StringJsonProp hash;
      Uint64JsonProp height;

      BlockIdentifierJsonObject() :
        hash("hash"),
        height("height")
        {}
    };

  } // namespace ResponseTypes
}

#endif