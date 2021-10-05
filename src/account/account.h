#ifndef IOTEX_ACCOUNT_H
#define IOTEX_ACCOUNT_H

#include <stdint.h>
#include <stddef.h>
#include "constants.h"
#include "protobuf/pb_api.h"
#include "random/random.h"
#include "encoder/encoder.h"
#include "signer/signer.h"
#include "connection/connection.h"

namespace iotex
{
    class Account
    {
        public:
            /**
             * \brief   Generates a new address
             */ 
            Account();

            /**
             * \brief   Creates an address from a given public key
             */ 
            Account(uint8_t privateKey[IOTEX_PRIVATE_KEY_SIZE]);

            // Getters
            void getIotexAddress(char buffer[IOTEX_ADDRESS_STRLEN+1]);
            void getEthereumAddress(char buffer[IOTEX_ADDRESS_STRLEN+1]);
            void getPublicKey(uint8_t buffer[IOTEX_ADDRESS_STRLEN+1]);
            void getPrivateKey(uint8_t buffer[IOTEX_ADDRESS_STRLEN+1]);

            // Signing
            void signMessage(const uint8_t *message, size_t size, uint8_t signature[IOTEX_SIGNATURE_SIZE]);
            void signTokenTransferAction(iotex::ResponseTypes::ActionCore_Transfer &transfer, uint8_t signature[IOTEX_SIGNATURE_SIZE]);
            void signExecutionAction(iotex::ResponseTypes::ActionCore_Execution &execution, uint8_t signature[IOTEX_SIGNATURE_SIZE], uint8_t hash[IOTEX_HASH_SIZE] = nullptr);

            // Action execution
            template<typename TAPI>
            ResultCode sendTokenTransferAction(Connection<TAPI>& conn, uint64_t nonce, uint64_t gasLimit, const char* gasPrice, const char* amount, const char* recipient, uint8_t hash[IOTEX_HASH_SIZE])
            {
                ResponseTypes::ActionCore_Transfer core;
                core.version = 1;
                core.gasLimit = gasLimit;
                core.nonce = nonce;
                strcpy(core.gasPrice, gasPrice);
                core.chainId = 0;
                strcpy(core.transfer.amount, amount);
                core.transfer.payload = "";
                strcpy(core.transfer.recipient, recipient);

                // Sign
                uint8_t signature[IOTEX_SIGNATURE_SIZE];
                signTokenTransferAction(core, signature);

                return conn.api.wallets.sendTokenTransfer(_publicKey, signature, core, hash);
            }

            template<typename TAPI>
            ResultCode sendExecutionAction(Connection<TAPI>& conn, uint64_t nonce, uint64_t gasLimit, const char* gasPrice, const char* amount, const char contract[IOTEX_ADDRESS_STRLEN + 1], IotexString data, uint8_t hash[IOTEX_HASH_SIZE])
            {
                ResponseTypes::ActionCore_Execution core;
                core.version = 1;
                core.gasLimit = gasLimit;
                core.nonce = nonce;
                strcpy(core.gasPrice, gasPrice);
                strcpy(core.execution.amount, amount);
                strcpy(core.execution.contract, contract);
                core.execution.data = data;

                // Sign
                uint8_t signature[IOTEX_SIGNATURE_SIZE];
                signExecutionAction(core, signature);
                
                return conn.api.wallets.sendExecution(_publicKey, signature, core, hash);
            }

        private:
            IotexString _iotexAddr;
            IotexString _ethAddr;
            uint8_t _publicKey[IOTEX_PUBLIC_KEY_SIZE];
            uint8_t _privateKey[IOTEX_PRIVATE_KEY_SIZE];
            
            // Dependencies
            Random* _pRandomGenerator;
            Encoder* _pEncoder;
            Signer* _pSigner;

        // Private methods
        private:
            void GenerateAddressesFromPrivateKey();
            void generateIotexAddress();
            void generateEthAddress();
            void setDepsFromGlobals();

        // Methods below should only be used for testing
        public:
            // Allow to inject mock dependencies in constructor
            Account(Random* pRandomGenerator, Encoder* pEncoder, Signer* pSigner, uint8_t privateKey[IOTEX_PRIVATE_KEY_SIZE] = nullptr);
    };
}

#endif