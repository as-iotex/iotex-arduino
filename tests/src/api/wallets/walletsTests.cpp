#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "api/wallet/wallets.h"
#include "connection/connection.h"

using namespace std;
using namespace testing;
using namespace Iotex;
using namespace Iotex::api;
using namespace Iotex::ResponseTypes;

namespace 
{
    constexpr const char tTransactionHash[] = "ac675a8440e6e30c3b9dcfb810b772e2ba9e2c1d281db0c1f4bee073b35f890c";
    constexpr const char tAccount[] = "io1xkx7y9ygsa3dlmvzzyvv8zm6hd6rmskh4dawyu";
    constexpr const char tIp[] = "gateway.iotexlab.io";
    constexpr const char tBaseUrl[] = "iotexapi.APIService";
    constexpr const int tPort = 10000;
}  // namespace

class WalletTests : public Test
{
    void SetUp() override {}
    
    void TearDown() override {}
};

TEST_F(WalletTests, GetBalance)
{
    Connection<Api> connection(tIp, tPort, tBaseUrl);
    std::string balance;
    ResultCode result = connection.api.wallets.getBalance(tAccount, balance);
    
    ASSERT_EQ(result, ResultCode::SUCCESS);
    ASSERT_EQ(balance, "888000000000000000000");
}

TEST_F(WalletTests, GetAccount)
{
    Connection<Api> connection(tIp, tPort, tBaseUrl);
    AccountMeta accountMeta;
    ResultCode result = connection.api.wallets.getAccount(tAccount, accountMeta);

    ASSERT_EQ(result, ResultCode::SUCCESS);
    ASSERT_STREQ(accountMeta.address, tAccount);
    ASSERT_STREQ(accountMeta.balance, "888000000000000000000");
    ASSERT_FALSE(accountMeta.isContract);
    ASSERT_EQ(accountMeta.nonce, "0");
    ASSERT_EQ(accountMeta.pendingNonce, "1");
    ASSERT_EQ(accountMeta.numActions, "1");
}

TEST_F(WalletTests, GetActionByHash)
{
    Connection<Api> connection(tIp, tPort, tBaseUrl);
    ActionInfo_Transfer transaction;
    ResultCode result = connection.api.wallets.getTransactionByHash(tTransactionHash, transaction);

    ASSERT_EQ(result, ResultCode::SUCCESS);
    ASSERT_EQ(transaction.action.core.version, 0);
    ASSERT_EQ(transaction.action.core.nonce, 1);
    ASSERT_EQ(transaction.action.core.gasLimit, 21000);
    ASSERT_STREQ(transaction.action.core.gasPrice, "1000000000000");
    ASSERT_STREQ(transaction.action.core.transfer.amount, "888000000000000000000");
    ASSERT_STREQ(transaction.action.core.transfer.recipient, "io1xkx7y9ygsa3dlmvzzyvv8zm6hd6rmskh4dawyu");
    ASSERT_EQ(transaction.action.core.transfer.payload, "");
    ASSERT_STREQ(transaction.action.senderPublicKey, "BBEUzF7ETUgLUaIoWXHUELMR1nYf3XLxSMKfIRsuJC1+oUuKtDptd4rIVc5lADqeEGl2Wme0YKQHyP22IPmEB5A=");
    ASSERT_STREQ(transaction.action.signature, "mbWTQyW19tGwOv1qXr/0FZAiiX3xwbeklAnopz5qz8l5Wp+zYqxThQH3LBtWqFsRZ8PfmCouuCIgZEaZYOhiaBw=");
    
    ASSERT_STREQ(transaction.actHash, "ac675a8440e6e30c3b9dcfb810b772e2ba9e2c1d281db0c1f4bee073b35f890c");
    ASSERT_STREQ(transaction.blkHash, "6efbb4bdaa0ce4c8b45417fbf87d545a3c0c174d432b8d1005d588d89a98faf5");
    ASSERT_EQ(transaction.blkHeight, "8755936");
    ASSERT_STREQ(transaction.sender, "io1l5cqp670fmqgyfttux85e3y362jxnlmpskgeus");
    ASSERT_STREQ(transaction.gasFee, "10000000000000000");
    ASSERT_STREQ(transaction.timestamp, "2021-05-15T16:39:45Z");
   
}

