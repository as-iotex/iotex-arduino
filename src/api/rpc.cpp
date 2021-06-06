#include "api/rpc.h"

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

