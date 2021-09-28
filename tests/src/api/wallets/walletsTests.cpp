#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "secrets.h"

#include "api/wallet/wallets.h"
#include "connection/connection.h"

using namespace std;
using namespace testing;
using namespace iotex;
using namespace iotex::api;
using namespace iotex::ResponseTypes;

namespace 
{
    constexpr const char tTransactionHash[] = "e19dfb0c84799fc43217287d0d81369348279a0b3b32d0ad2f973ee5aaa392ae";
    constexpr const char tAccount[] = IOTEX_ADDRESS_2;
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
    ASSERT_EQ(balance, "1000000000000000000");
}

TEST_F(WalletTests, GetAccount)
{
    Connection<Api> connection(tIp, tPort, tBaseUrl);
    AccountMeta accountMeta;
    ResultCode result = connection.api.wallets.getAccount(tAccount, accountMeta);

    ASSERT_EQ(result, ResultCode::SUCCESS);
    ASSERT_STREQ(accountMeta.address, tAccount);
    ASSERT_STREQ(accountMeta.balance, "1000000000000000000");
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
    ASSERT_EQ(transaction.action.core.nonce, 19);
    ASSERT_EQ(transaction.action.core.gasLimit, 21000);
    ASSERT_STREQ(transaction.action.core.gasPrice, "1000000000000");
    ASSERT_STREQ(transaction.action.core.transfer.amount, "1000000000000000000");
    ASSERT_STREQ(transaction.action.core.transfer.recipient, "io1w3egd539q90jauqumvg09v3kv2hpqsy2mwym0q");
    ASSERT_EQ(transaction.action.core.transfer.payload, "");
    ASSERT_STREQ(transaction.action.senderPublicKey, "BBEUzF7ETUgLUaIoWXHUELMR1nYf3XLxSMKfIRsuJC1+oUuKtDptd4rIVc5lADqeEGl2Wme0YKQHyP22IPmEB5A=");
    ASSERT_STREQ(transaction.action.signature, "WC9/nl3gpnIvt4HdibVGG4gZrAolg+c1hFQ5sL5e22US92ymx9QZiiaEO3s2/fDCPamjuAxP2IF4dl7zs5XfqRs=");
    
    ASSERT_STREQ(transaction.actHash, "e19dfb0c84799fc43217287d0d81369348279a0b3b32d0ad2f973ee5aaa392ae");
    ASSERT_STREQ(transaction.blkHash, "81c03186a0374154a6c898e64a675d33784b2086006790afa0ebc2591be97d08");
    ASSERT_EQ(transaction.blkHeight, "13208023");
    ASSERT_STREQ(transaction.sender, "io1l5cqp670fmqgyfttux85e3y362jxnlmpskgeus");
    ASSERT_STREQ(transaction.gasFee, "10000000000000000");
    ASSERT_STREQ(transaction.timestamp, "2021-09-14T07:14:40Z");
   
}

