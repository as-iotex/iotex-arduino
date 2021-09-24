#ifndef IOTEX_REFLECTION_HELPER_H
#define IOTEX_REFLECTION_HELPER_H

namespace Iotex
{
    namespace reflection
    {
        /**
         * @enum CppType  Variable types, for reflection purposes
         */
        enum class CppType
        {
            UINT8,
            UINT16,
            UINT32,
            UINT64,
            BIGINT,
            BOOLEAN,
            C_STRING,
            STRING,
            OBJECT,
            BYTES,
            ARRAY // JSON array
        };
    }
}

#endif