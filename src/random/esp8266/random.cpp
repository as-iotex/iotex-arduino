#ifdef ESP8266
#include "random/random.h"

#include "ESP8266TrueRandom.h"

using namespace Iotex;

Random randomGenerator;

void Random::fillRandom(uint8_t *buf, size_t size)
{
    for (int i = 0; i < size; i++)
    {
        buf[i] = ESP8266TrueRandom.randomByte();
    }
}

#endif