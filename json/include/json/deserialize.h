#ifndef JSON_DESERIALIZE_H
#define JSON_DESERIALIZE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "parse-framework/parse.h" // for flo_parseStatus
#include "util/text/string.h"      // for flo_string
#include "util/types.h"            // for flo_string_max_a
#include "json/struct-member.h"    // for flo_json_schema
#include "json/types.h"
#include <stddef.h> // for ptrdiff_t

typedef enum {
    FLO_DESERIALIZATION_SUCCESS,
    FLO_DESERIALIZATION_MAX_NESTING,
    FLO_DESERIALIZATION_BAD_FORMAT,
    FLO_DESERIALIZATION_OUT_OF_RANGE,
    FLO_DESERIALIZATION_MAX_ELEMENTS,
    FLO_DESERIALIZATION_FAILURE,
    FLO_DESERIALIZATION_NUM_STATUS
} flo_json_deserializationResult;

flo_string
flo_json_deserializationResultToString(flo_json_deserializationResult result);

flo_json_deserializationResult
flo_json_parseKey(flo_json_void *toDeserialize, flo_string_max_a *nestedKeys,
                  flo_parseStatus *ps, flo_json_underlyingType underlyingType,
                  struct flo_json_schema_a *schemas, ptrdiff_t totalKeySize);

flo_json_deserializationResult flo_json_parse(flo_json_void *toDeserialize,
                                              flo_string_max_a *nestedKeys,
                                              flo_parseStatus *ps,
                                              flo_json_schema schema);
flo_json_deserializationResult flo_json_parse_root(flo_json_void *toDeserialize,
                                                   flo_string_max_a *nestedKeys,
                                                   flo_string json,
                                                   flo_json_schema schema);

#ifdef __cplusplus
}
#endif

#endif
