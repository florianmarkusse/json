#ifndef JSON_TYPES_BOOL_H
#define JSON_TYPES_BOOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "parse-framework/parse.h" // for flo_parseStatus
#include "json/deserialize.h"      // for flo_json_deserializationResult

typedef struct {
    bool parsed;
    flo_json_deserializationResult result;
} flo_json_boolParseResult;
flo_json_boolParseResult flo_json_parseBool(flo_parseStatus *ps);

#ifdef __cplusplus
}
#endif

#endif
