#ifndef JSON_TYPES_H
#define JSON_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "util/array.h"
#include "util/text/string.h"
#include <stdint.h>

// A json type can have the following configuration of these flags:
// - 0: a key that is omitted, may be an optional key.
// - FLO_JSON_PRESENT: a key that is present.
// - FLC_JSON_PRESENT | FLO_JSON_NULL: a key that is passed as null.
#define FLO_JSON_PRESENT 0x01
#define FLO_JSON_NULL 0x02

// Adding eztra padding here because otherwise a json object might be too small
// leading to writes to misaligned pointers leading to UB.
#define FLO_JSON_CREATE_TYPE(T)                                                \
    struct __attribute((aligned(8))) {                                         \
        uint8_t flags;                                                         \
        T value;                                                               \
    }

#define FLO_JSON_CREATE_OBJECT(T, name)                                        \
    typedef struct __attribute((aligned(8))) {                                 \
        uint8_t flags;                                                         \
        T value;                                                               \
    } name

typedef FLO_JSON_CREATE_TYPE(void *) flo_json_void;
typedef FLO_MAX_LENGTH_ARRAY(flo_json_void) flo_json_void_max_a;
typedef FLO_JSON_CREATE_TYPE(flo_json_void_max_a) flo_json_void_array_max_a;

typedef FLO_JSON_CREATE_TYPE(flo_string) flo_json_string;
typedef FLO_MAX_LENGTH_ARRAY(flo_json_string) flo_json_string_max_a;
typedef FLO_JSON_CREATE_TYPE(flo_json_string_max_a) flo_json_string_array_max_a;

typedef FLO_JSON_CREATE_TYPE(uint64_t) flo_json_uint64;
typedef FLO_MAX_LENGTH_ARRAY(flo_json_uint64) flo_json_uint64_max_a;
typedef FLO_JSON_CREATE_TYPE(flo_json_uint64_max_a) flo_json_uint64_array_max_a;

typedef FLO_JSON_CREATE_TYPE(bool) flo_json_bool;
typedef FLO_MAX_LENGTH_ARRAY(flo_json_bool) flo_json_bool_max_a;
typedef FLO_JSON_CREATE_TYPE(flo_json_bool_max_a) flo_json_bool_array_max_a;

typedef FLO_JSON_CREATE_TYPE(void *) flo_json_object;
typedef FLO_MAX_LENGTH_ARRAY(flo_json_object) flo_json_object_max_a;
typedef FLO_JSON_CREATE_TYPE(flo_json_object_max_a) flo_json_object_array_max_a;

#ifdef __cplusplus
}
#endif

#endif
