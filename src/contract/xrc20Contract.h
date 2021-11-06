#ifndef IOTEX_XRC20_CONTRACT_H
#define IOTEX_XRC20_CONTRACT_H

#include "contract/contract.h"
#include "IoTeXConstants.h"

#define IOTEX_CONTRACT_ENCODED_FUNCTIONSELECTOR_SIZE 4
#define IOTEX_CONTRACT_ENCODED_TOTALSUPPLY_SIZE 36
#define IOTEX_CONTRACT_ENCODED_TRANSFER_SIZE 68

namespace iotex
{
class Xrc20Contract
{
  public:
	static void generateCallDataForTotalSupply(uint8_t data[4]);
	static void generateCallDataForBalanceOf(uint8_t address[ETH_ADDRESS_SIZE],
											 uint8_t data[IOTEX_CONTRACT_ENCODED_TOTALSUPPLY_SIZE]);
	static void generateCallDataForTransfer(uint8_t owner[ETH_ADDRESS_SIZE], uint64_t value,
											uint8_t data[IOTEX_CONTRACT_ENCODED_TRANSFER_SIZE]);
};
} // namespace iotex

#endif