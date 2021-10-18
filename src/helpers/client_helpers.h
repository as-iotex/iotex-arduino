/**
 * Conditionally include Arduino or Linux headers 
 */ 

#ifndef CLIENT_HELPERS_H
#define CLIENT_HELPERS_H

#include <cstring>
#include <string>
#include <vector>

#if defined(ARDUINO) || defined(ESP8266) || defined(ESP32) || defined(__SAMD21G18A__)

    #include <Arduino.h>
    #define IotexString String

    #if defined(ESP8266) || defined(ESP32) 
        #include <pgmspace.h>
        #define IOTEX_DEBUG(msg) Serial.println(msg)
        #define IOTEX_DEBUG_F(fmt, ...) Serial.printf(fmt, ##__VA_ARGS__)
        #define IOTEX_DEBUG_HEX_BUF(buf, length) iotex::Helpers::printHex(buf, length)
    #endif

    #if defined(__SAMD21G18A__)
        #define IOTEX_DEBUG(msg) Serial.println(msg)
        #define IOTEX_DEBUG_F(fmt, ...)
        #define IOTEX_DEBUG_HEX_BUF(buf, length) iotex::Helpers::printHex(buf, length)
    #endif


#else  // #if (defined ARDUINO || defined ESP8266 || defined ESP32)
    #define IotexString std::string
        #define IOTEX_DEBUG(msg) printf("%s\r\n", msg)
    #define IOTEX_DEBUG_F(fmt, ...) printf(fmt, ##__VA_ARGS__)
    #define IOTEX_DEBUG_HEX_BUF(buf, length) iotex::Helpers::printHex(buf, length)
    #ifndef OS
        #define OS
    #endif
#endif  // #if (defined ARDUINO || defined ESP8266 || defined ESP32)

namespace iotex
{
    namespace Helpers
    {
        static void vectorToHexString(std::vector<uint8_t>& data, IotexString& out)
        {
            out = "";
            char charBuf[3] = {0};
            for (int i = 0; i < data.size(); i++)
            {
                sprintf(charBuf, "%02x", data[i]);
                out += charBuf;
            }
        }
        
        static inline void printHex(uint8_t* data, int length)
        {
            for (uint8_t i = 0; i < length; i++)
            {
                char buf[3] = {0};
                sprintf(buf, "%02x", data[i]);
                IOTEX_DEBUG(buf);
            }
            IOTEX_DEBUG("\r\n");
        }
    }
}



#endif


