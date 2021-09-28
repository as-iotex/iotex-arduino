#ifndef IOTEX_RANDOM_H
#define IOTEX_RANDOM_H

#include "stdint.h"
#include "stddef.h"

namespace iotex
{
    class Random
    {
        public:
            virtual void fillRandom(uint8_t* buf, size_t size);
    };
}

// Declare global object
extern iotex::Random randomGenerator;

#endif