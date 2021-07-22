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
				static RpcCallData sendTokenTransfer(Host& host, const ResponseTypes::ActionCore_Transfer transfer, const uint8_t senderPubKey[IOTEX_PUBLIC_KEY_SIZE], const uint8_t signature[IOTEX_SIGNATURE_SIZE]);
			};

		}  // namespace rpc
	}  // namespace api
}  // namespace Iotex

#endif
