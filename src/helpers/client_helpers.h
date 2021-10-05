/**
 * Conditionally include Arduino or Linux headers 
 */ 

#ifndef CLIENT_HELPERS_H
#define CLIENT_HELPERS_H

#include <cstring>
#include <string>
#include <vector>

#if (defined ARDUINO || defined ESP8266 || defined ESP32)

#define USE_IOT

#include <Arduino.h>
#include <pgmspace.h>

#define IotexString String
#define IOTEX_DEBUG_F(fmt, ...) Serial.printf(fmt, ##__VA_ARGS__)
#define IOTEX_DEBUG_HEX_BUF(buf, length) iotex::helpers::printHex(buf, length)

#else
    #define IotexString std::string
    #define IOTEX_DEBUG_F(fmt, ...) printf(fmt, ##__VA_ARGS__)
    #define IOTEX_DEBUG_HEX_BUF(buf, length) iotex::helpers::printHex(buf, length)
    #ifndef OS
        #define OS
    #endif
#endif


namespace iotex
{
    namespace helpers
    {
        inline void vectorToHexString(std::vector<uint8_t>& data, IotexString& out)
        {
            out = "";
            char charBuf[3] = {0};
            for (int i = 0; i < data.size(); i++)
            {
                sprintf(charBuf, "%02x", data[i]);
                out += charBuf;
            }
        }
        
        inline void printHex(uint8_t* data, int length)
        {
            for (uint8_t i = 0; i < length; i++)
            {
                IOTEX_DEBUG_F("%02x", data[i]);
            }
            IOTEX_DEBUG_F("\r\n");
        }
    }
}



#endif


