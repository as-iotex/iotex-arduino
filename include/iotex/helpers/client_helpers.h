/**
 * Conditionally include Arduino or Linux headers 
 */ 

#ifndef CLIENT_HELPERS_H
#define CLIENT_HELPERS_H

#include <cstring>
#include <string>

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

#endif
