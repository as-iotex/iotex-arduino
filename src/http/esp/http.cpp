#include "http/http.h"

#include <memory>
#include <string>

// Conditionally include the required HTTP header
#ifdef ESP8266
	#include <ESP8266HTTPClient.h>
#else
	#include <HTTPClient.h>
#endif

#include "helpers/client_helpers.h"

namespace Iotex 
{
	namespace 
	{
		class PlatformHTTP : public AbstractHTTP 
		{
			public:
				PlatformHTTP() = default;

				std::string get(const char* request) override 
				{
					HTTPClient httpClient;
					httpClient.setReuse(false);
					httpClient.setTimeout(3000);

					httpClient.addHeader("Content-Type", "application/json");
					httpClient.begin(std::string("http://" + std::string(request)).c_str());

					httpClient.GET();

					return httpClient.getString().c_str();
				}

				std::string post(const char* request, const char *body) override 
				{
					HTTPClient httpClient;
					httpClient.setReuse(true);
					httpClient.setTimeout(3000);

					httpClient.addHeader("Content-Type", "application/json");
					httpClient.begin(std::string("http://" + std::string(request)).c_str());

					httpClient.POST(body);
					return httpClient.getString().c_str();
				}
		}; // class PlatformHTTP : public AbstractHTTP
	}  // namespace


	/**
	 * HTTP object factory
	 **/
	std::unique_ptr<IHTTP> makeHTTP()
	{
		return std::unique_ptr<IHTTP>(new PlatformHTTP());
	}

}  // namespace Iotex