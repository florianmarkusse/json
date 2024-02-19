#ifndef JSON_TYPES_ARRAY_H
#define JSON_TYPES_ARRAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "parse-framework/parse.h" // for flo_parseStatus
#include "util/types.h"            // for flo_string_max_a
#include "json/deserialize.h"      // for flo_json_deserializationResult
#include "json/struct-member.h"    // for flo_json_schema_a (ptr only), flo...
#include "json/types.h"            // for flo_json_void_array_max_a
#include <stddef.h>                // for ptrdiff_t

flo_json_deserializationResult flo_json_deserializeArray(
    flo_parseStatus *ps, struct flo_json_schema_a *keys, ptrdiff_t totalKeySize,
    flo_json_void_array_max_a *array, flo_json_underlyingType elementType,
    flo_string_max_a *nestedKeys);

#ifdef __cplusplus
}
#endif

#endif
