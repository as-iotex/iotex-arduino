#include "api/rpc.h"

#include <iostream>
#include "encoder/encoder.h"
#include <numeric>
#include <string>

using namespace Iotex::api::rpc;

namespace 
{
  constexpr const uint8_t URL_MAX_LEN = 128U;
}

////////////////////////////////////////////////////////////////////////////////
//Wallets
////////////////////////////////////////////////////////////////////////////////

RpcCallData Wallets::getAccount(Host& host, const std::string address)
{
	RpcCallData ret;
	// Url
	ret.url.reserve(URL_MAX_LEN);
	ret.url += host.toString().c_str();
	ret.url += "GetAccount";

	// Body
	ret.body = "";
	ret.body += R"({"address":")";
	ret.body += address;
	ret.body += R"("})";

	return ret;
}

RpcCallData Wallets::getBalance(Host& host, const std::string address)
{
 	return getAccount(host, address);
}

RpcCallData Wallets::getActionByHash(Host& host, const std::string hash, bool checkPending)
{
	RpcCallData ret;
	// Url
	ret.url.reserve(URL_MAX_LEN);
	ret.url += host.toString().c_str();
	ret.url += "GetActions";

	// Body
	ret.body = "";
	ret.body += R"({"byHash": {"actionHash": ")";
	ret.body += hash;
	ret.body += R"(","checkPending": )";
	ret.body += checkPending ? "true" : "false";
	ret.body += R"(}})";

	return ret;
}


RpcCallData Wallets::sendTokenTransfer(Host& host, const ResponseTypes::ActionCore_Transfer transfer, const uint8_t senderPubKey[IOTEX_PUBLIC_KEY_SIZE], const uint8_t signature[IOTEX_SIGNATURE_SIZE])
{
	RpcCallData ret;
	// Base64 encode signature and sender public key
    char base64Signature[IOTEX_SIGNATURE_SIZE * 2] = {0}; // Double the size is enough for a base64 encoded message
    char base64PublicKey[IOTEX_PUBLIC_KEY_SIZE * 2] = {0}; // Double the size is enough for a base64 encoded message
	encoder.base64_encode(signature, IOTEX_SIGNATURE_SIZE, base64Signature);
	encoder.base64_encode(senderPubKey, IOTEX_PUBLIC_KEY_SIZE, base64PublicKey);

	// Url
	ret.url.reserve(URL_MAX_LEN);
	ret.url += host.toString().c_str();
	ret.url += "SendAction";

	// Body
	char body[1024] = {0}; 	// TODO: Improve max size. 1024 should be enough for most transfers for now

	sprintf(body + strlen(body), R"({"action": {"core": )");
	sprintf(body + strlen(body), R"({"version": )");
	sprintf(body + strlen(body), "%d", transfer.version);
	sprintf(body + strlen(body), R"(,"nonce": ")");
	sprintf(body + strlen(body), "%llu", transfer.nonce);
	sprintf(body + strlen(body), R"(","gasLimit": ")");
	sprintf(body + strlen(body), "%llu", transfer.gasLimit);
	sprintf(body + strlen(body), R"(","gasPrice": ")");
	sprintf(body + strlen(body), "%s", transfer.gasPrice);
	sprintf(body + strlen(body), R"(")");
	sprintf(body + strlen(body), R"(,"chainID": )");
	sprintf(body + strlen(body), "%d", transfer.chainId);
	sprintf(body + strlen(body), R"(,"transfer":)");
	sprintf(body + strlen(body), R"({"amount": ")");
	sprintf(body + strlen(body), "%s", transfer.transfer.amount);
	sprintf(body + strlen(body), R"(")");
	sprintf(body + strlen(body), R"(,"recipient": ")");
	sprintf(body + strlen(body), "%s", transfer.transfer.recipient);
	sprintf(body + strlen(body), R"(","payload": ")");
	sprintf(body + strlen(body), R"("}})");
	sprintf(body + strlen(body), R"(,"senderPubKey": ")");
	sprintf(body + strlen(body), "%s", base64PublicKey);
	sprintf(body + strlen(body), R"(")");
	sprintf(body + strlen(body), R"(,"signature": ")");
	sprintf(body + strlen(body), "%s", base64Signature);
	sprintf(body + strlen(body), R"(","encoding": "IOTEX_PROTOBUF"}})");

	ret.body = body;
	return ret;
}