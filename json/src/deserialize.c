#include "json/deserialize.h"
#include "parse-framework/parse.h" // for flo_parseStatus
#include "util/assert.h"           // for FLO_ASSERT
#include "util/text/string.h"      // for FLO_STRING, flo_string
#include "util/types.h"            // for flo_string_max_a
#include "json/struct-member.h"    // for FLO_DESERIALIZE_BOOL, FLO_DESERIA...
#include "json/types.h"            // for FLO_JSON_PRESENT, flo_json_bool
#include "json/types/array.h"      // for flo_json_deserializeArray
#include "json/types/bool.h"       // for flo_json_parseBool, flo_json_bool...
#include "json/types/common.h"     // for flo_json_isNullValue
#include "json/types/object.h"     // for flo_deserializeObject
#include "json/types/string.h"     // for flo_json_parseString, flo_json_st...
#include "json/types/uint64.h"     // for flo_json_parseUint64, flo_json_ui...
#include <stddef.h>                // for ptrdiff_t
#include <stdint.h>                // for uint8_t

#define SET_VALUE(type, result, toDeserialize)                                 \
    *(type *)toDeserialize =                                                   \
        (type){.flags = FLO_JSON_PRESENT, .value = (result)};

flo_json_deserializationResult
flo_json_parseKey(flo_json_void *toDeserialize, flo_string_max_a *nestedKeys,
                  flo_parseStatus *ps, flo_json_underlyingType underlyingType,
                  struct flo_json_schema_a *schemas, ptrdiff_t totalKeySize) {
    if (flo_json_isNullValue(*ps)) {
        toDeserialize->flags = (uint8_t)FLO_JSON_PRESENT | FLO_JSON_NULL;
        return FLO_DESERIALIZATION_SUCCESS;
    }

    switch (underlyingType) {
    case FLO_DESERIALIZE_OBJECT: {
        flo_json_deserializationResult deserializationResult =
            flo_deserializeObject((flo_json_object *)toDeserialize, nestedKeys,
                                  ps, schemas);
        if (deserializationResult == FLO_DESERIALIZATION_SUCCESS) {
            toDeserialize->flags = (uint8_t)FLO_JSON_PRESENT;
        }
        return deserializationResult;
    }
    case FLO_DESERIALIZE_OBJECT_ARRAY:
    case FLO_DESERIALIZE_STRING_ARRAY:
    case FLO_DESERIALIZE_UINT64_ARRAY:
    case FLO_DESERIALIZE_BOOL_ARRAY: {
        return flo_json_deserializeArray(
            ps, schemas, totalKeySize,
            (flo_json_void_array_max_a *)toDeserialize, underlyingType - 1,
            nestedKeys);
    }
    case FLO_DESERIALIZE_STRING: {
        flo_json_stringParseResult parseResult = flo_json_parseString(ps);
        if (parseResult.result != FLO_DESERIALIZATION_SUCCESS) {
            return parseResult.result;
        }

        SET_VALUE(flo_json_string, parseResult.parsed, toDeserialize);
        return FLO_DESERIALIZATION_SUCCESS;
    }
    case FLO_DESERIALIZE_UINT64: {
        flo_json_uint64ParseResult parseResult = flo_json_parseUint64(ps);
        if (parseResult.result != FLO_DESERIALIZATION_SUCCESS) {
            return parseResult.result;
        }

        SET_VALUE(flo_json_uint64, parseResult.parsed, toDeserialize);
        return FLO_DESERIALIZATION_SUCCESS;
    }
    case FLO_DESERIALIZE_BOOL: {
        flo_json_boolParseResult parseResult = flo_json_parseBool(ps);
        if (parseResult.result != FLO_DESERIALIZATION_SUCCESS) {
            return parseResult.result;
        }

        SET_VALUE(flo_json_bool, parseResult.parsed, toDeserialize);
        return FLO_DESERIALIZATION_SUCCESS;
    }
    }
    FLO_ASSERT(false);
    return FLO_DESERIALIZATION_FAILURE;
}

/**
 * Assumes it enters at the first actual character that makes up the value of a
 * key. Leaves the parsestatus to after the last character of the key.
 */
flo_json_deserializationResult flo_json_parse(flo_json_void *toDeserialize,
                                              flo_string_max_a *nestedKeys,
                                              flo_parseStatus *ps,
                                              flo_json_schema schema) {
    if (nestedKeys->len >= nestedKeys->cap) {
        return FLO_DESERIALIZATION_MAX_NESTING;
    }
    nestedKeys->buf[nestedKeys->len++] = schema.keyName;

    return flo_json_parseKey(toDeserialize, nestedKeys, ps,
                             schema.underlyingType, schema.keys,
                             schema.byteSize);
}

flo_json_deserializationResult flo_json_parse_root(flo_json_void *toDeserialize,
                                                   flo_string_max_a *nestedKeys,
                                                   flo_string json,
                                                   flo_json_schema schema) {
    flo_parseStatus ps = {.idx = 0, .text = json};
    return flo_json_parse(toDeserialize, nestedKeys, &ps, schema);
}

static flo_string
    deserializationResultToString[FLO_DESERIALIZATION_NUM_STATUS] = {
        FLO_STRING("Success"),
        FLO_STRING("Max JSON nesting"),
        FLO_STRING("Bad format"),
        FLO_STRING("Out of range"),
        FLO_STRING("Exceeded maximum number of elements"),
};

flo_string
flo_json_deserializationResultToString(flo_json_deserializationResult result) {
    if (result >= 0 && result < FLO_DESERIALIZATION_NUM_STATUS) {
        return deserializationResultToString[result];
    }
    return FLO_STRING("Unknown deserialization result status code!");
}
