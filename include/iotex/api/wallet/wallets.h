#ifndef WALLETS_H
#define WALLETS_H

#include "api/base.h"
#include "api/rpc.h"
#include "protobuf/pb_api.h"

#include <map>
#include <string>

namespace Iotex
{
    namespace api
    {
        class IWallets : public Base
        {
            public:
                virtual ~IWallets() {}

                virtual ResultCode getAccount(const char *const address, ResponseTypes::AccountMeta& data) = 0;

                virtual ResultCode getBalance(const char *const address, std::string& balance) = 0;

                virtual ResultCode getTransactionByHash(const char *const address, ResponseTypes::ActionInfo_Transfer& action) = 0;

            protected:
                IWallets(Host &host, IHTTP &http) : Base(host, http) {}
        };  // class IWallets

        class Wallets : public IWallets 
        {
            public:
                Wallets(Host &host, IHTTP &http) : IWallets(host, http) {}

                virtual ResultCode getAccount(const char *const address, ResponseTypes::AccountMeta& data) override;

                virtual ResultCode getBalance(const char *const address, std::string& balance) override;

                virtual ResultCode getTransactionByHash(const char *const address, ResponseTypes::ActionInfo_Transfer& action) override;
        };  // class Wallets : public IWallets

    }  // namespace api
}  // namespace Iotex

#endif
