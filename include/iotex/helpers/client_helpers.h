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

#else
    #define IotexString std::string
    #define IOTEX_DEBUG_F(fmt, ...) printf(fmt, ##__VA_ARGS__)
    #ifndef OS
        #define OS
    #endif
#endif


namespace Iotex
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
    }
}



#endif


