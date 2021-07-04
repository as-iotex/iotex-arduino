
#include "api/wallet/wallets.h"

using namespace Iotex;
using namespace Iotex::api;
using namespace Iotex::ResponseTypes;

ResultCode Wallets::getAccount(const char *const address, AccountMeta& data)
{
	rpc::RpcCallData callData = rpc::Wallets::getAccount(this->host_, address);
	auto rsp = http_->post(callData.url.c_str(), callData.body.c_str());
	if (rsp == "")
	{
		return ResultCode::ERROR_HTTP;
	}

	ResponseTypes::GetAccountResponse resp;
	ResultCode res = resp.fromJson(rsp);
	if (res != ResultCode::SUCCESS)
		return res;

	data = resp.accountMeta;
	return ResultCode::SUCCESS;
}

ResultCode Wallets::getBalance(const char *const address, std::string& balance)
{
	rpc::RpcCallData callData = rpc::Wallets::getBalance(this->host_, address);
	auto rawRsp = http_->post(callData.url.c_str(), callData.body.c_str());
	if (rawRsp == "")
	{
		return ResultCode::ERROR_HTTP;
	}

	ResponseTypes::GetBalanceResponse response;
	ResultCode result = response.fromJson(rawRsp);
	if (result != ResultCode::SUCCESS)
		return result;

	balance = std::string(response.getBalance());
	return ResultCode::SUCCESS;
}


ResultCode Wallets::getTransactionByHash(const char *const address, ResponseTypes::ActionInfo_Transfer& action)
{
	rpc::RpcCallData callData = rpc::Wallets::getActionByHash(this->host_, address);
	auto rawRsp = http_->post(callData.url.c_str(), callData.body.c_str());
	if (rawRsp == "")
	{
		return ResultCode::ERROR_HTTP;
	}
	ResponseTypes::GetActionsResponse_Transfer response;
	ResultCode result = response.fromJson(rawRsp);
	if (result != ResultCode::SUCCESS)
		return result;

	action = response.actionInfo.at(0);
	return ResultCode::SUCCESS;
}
