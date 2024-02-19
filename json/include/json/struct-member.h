#ifndef JSON_STRUCT_MEMBER_H
#define JSON_STRUCT_MEMBER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "util/macros.h"
#include "util/text/string.h"
#include <assert.h>
#include <limits.h>

// We are using a uint64 to represent whether or not a specific key is valid for
// the supplied JSON key
#define UINT64_BITS (sizeof(uint64_t) * CHAR_BIT)
#define FLO_JSON_MAX_KEYS_PER_STRUCT UINT64_BITS

#define MEMBER_TYPE(_struct, member) typeof(((_struct *)0)->member)

#define GET_CORRESPONDING_ENUM_SIMPLE(_struct, member)                         \
    _Generic((MEMBER_TYPE(_struct, member)){},                                 \
        flo_json_string: FLO_DESERIALIZE_STRING,                               \
        flo_json_string_array_max_a: FLO_DESERIALIZE_STRING_ARRAY,             \
        flo_json_uint64: FLO_DESERIALIZE_UINT64,                               \
        flo_json_uint64_array_max_a: FLO_DESERIALIZE_UINT64_ARRAY,             \
        flo_json_bool: FLO_DESERIALIZE_BOOL,                                   \
        flo_json_bool_array_max_a: FLO_DESERIALIZE_BOOL_ARRAY)

#define GET_CORRESPONDING_ENUM_OBJECT(_struct, member)                         \
    _Generic((MEMBER_TYPE(_struct, member)){},                                 \
        flo_json_object_array_max_a: FLO_DESERIALIZE_OBJECT_ARRAY,             \
        default: FLO_DESERIALIZE_OBJECT)

// if you add something on this enum, also add it to one of the macros above!!!
// The nunbering is important. During (de)serialization, the underlying type is
// decremented to its actual type when an array type is encountered to
// (de)serialize the array's elements.
typedef enum {
    FLO_DESERIALIZE_OBJECT = 0,
    FLO_DESERIALIZE_OBJECT_ARRAY = FLO_DESERIALIZE_OBJECT + 1,
    FLO_DESERIALIZE_STRING = 2,
    FLO_DESERIALIZE_STRING_ARRAY = FLO_DESERIALIZE_STRING + 1,
    FLO_DESERIALIZE_UINT64 = 4,
    FLO_DESERIALIZE_UINT64_ARRAY = FLO_DESERIALIZE_UINT64 + 1,
    FLO_DESERIALIZE_BOOL = 6,
    FLO_DESERIALIZE_BOOL_ARRAY = FLO_DESERIALIZE_BOOL + 1,
} flo_json_underlyingType;

typedef struct {
    flo_string keyName;
    ptrdiff_t offset;
    ptrdiff_t byteSize;
    flo_json_underlyingType underlyingType;
    struct flo_json_schema_a *keys;
} flo_json_schema;

struct flo_json_schema_a {
    flo_json_schema *buf;
    ptrdiff_t len;
};

#define FLO_JSON_CREATE_ROOT_3(_underlyingType, _keys, _struct)                \
    (flo_json_schema){.offset = 0,                                             \
                      .underlyingType = (_underlyingType),                     \
                      .byteSize = FLO_SIZEOF(_struct),                         \
                      .keys = (_keys),                                         \
                      .keyName = FLO_STRING(FLO_STRINGIFY(_struct))};

#define FLO_JSON_CREATE_ROOT_2(_underlyingType, _struct)                       \
    (flo_json_schema){.offset = 0,                                             \
                      .underlyingType = (_underlyingType),                     \
                      .byteSize = FLO_SIZEOF(_struct),                         \
                      .keys = NULL,                                            \
                      .keyName = FLO_STRING(FLO_STRINGIFY(_struct))};

#define FLO_JSON_CREATE_ROOT_X(a, b, c, d, ...) d
#define FLO_JSON_CREATE_ROOT(...)                                              \
    FLO_JSON_CREATE_ROOT_X(__VA_ARGS__, FLO_JSON_CREATE_ROOT_3,                \
                           FLO_JSON_CREATE_ROOT_2)                             \
    (__VA_ARGS__)

#define FLO_JSON_CREATE_KEYS_ARRAY(internalBuffer, keysArray)                  \
    static_assert(FLO_COUNTOF(internalBuffer) <=                               \
                  FLO_JSON_MAX_KEYS_PER_STRUCT);                               \
    struct flo_json_schema_a keysArray = (struct flo_json_schema_a) {          \
        .buf = (internalBuffer), .len = FLO_COUNTOF(internalBuffer)            \
    }

#define FLO_JSON_CREATE_OBJECT_KEY(jsonWrapper, _struct, _key, _keys, keyType) \
    {                                                                          \
        .keyName = FLO_STRING(#_key),                                          \
        .offset = offsetof(jsonWrapper, value) + offsetof(_struct, _key),      \
        .underlyingType = GET_CORRESPONDING_ENUM_OBJECT(_struct, _key),        \
        .byteSize = FLO_SIZEOF(keyType), .keys = (_keys)                       \
    }

#define FLO_JSON_CREATE_SIMPLE_ARRAY_KEY(jsonWrapper, _struct, _key,           \
                                         elementType)                          \
    {                                                                          \
        .keyName = FLO_STRING(#_key),                                          \
        .offset = offsetof(jsonWrapper, value) + offsetof(_struct, _key),      \
        .underlyingType = GET_CORRESPONDING_ENUM_SIMPLE(_struct, _key),        \
        .byteSize = FLO_SIZEOF(elementType),                                   \
    }

#define FLO_JSON_CREATE_SIMPLE_KEY(jsonWrapper, _struct, _key)                 \
    {                                                                          \
        .keyName = FLO_STRING(#_key),                                          \
        .offset = offsetof(jsonWrapper, value) + offsetof(_struct, _key),      \
        .underlyingType = GET_CORRESPONDING_ENUM_SIMPLE(_struct, _key),        \
        .byteSize = FLO_SIZEOF(MEMBER_TYPE(_struct, _key)),                    \
    }

#ifdef __cplusplus
}
#endif

#endif
