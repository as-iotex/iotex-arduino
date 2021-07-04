cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

project(iotex-antenna-arduino)

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)
set(PROJECT_VERSION_PATCH 0)

set(CMAKE_CXX_STANDARD 11)

add_library(iotex-antenna-arduino STATIC
    # Extern
        # JSON
        extern/cjson/cJSON.c
        # Base64
        extern/base64/base64.c
    # Crypto
    #     extern/crypto/secp256k1.c
    #     extern/crypto/ecdsa.c
    #     extern/crypto/sha2.c    
    #     extern/crypto/sha3.c    
    #     extern/crypto/bignum.c    
    #     extern/crypto/rand.c    
    #     extern/crypto/memzero.c    
    #     extern/crypto/rfc6979.c    
    #     extern/crypto/hmac_drbg.c    
    
    # # NanoPb
    #     extern/nanopb/pb_common.c
    #     extern/nanopb/pb_encode.c
    #     extern/nanopb/pb_decode.c

    # Iotex embedded
        # extern/iotex-emb/abi_pack.c
        # extern/iotex-emb/abi_read_contract.c
        # extern/iotex-emb/config.c
        # extern/iotex-emb/endian_conv.c
        # extern/iotex-emb/iotex_emb.c
        # extern/iotex-emb/parse.c
        # extern/iotex-emb/pb_pack.c
        # extern/iotex-emb/pb_proto.c
        # extern/iotex-emb/request.c
        # extern/iotex-emb/response.c
        # extern/iotex-emb/rule.c
        # extern/iotex-emb/signer.c
        # extern/iotex-emb/u128.c
        # extern/iotex-emb/utils.c

    # Protobuf
        # protobuf/c_files/action.pb.c

    # Iotex-arduino
        src/http/os/http.cpp        
        src/host/host.cpp        
        src/api/rpc.cpp
        src/api/wallet/wallets.cpp
        src/api/protobuf/pb_api.cpp
)

target_include_directories(iotex-antenna-arduino 
    PUBLIC include

    # Extern
    # PUBLIC extern/BigNumber/src
    # PUBLIC extern/BigNumber/src/BigNumber
    PUBLIC extern
    # PUBLIC extern/crypto
    # PUBLIC extern/tiny-bignum-c
    # PUBLIC extern/iotex-emb/include/iotex-emb
    # PUBLIC extern/iotex-emb/include
    # PUBLIC extern/nanopb
    # PUBLIC protobuf/c_files

    # Iotex-arduino
    PUBLIC include/iotex
)

target_compile_definitions(iotex-antenna-arduino
    PUBLIC _DEBUG_HTTP_
    # PUBLIC ARDUINO
)

target_link_libraries(iotex-antenna-arduino
    # Remove curl for Arduino
    PRIVATE curl
)

add_subdirectory(tests)
