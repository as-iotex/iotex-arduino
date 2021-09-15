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

namespace Iotex
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
            void getIotexAddress(char buffer[IOTEX_ADDRESS_STRLEN]);
            void getEthereumAddress(char buffer[IOTEX_ADDRESS_STRLEN]);
            void getPublicKey(uint8_t buffer[IOTEX_ADDRESS_STRLEN]);
            void getPrivateKey(uint8_t buffer[IOTEX_ADDRESS_STRLEN]);

            // Signing
            void signMessage(const uint8_t *message, size_t size, uint8_t signature[IOTEX_SIGNATURE_SIZE]);
            void signTokenTransferAction(Iotex::ResponseTypes::ActionCore_Transfer &transfer, uint8_t signature[IOTEX_SIGNATURE_SIZE]);

            // Action execution
            template<typename TAPI>
            ResultCode sendTokenTransferAction(Connection<TAPI>& conn, uint64_t nonce, uint64_t gasLimit, const char* gasPrice, const char* amount, const char* recipient, uint8_t hash[IOTEX_HASH_SIZE])
            {
                // TODO Get account meta instead of receiving nonce?

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