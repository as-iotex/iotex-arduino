#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "abi/abi.h"
#include "contract/contract.h"
#include "signer/signer.h"

using namespace std;
using namespace testing;
using namespace Iotex;
using namespace Iotex::abi;

class ContractTests : public Test
{
    void SetUp() override {}
    
    void TearDown() override {}
};

TEST_F(ContractTests, generateBytesForUint)
{
    uint8_t buf[32] = {0};
    char strBuf[65] = {0};
    // 8 bit
    uint8_t value8 = 0xF0;
    int32_t ret = Contract::generateBytesForUint(&value8, 1, buf);
    signer.hex2str(buf, sizeof(buf), strBuf, sizeof(strBuf));
    ASSERT_STREQ("00000000000000000000000000000000000000000000000000000000000000f0", strBuf);
    // 16 bit
    uint16_t value16 = 0x1234;
    ret = Contract::generateBytesForUint((uint8_t*)&value16, 2, buf);
    signer.hex2str(buf, sizeof(buf), strBuf, sizeof(strBuf));
    ASSERT_STREQ("0000000000000000000000000000000000000000000000000000000000001234", strBuf);
    // 32 bit
    uint32_t value32 = 0x12345678;
    ret = Contract::generateBytesForUint((uint8_t*)&value32, 4, buf);
    signer.hex2str(buf, sizeof(buf), strBuf, sizeof(strBuf));
    ASSERT_STREQ("0000000000000000000000000000000000000000000000000000000012345678", strBuf);
    // 64 bit
    uint64_t value64 = 2459565876494606882;
    ret = Contract::generateBytesForUint((uint8_t*)&value64, 8, buf);
    signer.hex2str(buf, sizeof(buf), strBuf, sizeof(strBuf));
    ASSERT_STREQ("0000000000000000000000000000000000000000000000002222222222222222", strBuf);   
}

TEST_F(ContractTests, generateBytesForInt)
{
    uint8_t buf[32] = {0};
    char strBuf[65] = {0};
    // 8 bit
    int8_t value8 = -1;
    int32_t ret = Contract::generateBytesForInt(&value8, 1, buf);
    signer.hex2str(buf, sizeof(buf), strBuf, sizeof(strBuf));
    ASSERT_STREQ("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", strBuf);
    value8 = -50;
    ret = Contract::generateBytesForInt(&value8, 1, buf);
    signer.hex2str(buf, sizeof(buf), strBuf, sizeof(strBuf));
    ASSERT_STREQ("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffce", strBuf);
    // 16 bit
    int16_t value16 = -5000;
    ret = Contract::generateBytesForInt((int8_t*)&value16, 2, buf);
    signer.hex2str(buf, sizeof(buf), strBuf, sizeof(strBuf));
    ASSERT_STREQ("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffec78", strBuf);   
    value16 = 5000;
    ret = Contract::generateBytesForInt((int8_t*)&value16, 2, buf);
    signer.hex2str(buf, sizeof(buf), strBuf, sizeof(strBuf));
    ASSERT_STREQ("0000000000000000000000000000000000000000000000000000000000001388", strBuf); 
    // 32 bit
    int32_t value32 = -572662306;
    ret = Contract::generateBytesForInt((int8_t*)&value32, 4, buf);
    signer.hex2str(buf, sizeof(buf), strBuf, sizeof(strBuf));
    ASSERT_STREQ("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffddddddde", strBuf);   
    value32 = 572662306;
    ret = Contract::generateBytesForInt((int8_t*)&value32, 4, buf);
    signer.hex2str(buf, sizeof(buf), strBuf, sizeof(strBuf));
    ASSERT_STREQ("0000000000000000000000000000000000000000000000000000000022222222", strBuf); 
    // 64 bit
    int64_t value64 = 2459565876494606882;
    ret = Contract::generateBytesForInt((int8_t*)&value64, 8, buf);
    signer.hex2str(buf, sizeof(buf), strBuf, sizeof(strBuf));
    ASSERT_STREQ("0000000000000000000000000000000000000000000000002222222222222222", strBuf);   
    value64 = -2459565876494606882;
    ret = Contract::generateBytesForInt((int8_t*)&value64, 8, buf);
    signer.hex2str(buf, sizeof(buf), strBuf, sizeof(strBuf));
    ASSERT_STREQ("ffffffffffffffffffffffffffffffffffffffffffffffffddddddddddddddde", strBuf); 
}

TEST_F(ContractTests, generateBytesForAddress)
{
    char addrStr[] = "89205a3a3b2a69de6dbf7f01ed13b2108b2c43e7";
    std::string expected = "000000000000000000000000" + std::string(addrStr);
    uint8_t in[20];
    signer.str2hex(addrStr, in, 20);
    uint8_t buf[32] = {0};
    char strBuf[65] = {0};
    int32_t ret = Contract::generateBytesForAddress(in, buf);
    signer.hex2str(buf, sizeof(buf), strBuf, sizeof(strBuf));
    ASSERT_STREQ(expected.c_str(), strBuf);
}

TEST_F(ContractTests, generateBytesForBool)
{
    // False
    uint8_t buf[32] = {0};
    char strBuf[65] = {0};
    Contract::generateBytesForBool(false, buf);
    signer.hex2str(buf, sizeof(buf), strBuf, sizeof(strBuf));
    ASSERT_STREQ("0000000000000000000000000000000000000000000000000000000000000000", strBuf);
    // False
    Contract::generateBytesForBool(true, buf);
    signer.hex2str(buf, sizeof(buf), strBuf, sizeof(strBuf));
    ASSERT_STREQ("0000000000000000000000000000000000000000000000000000000000000001", strBuf);
}

