/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.6-dev */

#ifndef PB_ACTION_PB_H_INCLUDED
#define PB_ACTION_PB_H_INCLUDED
#include <extern/nanopb/pb.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Enum definitions */
typedef enum _Encoding { 
    Encoding_IOTEX_PROTOBUF = 0, 
    Encoding_ETHEREUM_RLP = 1 
} Encoding;

/* Struct definitions */
typedef struct _Execution { 
    pb_callback_t amount; 
    pb_callback_t contract; 
    pb_callback_t data; 
} Execution;

typedef struct _Transfer { 
    /* used by state-based model */
    pb_callback_t amount; 
    pb_callback_t recipient; /* bytes payload = 3; // Unused */
} Transfer;

typedef struct _ActionCore { 
    uint32_t version; 
    uint64_t nonce; 
    uint64_t gasLimit; 
    pb_callback_t gasPrice; 
    pb_size_t which_action;
    union {
        Transfer transfer;
        Execution execution;
    } action; 
} ActionCore;

typedef struct _Action { 
    bool has_core;
    ActionCore core; 
    pb_callback_t senderPubKey; 
    pb_callback_t signature; 
    Encoding encoding; 
} Action;


/* Helper constants for enums */
#define _Encoding_MIN Encoding_IOTEX_PROTOBUF
#define _Encoding_MAX Encoding_ETHEREUM_RLP
#define _Encoding_ARRAYSIZE ((Encoding)(Encoding_ETHEREUM_RLP+1))


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define Transfer_init_default                    {{{NULL}, NULL}, {{NULL}, NULL}}
#define Execution_init_default                   {{{NULL}, NULL}, {{NULL}, NULL}, {{NULL}, NULL}}
#define ActionCore_init_default                  {0, 0, 0, {{NULL}, NULL}, 0, {Transfer_init_default}}
#define Action_init_default                      {false, ActionCore_init_default, {{NULL}, NULL}, {{NULL}, NULL}, _Encoding_MIN}
#define Transfer_init_zero                       {{{NULL}, NULL}, {{NULL}, NULL}}
#define Execution_init_zero                      {{{NULL}, NULL}, {{NULL}, NULL}, {{NULL}, NULL}}
#define ActionCore_init_zero                     {0, 0, 0, {{NULL}, NULL}, 0, {Transfer_init_zero}}
#define Action_init_zero                         {false, ActionCore_init_zero, {{NULL}, NULL}, {{NULL}, NULL}, _Encoding_MIN}

/* Field tags (for use in manual encoding/decoding) */
#define Execution_amount_tag                     1
#define Execution_contract_tag                   2
#define Execution_data_tag                       3
#define Transfer_amount_tag                      1
#define Transfer_recipient_tag                   2
#define ActionCore_version_tag                   1
#define ActionCore_nonce_tag                     2
#define ActionCore_gasLimit_tag                  3
#define ActionCore_gasPrice_tag                  4
#define ActionCore_transfer_tag                  10
#define ActionCore_execution_tag                 12
#define Action_core_tag                          1
#define Action_senderPubKey_tag                  2
#define Action_signature_tag                     3
#define Action_encoding_tag                      4

/* Struct field encoding specification for nanopb */
#define Transfer_FIELDLIST(X, a) \
X(a, CALLBACK, SINGULAR, STRING,   amount,            1) \
X(a, CALLBACK, SINGULAR, STRING,   recipient,         2)
#define Transfer_CALLBACK pb_default_field_callback
#define Transfer_DEFAULT NULL

#define Execution_FIELDLIST(X, a) \
X(a, CALLBACK, SINGULAR, STRING,   amount,            1) \
X(a, CALLBACK, SINGULAR, STRING,   contract,          2) \
X(a, CALLBACK, SINGULAR, BYTES,    data,              3)
#define Execution_CALLBACK pb_default_field_callback
#define Execution_DEFAULT NULL

#define ActionCore_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UINT32,   version,           1) \
X(a, STATIC,   SINGULAR, UINT64,   nonce,             2) \
X(a, STATIC,   SINGULAR, UINT64,   gasLimit,          3) \
X(a, CALLBACK, SINGULAR, STRING,   gasPrice,          4) \
X(a, STATIC,   ONEOF,    MESSAGE,  (action,transfer,action.transfer),  10) \
X(a, STATIC,   ONEOF,    MESSAGE,  (action,execution,action.execution),  12)
#define ActionCore_CALLBACK pb_default_field_callback
#define ActionCore_DEFAULT NULL
#define ActionCore_action_transfer_MSGTYPE Transfer
#define ActionCore_action_execution_MSGTYPE Execution

#define Action_FIELDLIST(X, a) \
X(a, STATIC,   OPTIONAL, MESSAGE,  core,              1) \
X(a, CALLBACK, SINGULAR, BYTES,    senderPubKey,      2) \
X(a, CALLBACK, SINGULAR, BYTES,    signature,         3) \
X(a, STATIC,   SINGULAR, UENUM,    encoding,          4)
#define Action_CALLBACK pb_default_field_callback
#define Action_DEFAULT NULL
#define Action_core_MSGTYPE ActionCore

extern const pb_msgdesc_t Transfer_msg;
extern const pb_msgdesc_t Execution_msg;
extern const pb_msgdesc_t ActionCore_msg;
extern const pb_msgdesc_t Action_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define Transfer_fields &Transfer_msg
#define Execution_fields &Execution_msg
#define ActionCore_fields &ActionCore_msg
#define Action_fields &Action_msg

/* Maximum encoded size of messages (where known) */
/* Transfer_size depends on runtime parameters */
/* Execution_size depends on runtime parameters */
/* ActionCore_size depends on runtime parameters */
/* Action_size depends on runtime parameters */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
