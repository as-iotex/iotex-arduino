
#include "api/wallet/wallets.h"
#include "helpers/client_helpers.h"
#include "signer/signer.h"

using namespace iotex;
using namespace iotex::api;
using namespace iotex::ResponseTypes;

ResultCode Wallets::getAccount(const char* const address, AccountMeta& data)
{
	IOTEX_DEBUG_F("Wallets::getAccount(): Address %s\r\n", address);
	rpc::RpcCallData callData = rpc::Wallets::getAccount(this->host_, address);
	IotexString rspBody;
	IOTEX_DEBUG_F("Wallets::getAccount(): Post\r\n");
	auto ret = http_->post(callData.url.c_str(), callData.body.c_str(), rspBody);
	if(ret != ResultCode::SUCCESS)
	{
		return ResultCode::ERROR_HTTP;
	}

	ResponseTypes::GetAccountResponse resp;
	IOTEX_DEBUG_F("Wallets::getAccount(): FromJson\r\n");
	ret = resp.fromJson(rspBody);
	if(ret != ResultCode::SUCCESS)
		return ret;

	data = resp.accountMeta;
	return ret;
}

ResultCode Wallets::getBalance(const char* const address, IotexString& balance)
{
	rpc::RpcCallData callData = rpc::Wallets::getBalance(this->host_, address);
	IotexString rspBody;
	auto ret = http_->post(callData.url.c_str(), callData.body.c_str(), rspBody);
	if(ret != ResultCode::SUCCESS)
	{
		return ret;
	}

	ResponseTypes::GetBalanceResponse response;
	ret = response.fromJson(rspBody);
	if(ret != ResultCode::SUCCESS)
		return ret;

	balance = IotexString(response.getBalance());
	return ret;
}

ResultCode Wallets::getTransactionByHash(const char* const address,
										 ResponseTypes::ActionInfo_Transfer& action)
{
	rpc::RpcCallData callData = rpc::Wallets::getActionByHash(this->host_, address);
	IotexString rspBody;
	auto ret = http_->post(callData.url.c_str(), callData.body.c_str(), rspBody);
	if(ret != ResultCode::SUCCESS)
	{
		return ret;
	}
	ResponseTypes::GetActionResponse_Transfer response;
	ret = response.fromJson(rspBody);
	if(ret != ResultCode::SUCCESS)
		return ret;

	action = response.actionInfo;
	return ret;
}

ResultCode Wallets::getExecutionByHash(const char* const address,
									   ResponseTypes::ActionInfo_Execution& action)
{
	rpc::RpcCallData callData = rpc::Wallets::getActionByHash(this->host_, address);
	IotexString rspBody;
	auto ret = http_->post(callData.url.c_str(), callData.body.c_str(), rspBody);
	if(ret != ResultCode::SUCCESS)
	{
		return ret;
	}
	ResponseTypes::GetActionResponse_Execution response;
	ret = response.fromJson(rspBody);
	if(ret != ResultCode::SUCCESS)
		return ret;

	action = response.actionInfo;
	return ret;
}

ResultCode Wallets::sendTokenTransfer(const uint8_t senderPubKey[IOTEX_PUBLIC_KEY_SIZE],
									  const uint8_t signature[IOTEX_SIGNATURE_SIZE],
									  const ResponseTypes::ActionCore_Transfer& transfer,
									  uint8_t hash[IOTEX_HASH_SIZE])
{
	rpc::RpcCallData callData =
		rpc::Wallets::sendTokenTransfer(this->host_, transfer, senderPubKey, signature);
	IotexString rspBody;
	ResultCode ret = http_->post(callData.url.c_str(), callData.body.c_str(), rspBody);
	if(ret != ResultCode::SUCCESS)
		return ret;

	ResponseTypes::SendExecutionResponse response;
	ret = response.fromJson(rspBody);
	if(ret != ResultCode::SUCCESS)
		return ret;

	signer.str2hex(response.hash, hash, IOTEX_HASH_SIZE);
	return ret;
}

ResultCode Wallets::sendExecution(const uint8_t senderPubKey[IOTEX_PUBLIC_KEY_SIZE],
								  const uint8_t signature[IOTEX_SIGNATURE_SIZE],
								  const ResponseTypes::ActionCore_Execution& execution,
								  uint8_t hash[IOTEX_HASH_SIZE])
{
	rpc::RpcCallData callData =
		rpc::Wallets::sendExecution(this->host_, execution, senderPubKey, signature);
	IotexString rspBody;

	IOTEX_DEBUG_F("Wallets::sendExecution: %s\n", callData.body.c_str());

	ResultCode ret = http_->post(callData.url.c_str(), callData.body.c_str(), rspBody);
	if(ret != ResultCode::SUCCESS)
		return ret;

	ResponseTypes::SendExecutionResponse response;
	ret = response.fromJson(rspBody);
	if(ret != ResultCode::SUCCESS)
		return ret;

	signer.str2hex(response.hash, hash, IOTEX_HASH_SIZE);

	IOTEX_DEBUG_F("Action hash: %s\n", response.hash);

	return ret;
}