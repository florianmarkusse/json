#ifndef JSON_TYPES_STRING_H
#define JSON_TYPES_STRING_H

#include "parse-framework/parse.h" // for flo_parseStatus
#include "util/text/string.h"      // for flo_string
#include "json/deserialize.h"      // for flo_json_deserializationResult

typedef struct {
    flo_string parsed;
    flo_json_deserializationResult result;
} flo_json_stringParseResult;
flo_json_stringParseResult flo_json_parseString(flo_parseStatus *ps);

#endif
