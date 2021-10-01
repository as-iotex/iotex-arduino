#ifndef RPC_H
#define RPC_H

#include <cstdio>
#include <map>
#include <string>

#include "host/host.h"
#include "protobuf/pb_api.h"

namespace iotex 
{
	namespace api 
	{
		namespace rpc 
		{
			struct RpcCallData
			{
				IotexString url;
				IotexString body;
			};

			class Wallets
			{
				public:
				static RpcCallData getAccount(Host& host, const IotexString address);
				static RpcCallData getBalance(Host& host, const IotexString address);
				static RpcCallData getActionByHash(Host& host, const IotexString hash, bool checkPending = false);
				static RpcCallData sendTokenTransfer(Host& host, const ResponseTypes::ActionCore_Transfer transfer, const uint8_t senderPubKey[IOTEX_PUBLIC_KEY_SIZE], const uint8_t signature[IOTEX_SIGNATURE_SIZE]);
				static RpcCallData sendExecution(Host& host, const ResponseTypes::ActionCore_Execution execution, const uint8_t senderPubKey[IOTEX_PUBLIC_KEY_SIZE], const uint8_t signature[IOTEX_SIGNATURE_SIZE]);
			};

		}  // namespace rpc
	}  // namespace api
}  // namespace iotex

#endif
