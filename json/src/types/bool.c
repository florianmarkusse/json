#include "json/types/bool.h"
#include "util/text/string.h" // for FLO_STRING
#include <string.h>           // for memcmp

#define TRUE_STRING FLO_STRING("true")
#define FALSE_STRING FLO_STRING("false")

flo_json_boolParseResult flo_json_parseBool(flo_parseStatus *ps) {
    if (ps->idx <= ps->text.len - TRUE_STRING.len &&
        !memcmp(&ps->text.buf[ps->idx], TRUE_STRING.buf, TRUE_STRING.len)) {
        return (flo_json_boolParseResult){
            .parsed = true, .result = FLO_DESERIALIZATION_SUCCESS};
    }

    if (ps->idx <= ps->text.len - FALSE_STRING.len &&
        !memcmp(&ps->text.buf[ps->idx], FALSE_STRING.buf, FALSE_STRING.len)) {
        return (flo_json_boolParseResult){
            .parsed = false, .result = FLO_DESERIALIZATION_SUCCESS};
    }

    return (flo_json_boolParseResult){.result = FLO_DESERIALIZATION_BAD_FORMAT};
}
