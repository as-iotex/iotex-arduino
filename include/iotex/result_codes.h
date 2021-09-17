#ifndef IOTEX_RESULT_CODES_H
#define IOTEX_RESULT_CODES_H

namespace Iotex
{
    enum class ResultCode
    {
        SUCCESS,
        ERROR_HTTP,
        ERROR_JSON_PARSE,
        ERROR_BAD_PARAMETER,
        ERROR_SIGNATURE,
        ERROR_UNKNOWN,
        ERROR_STORAGE_OPEN,
        ERROR_STORAGE_BAD_READ,
        ERROR_GRPC,
        ERROR_WRONG_TYPE
    };
}

#endif