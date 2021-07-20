
#include "api/wallet/wallets.h"
#include "signer/signer.h"

using namespace Iotex;
using namespace Iotex::api;
using namespace Iotex::ResponseTypes;

ResultCode Wallets::getAccount(const char *const address, AccountMeta& data)
{
	rpc::RpcCallData callData = rpc::Wallets::getAccount(this->host_, address);
	std::string rspBody;
	auto ret = http_->post(callData.url.c_str(), callData.body.c_str(), rspBody);
	if (ret != ResultCode::SUCCESS)
	{
		return ResultCode::ERROR_HTTP;
	}

	ResponseTypes::GetAccountResponse resp;
	ret = resp.fromJson(rspBody);
	if (ret != ResultCode::SUCCESS)
		return ret;

	data = resp.accountMeta;
	return ret;
}

ResultCode Wallets::getBalance(const char *const address, std::string& balance)
{
	rpc::RpcCallData callData = rpc::Wallets::getBalance(this->host_, address);
	std::string rspBody;
	auto ret = http_->post(callData.url.c_str(), callData.body.c_str(), rspBody);
	if (ret != ResultCode::SUCCESS)
	{
		return ret;
	}

	ResponseTypes::GetBalanceResponse response;
	ret = response.fromJson(rspBody);
	if (ret != ResultCode::SUCCESS)
		return ret;

	balance = std::string(response.getBalance());
	return ret;
}


ResultCode Wallets::getTransactionByHash(const char *const address, ResponseTypes::ActionInfo_Transfer& action)
{
	rpc::RpcCallData callData = rpc::Wallets::getActionByHash(this->host_, address);
	std::string rspBody;
	auto ret = http_->post(callData.url.c_str(), callData.body.c_str(), rspBody);
	if (ret != ResultCode::SUCCESS)
	{
		return ret;
	}
	ResponseTypes::GetActionsResponse_Transfer response;
	ret = response.fromJson(rspBody);
	if (ret != ResultCode::SUCCESS)
		return ret;

	action = response.actionInfo.at(0);
	return ret;
}

ResultCode Wallets::sendTokenTransfer(const uint8_t senderPubKey[IOTEX_PUBLIC_KEY_SIZE], const uint8_t signature[IOTEX_SIGNATURE_SIZE], const ResponseTypes::ActionCore_Transfer &transfer, uint8_t hash[IOTEX_HASH_SIZE])
{
	rpc::RpcCallData callData = rpc::Wallets::sendTokenTransfer(this->host_, transfer, senderPubKey, signature);
	std::string rspBody;
	ResultCode ret = http_->post(callData.url.c_str(), callData.body.c_str(), rspBody);
	if (ret != ResultCode::SUCCESS)
		return ret;

	ResponseTypes::SendTokenTransferResponse response;
	ret = response.fromJson(rspBody);
	if (ret != ResultCode::SUCCESS)
		return ret;

	signer.str2hex(response.hash, hash, IOTEX_HASH_SIZE);
	return ret;
}