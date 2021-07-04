#include "encoder/encoder.h"
#include "signer/signer.h"

#include "bech32.h"

#include "nanopb/pb_common.h"
#include "nanopb/pb_encode.h"
#include "nanopb/pb_decode.h"
#include "action.pb.h"

#include <string>
#include <string.h>
#include <vector>


#ifdef	__cplusplus
extern "C" {
#endif

#include "base64/base64.h"

#ifdef	__cplusplus
}
#endif

using namespace Iotex;
using namespace std;

namespace
{
    bool encode_string(pb_ostream_t* stream, const pb_field_t* field, void* const* arg)
    {
        const char* str = (const char*)(*arg);

        if (!pb_encode_tag_for_field(stream, field))
            return false;

        return pb_encode_string(stream, (uint8_t*)str, strlen(str));
    }

    bool encode_bytes(pb_ostream_t* stream, const pb_field_t* field, void* const* arg)
    {
        const uint8_t* bytes = (const uint8_t*)(*arg);

        if (!pb_encode_tag_for_field(stream, field))
            return false;

        return pb_encode_string(stream, (uint8_t*)bytes, 0);    // Don't encode anything for now, payload should be empty
    }
}

// Define global encoder
Encoder encoder;

ResultCode Encoder::bech32_encodeHexString(const std::string &data, std::string &encodedData)
{
    // Convert string to vector and delegate
    uint8_t bytes[data.length()/2];
    signer.str2hex(data.c_str(), bytes, sizeof(bytes));
    std::vector<uint8_t> v(bytes, bytes + sizeof(bytes));
    bech32_encode(v, encodedData);

    return ResultCode::SUCCESS;
}

ResultCode Encoder::bech32_encode(const vector<uint8_t> data, std::string &encodedData)
{
    // Convert to 5 bit int
    vector<uint8_t> dataAs5BitUint;
    bech32_convertBytesTo5Bit(data, dataAs5BitUint);

    // Encode
    // Use "io" as prefix for mainnet
    encodedData = bech32::Encode(bech32::Encoding::BECH32, "io", dataAs5BitUint);

    return ResultCode::SUCCESS;
}

ResultCode Encoder::bech32_convertBytesTo5Bit(const vector<uint8_t> &in, vector<uint8_t>& out)
{
    // TODO Improve this algorithm. 
    // Not a big deal right now as this is only called once at account creation
    vector<uint8_t> bitsAsBytes;
    bitsAsBytes.reserve(in.size() * 8);

    for (int i = 0; i < in.size(); i++)
    {
        uint8_t currentByte = in[i];
        for (int b = 7; b >= 0; b--)
        {
            uint8_t bitValue = 0;
            bitValue = (currentByte & (1 << b)) != 0;
            bitsAsBytes.push_back(bitValue);
        }
    }

    size_t outSize = bitsAsBytes.size() / 5;
    uint8_t remainder = bitsAsBytes.size() % 5;
    uint8_t padding = remainder ? 5 - remainder : 0;
    out.clear();
    out.reserve(outSize);

    size_t currentBitIndex = 0;

    for (int i = 0; i < outSize; i++)
    {
        out.push_back(0);
        for (int b = 4; b >= 0; b--)
        {
            out[i] |= bitsAsBytes[currentBitIndex++] << b;
        }
    }

    if (remainder)
    {
        out.push_back(0);
        for (int b = 4; b >= 0; b--)
        {
            if (currentBitIndex < bitsAsBytes.size())
            {
                bitsAsBytes[outSize - 1] |= bitsAsBytes[currentBitIndex++] << b;
            }
        }
    }

    return ResultCode::SUCCESS;
}


ResultCode Encoder::base64_encode(const uint8_t *data, size_t size, char *out)
{
    bintob64(out, (void const*) data, size);
    return ResultCode::SUCCESS;
}

int32_t Encoder::protobuf_encodeTransfer(ResponseTypes::ActionCore_Transfer& transfer, uint8_t* out, size_t maxOutSize)
{
    ActionCore pbCore;
    uint8_t encodedCore[1024];

    pbCore.version = transfer.version;
    pbCore.gasLimit = transfer.gasLimit;
    pbCore.nonce = transfer.nonce;
    pbCore.gasPrice.arg = transfer.gasPrice;
    pbCore.gasPrice.funcs.encode = &encode_string;
    pbCore.chainID = transfer.chainId;
    pbCore.which_action = ActionCore_transfer_tag;
    pbCore.action.transfer.amount.arg = transfer.transfer.amount;
    pbCore.action.transfer.amount.funcs.encode = &encode_string;
    pbCore.action.transfer.payload.arg = (void*)transfer.transfer.payload.c_str();
    pbCore.action.transfer.payload.funcs.encode = &encode_bytes;
    pbCore.action.transfer.recipient.arg = transfer.transfer.recipient;
    pbCore.action.transfer.recipient.funcs.encode = &encode_string;

    pb_ostream_t stream = pb_ostream_from_buffer(encodedCore, sizeof(encodedCore));
    pb_encode(&stream, ActionCore_fields, &pbCore);

    if(stream.bytes_written > maxOutSize)
    {
        return -1;
    }

    memset(out, 0, maxOutSize);
    memcpy(out, encodedCore, stream.bytes_written);
    
    return stream.bytes_written;
}