#include "json/types/string.h"
#include <stddef.h> // for ptrdiff_t
#include <stdint.h> // for uint32_t, uint64_t

flo_json_stringParseResult flo_json_parseString(flo_parseStatus *ps) {
    flo_json_stringParseResult result;
    if (flo_parseCurrentChar(*ps) == '\"') {
        ps->idx++;
    } else {
        result.result = FLO_DESERIALIZATION_BAD_FORMAT;
        return result;
    }
    ptrdiff_t valueStart = ps->idx;
    uint32_t valueLen = 0;
    uint64_t escapes = 0;
    FLO_PARSE_NEXT_CHAR_UNTIL(*ps, (ch == '\"' && escapes % 2 == 0), {
        valueLen++;
        if (ch == '\\') {
            escapes++;
        } else {
            escapes = 0;
        }
    })
    if (flo_parseCurrentChar(*ps) != '\"') {
        result.result = FLO_DESERIALIZATION_BAD_FORMAT;
        return result;
    }
    ps->idx++;

    result.parsed =
        FLO_STRING_LEN(flo_getCharPtr(ps->text, valueStart), valueLen);
    result.result = FLO_DESERIALIZATION_SUCCESS;
    return result;
}
