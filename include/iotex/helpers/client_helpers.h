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

#else
#define OS
#endif

#endif
