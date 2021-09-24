

#ifndef API_H
#define API_H

#include "api/abstract.h"
#include "api/wallet/wallets.h"

namespace Iotex
{
	class Api : public api::Abstract 
	{
		public:
			api::Wallets wallets;

		Api() : Abstract(),
			wallets(_host, *_http) {}
	};	// class Api
	
}  // namespace Iotex

#endif
