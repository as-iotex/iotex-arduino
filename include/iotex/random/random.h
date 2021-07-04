#ifndef IOTEX_RANDOM_H
#define IOTEX_RANDOM_H

#include "stdint.h"
#include "stddef.h"

namespace Iotex
{
    class Random
    {
        public:
            virtual void fillRandom(uint8_t* buf, size_t size);
    };
}

// Declare global object
extern Iotex::Random randomGenerator;

#endif