#ifndef BASE_H
#define BASE_H

#include "host/host.h"
#include "http/http.h"

#define IOTEX_ADDRESS_STRLEN    41
#define IOTEX_MAX_BALANCE_STRLEN    30
#define IOTEX_HASH_STRLEN    64
#define IOTEX_PUBLIC_KEY_STRLEN    88
#define IOTEX_SIGNATURE_STRLEN    88
#define IOTEX_TIMESTAMP_STRLEN    21 // TODO check if this is correct or it can be bigger

namespace Iotex 
{
    namespace api 
    {
        enum class ResultCode
        {
            SUCCESS,
            ERROR_HTTP,
            ERROR_JSON_PARSE,
            ERROR_UNKNOWN
        };

        class Base 
        {
            protected:
            Host& host_;
            IHTTP* http_;

            template <typename HTTPType>
            explicit Base(Host& host, HTTPType& http)
                : host_(host), http_(static_cast<IHTTP*>(&http)) {}
        };

    }  // namespace api
}  // namespace Iotex

#endif
