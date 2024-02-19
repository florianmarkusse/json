#ifndef JSON_TYPES_UINT64_H
#define JSON_TYPES_UINT64_H

#ifdef __cplusplus
extern "C" {
#endif

#include "parse-framework/parse.h" // for flo_parseStatus
#include "json/deserialize.h"      // for flo_json_deserializationResult
#include <stdint.h>                // for uint64_t

typedef struct {
    uint64_t parsed;
    flo_json_deserializationResult result;
} flo_json_uint64ParseResult;
flo_json_uint64ParseResult flo_json_parseUint64(flo_parseStatus *ps);

#ifdef __cplusplus
}
#endif

#endif
