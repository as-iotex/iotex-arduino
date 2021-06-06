#ifndef RPC_H
#define RPC_H

#include <cstdio>
#include <map>
#include <string>

#include "host/host.h"
#include "protobuf/pb_api.h"

namespace Iotex 
{
	namespace api 
	{
		namespace rpc 
		{
			struct RpcCallData
			{
				std::string url;
				std::string body;
			};

			class Wallets
			{
				public:
				static RpcCallData getAccount(Host& host, const std::string address);
				static RpcCallData getBalance(Host& host, const std::string address);
				static RpcCallData getActionByHash(Host& host, const std::string hash, bool checkPending = false);
			};

		}  // namespace rpc
	}  // namespace api
}  // namespace Iotex

#endif
