#include "json/types/array.h"
#include "parse-framework/parse.h"
#include "json/deserialize.h"
#include "json/struct-member.h"
#include "json/types.h" // for FLO_JSON_PRESENT, flo_json_string_max_a
#include <stddef.h>

flo_json_deserializationResult flo_json_deserializeArray(
    flo_parseStatus *ps, struct flo_json_schema_a *keys, ptrdiff_t totalKeySize,
    flo_json_void_array_max_a *array, flo_json_underlyingType elementType,
    flo_string_max_a *nestedKeys) {
    if (flo_parseCurrentChar(*(ps)) == '[') {
        (ps)->idx++;
    } else {
        return FLO_DESERIALIZATION_BAD_FORMAT;
    }
    FLO_PARSE_SKIP_EMPTY_SPACE(*ps);

    while ((ps)->idx < (ps)->text.len) {
        if (array->value.len >= array->value.cap) {
            return FLO_DESERIALIZATION_MAX_ELEMENTS;
        }
        flo_json_void *objectToDeserialize =
            (flo_json_void *)((char *)array->value.buf +
                              array->value.len * totalKeySize);
        flo_json_deserializationResult parseResult =
            flo_json_parseKey(objectToDeserialize, nestedKeys, ps, elementType,
                              keys, totalKeySize);
        if (parseResult != FLO_DESERIALIZATION_SUCCESS) {
            return parseResult;
        }
        array->value.len++;

        FLO_PARSE_NEXT_CHAR_UNTIL(*(ps), ch == ',', {
            if (ch == ']') {
                array->flags = FLO_JSON_PRESENT;
                return FLO_DESERIALIZATION_SUCCESS;
            }
        });
        (ps)->idx++;
        FLO_PARSE_SKIP_EMPTY_SPACE(*(ps));
    }
    return FLO_DESERIALIZATION_BAD_FORMAT;
}