TEST_F(ContractTests, generateBytesForStaticBytes)
{
    uint8_t val[3] = {1,2,3};
    uint8_t buf[32] = {0xFF};
    char strBuf[65] = {0};
    Contract::generateBytesForStaticBytes(val, 3, buf);
    signer.hex2str(buf, sizeof(buf), strBuf, sizeof(strBuf));
    ASSERT_STREQ("0102030000000000000000000000000000000000000000000000000000000000", strBuf);
}

TEST_F(ContractTests, generateBytesForBytes)
{
    uint8_t val[3] = {1,2,3};
    uint8_t buf[128] = {0xFF};
    char strBuf[sizeof(buf) * 2 + 1] = {0};
    size_t encoded = Contract::generateBytesForBytes(val, 3, buf);
    signer.hex2str(buf, encoded, strBuf, sizeof(strBuf));
    ASSERT_EQ(64, encoded);
    ASSERT_STREQ("00000000000000000000000000000000000000000000000000000000000000030102030000000000000000000000000000000000000000000000000000000000", strBuf);

    uint8_t val2[] = {0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 0x77, 0x6f, 0x72, 0x6c, 0x64, 0x21};
    encoded = Contract::generateBytesForBytes(val2, sizeof(val2), buf);
    signer.hex2str(buf, encoded, strBuf, sizeof(strBuf));
    ASSERT_EQ(64, encoded);
    ASSERT_STREQ("000000000000000000000000000000000000000000000000000000000000000d48656c6c6f2c20776f726c642100000000000000000000000000000000000000", strBuf);

    uint8_t val3[40] = {0};
    memset(val3, 0xff, sizeof(val3));
    encoded = Contract::generateBytesForBytes(val3, sizeof(val3), buf);
    signer.hex2str(buf, encoded, strBuf, sizeof(strBuf));
    ASSERT_EQ(96, encoded);
    ASSERT_STREQ("0000000000000000000000000000000000000000000000000000000000000028ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff000000000000000000000000000000000000000000000000", strBuf);
}

TEST_F(ContractTests, generateBytesForString)
{
    char val[] = "Hello, world!";
    uint8_t buf[64] = {0xFF};
    size_t encoded = Contract::generateBytesForString(val, strlen(val), buf);
    char strBuf[sizeof(buf) * 2 + 1] = {0};
    signer.hex2str(buf, sizeof(buf), strBuf, sizeof(strBuf));
    ASSERT_EQ(64, encoded);
    ASSERT_STREQ("000000000000000000000000000000000000000000000000000000000000000d48656c6c6f2c20776f726c642100000000000000000000000000000000000000", strBuf);
}

TEST_F(ContractTests, generateBytesForLongString)
{
    char val[] = "Hello, world!00000000000000000000000000000000";
    uint8_t buf[96] = {0xFF};
    size_t encoded = Contract::generateBytesForString(val, strlen(val), buf);
    char strBuf[sizeof(buf) * 2 + 1] = {0};
    signer.hex2str(buf, sizeof(buf), strBuf, sizeof(strBuf));
    ASSERT_EQ(96, encoded);
    ASSERT_STREQ("000000000000000000000000000000000000000000000000000000000000002d48656c6c6f2c20776f726c6421303030303030303030303030303030303030303030303030303030303030303000000000000000000000000000000000000000", strBuf);
}




// generateFunctionSelector

TEST_F(ContractTests, generateFunctionSelector_Uint32AndBool)
{
    FunctionAbi function;
    function.name = "baz";
    InputOutputAbi input;
    input.name = "inputName";
    input.size_bytes = 4;
    input.type = EthereumTypeName::UINT;
    function.inputs.push_back(input);
    InputOutputAbi input2;
    input2.name = "inputName";
    input2.type = EthereumTypeName::BOOL;
    function.inputs.push_back(input2);

    std::vector<uint8_t> out;
    Contract::generateFunctionSelector(function, out);

    char strBuf[out.size() * 2 + 1];
    memset(strBuf, 0, sizeof(strBuf));
    signer.hex2str(&out[0], out.size(), strBuf, sizeof(strBuf));

    ASSERT_STREQ("cdcd77c0", strBuf);
}





// generateCallData
TEST_F(ContractTests, generateCallData_Uint32AndBool)
{
    FunctionAbi function;
    function.name = "baz";
    InputOutputAbi input;
    input.name = "input1";
    input.size_bytes = 4;
    input.type = EthereumTypeName::UINT;
    function.inputs.push_back(input);
    InputOutputAbi input2;
    input2.name = "input2";
    input2.type = EthereumTypeName::BOOL;
    function.inputs.push_back(input2);
    
    std::vector<Iotex::abi::FunctionAbi> functions;
    functions.push_back(function);

    Contract contract(functions);
    ParameterValue param1;
    // param1.size = 32;
    param1.value.uint32 = 69;
    param1.size = 4;
    ParameterValue param2;
    param2.value.boolean = true;

    ParameterValuesDictionary params;
    params.emplace(std::make_pair<const IotexString, const Iotex::abi::ParameterValue>(std::string("input1"), (const ParameterValue)param1));
    params.emplace(std::make_pair<const IotexString, const Iotex::abi::ParameterValue>(std::string("input2"), (const ParameterValue)param2));
    std::string out;
    contract.generateCallData(function.name, params, out);

    std::string expected = "";
    expected += "cdcd77c0";     // Function selector
    expected += "0000000000000000000000000000000000000000000000000000000000000045";     // Param 1
    expected += "0000000000000000000000000000000000000000000000000000000000000001";     // Param 2
    ASSERT_STREQ(expected.c_str(), out.c_str());
}
