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
					if (!Initialize(request))
					{
						return "";
					};

					_httpClient.GET();

					return _httpClient.getString().c_str();
				}

				std::string post(const char* request, const char *body) override 
				{
					if (!Initialize(request))
					{
						return "";
					};

					_httpClient.POST(body);
					std::string ret;
					ret = _httpClient.getString().c_str();
					return ret;
				}

				int get(const char* request, char* rspBuf, size_t size)
				{
					if (!Initialize(request))
					{
						return -1;
					};

					_httpClient.GET();
					memcpy(rspBuf, _httpClient.getString().c_str(), _httpClient.getString().length());
					return strlen(rspBuf);
				}

				int post(const char* request, const char* body, char* rspBuf, size_t size)
				{
					if (!Initialize(request))
					{
						return -1;
					};
					int errorCode = _httpClient.POST(body);
					memcpy(rspBuf, _httpClient.getString().c_str(), _httpClient.getString().length());
					return strlen(rspBuf);
				}
			
			private:
				HTTPClient _httpClient;
				#ifdef ESP8266
				WiFiClient _wifiClient;
				#endif

				bool Initialize(const char* path)
				{
					_httpClient.setReuse(true);
					_httpClient.setTimeout(3000);
					_httpClient.addHeader("Content-Type", "application/json");
					String url = "http://";
					url += path;
					bool initOk = false;
					#ifdef ESP8266
					initOk = _httpClient.begin(_wifiClient, url);
					#else
					initOk = _httpClient.begin(url);
					#endif
					if (!initOk)
					{
						Serial.println("Failed to initialize HTTP client");
					}
					return initOk;
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