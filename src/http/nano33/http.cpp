#ifdef __SAMD21G18A__
#include "http/http.h"

#include <SPI.h>
#include <WiFiNINA.h>

namespace iotex
{
    namespace
    {
        class PlatformHTTP : public AbstractHTTP
        {
            public:
                IotexString get(const char* request) override 
                {
                    // Unimplemented
                    return "";
                }

                iotex::ResultCode post(const char* request, const char *body, IotexString& response) override
                {
                    const char * headerKeys[] = {"grpc-status", "grpc-message"} ;
                    const size_t numberOfHeaders = 2;

                    String rq = request;
                    if (!initialize(rq))
                    {
                        return ResultCode::ERROR_HTTP;
                    };
                    
                    client.print(F("POST "));
                    client.print(path);    
                    client.print(F(" HTTP/1.1\r\n"));
                    client.print(F("Host: "));
                    client.print(host);
                    client.print(":");
                    client.print(port);
                    client.print(F("\r\n"));
                    client.print(F("Content-Type: application/json\r\n"));
                    client.print(F("Connection: close\r\n"));
                    client.print(F("Content-Length: "));
                    client.print(strlen(body));
                    client.print(F("\r\n\r\n"));
                    client.print(body);

                    Serial.print(F("POST "));
                    Serial.print(path);    
                    Serial.print(F(" HTTP/1.1\r\n"));
                    Serial.print(F("Host: "));
                    Serial.print(host);
                    Serial.print(":");
                    Serial.print(port);
                    Serial.print(F("\r\n"));
                    Serial.print(F("Content-Type: application/json\r\n"));
                    Serial.print(F("Connection: close\r\n"));
                    Serial.print(F("Content-Length: "));
                    Serial.print(strlen(body));
                    Serial.print(F("\r\n\r\n"));
                    Serial.print(body);
                    Serial.println();
                    
                    response = getServerResponse();
                    if (response.length() > 0 && response.indexOf('{') != -1)
                    {
                        response = response.substring(response.indexOf('{'));
                        return ResultCode::SUCCESS;
                    }
                    else return ResultCode::ERROR_HTTP;
                }

            private:
                WiFiClient client;

                // Variables stored for current request
                String host;
                int port;
                String path;

                bool initialize(String& uri)
                {
                    // Get the host name or ip
                    if(uri.indexOf('/') != -1)
                    {
                        host = uri.substring(0, uri.indexOf('/'));
                    }
                    else
                    {
                        host = uri;
                    }

                    port = 80;  // Default to 80
                    if (!strncmp(uri.c_str(), "https", sizeof("https"))) port = 443;   // Default to 443 if https

                    if (host.indexOf(':') != -1)
                    {
                        port = atoi(host.substring(host.indexOf(':')+1).c_str());
                        host = host.substring(0, host.indexOf(':'));
                    }

                    path = uri.substring(uri.indexOf('/'));

                    // Connect
                    if (!client.connect(host.c_str(), port))
                    {
                        char dbgBuf[256] = {0};
                        sprintf(dbgBuf, "Failed to connect to http server\n Host: %s Port: %d Path: %s\n", host.c_str(), port, path.c_str());
                        IOTEX_DEBUG(dbgBuf);
                        return false;
                    }

                    return true;
                }

                bool waitForResponse()
                {
                    int timeout = 10000;
                    int elapsed = 0;
                    while (!client.available() && elapsed < timeout) 
                    {
                        elapsed++;
                        delay(1);
                        if (elapsed >= timeout)
                        {
                            IOTEX_DEBUG("waitForResponse(): Error, max timeout reached\n");
                            return false;
                        }
                    }
                    return client.available();
                }


                String getServerResponse()
                {
                    if (!waitForResponse())
                    {
                        IOTEX_DEBUG("getServerResponse(): No response received");
                        return "";
                    }

                    String response = "";
                    int i = 0;
                    if (client.available() > 0) 
                    {
                        while (client.available()) 
                        {
                            char c = client.read();
                            response.concat(c);                           
                        }
                    }

                    /* Disconnecting the client */
                    client.stop();

                    IOTEX_DEBUG("getServerResponse(): Response:");
                    IOTEX_DEBUG(response);

                    return response;
                }

        };
    }

    /**
     * HTTP object factory
     **/
    std::unique_ptr<IHTTP> makeHTTP()
    {
        return std::unique_ptr<IHTTP>(new PlatformHTTP());
    }

}
#endif // #ifdef __SAMD21G18A__