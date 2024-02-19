#include "json/types/object.h"
#include "parse-framework/parse.h" // for FLO_PARSE_NEXT_CHAR_UNTIL, flo_pa...
#include "util/text/string.h"      // for flo_getCharOr, FLO_STRING
#include "util/types.h"
#include "json/deserialize.h"   // for FLO_DESERIALIZATION_SUCCESS, flo_...
#include "json/struct-member.h" // for flo_json_schema_a
#include "json/types.h"
#include "json/types/common.h" // for flo_json_isEndOfJsonValue
#include <stddef.h>            // for ptrdiff_t, NULL
#include <stdint.h>            // for uint64_t, uint32_t

/**
 * Assumes it enters exactly after the first quote.
 * And leaves the parsestatus to after the closing quote.
 * keyLen can overflow in some dumb scenario but that does no harm.
 */
ptrdiff_t getMemberIndex(flo_parseStatus *ps, struct flo_json_schema_a *keys) {
    uint64_t acceptMembers = ~0ULL;
    uint32_t keyLen = 0;
    uint64_t escapes = 0;

    FLO_PARSE_NEXT_CHAR_UNTIL(*ps, (ch == '\"' && escapes % 2 == 0), {
        if (ch == '\\') {
            escapes++;
        } else {
            escapes = 0;
        }
        for (ptrdiff_t i = 0; i < keys->len; i++) {
            // Set the ith bit to 0 if ch is not equal to that in the
            // keyName buffer.
            acceptMembers &=
                ~(((uint64_t)(ch !=
                              flo_getCharOr(keys->buf[i].keyName, keyLen, '\0'))
                   << i));
        }

        keyLen++;
    })

    for (ptrdiff_t i = 0; i < keys->len; i++) {
        if (acceptMembers & (1ULL << i) && keys->buf[i].keyName.len == keyLen) {
            return i;
        }
    }

    return -1;
}

flo_json_deserializationResult
flo_deserializeObject(flo_json_object *toDeserialize,
                      flo_string_max_a *nestedKeys, flo_parseStatus *ps,
                      struct flo_json_schema_a *keys) {
    bool foundOpenBrace = false;
    FLO_PARSE_NEXT_CHAR_UNTIL(*ps, ch == '{');
    if (flo_parseCurrentChar(*ps)) {
        foundOpenBrace = true;
    }
    FLO_PARSE_NEXT_CHAR_UNTIL(*ps, ch == '\"' || ch == '}');
    // Empty object.
    if (flo_parseCurrentChar(*ps) == '}' && foundOpenBrace) {
        return FLO_DESERIALIZATION_SUCCESS;
    }

    while (ps->idx < ps->text.len) {
        ps->idx++;
        ptrdiff_t parsedKeyIndex = getMemberIndex(ps, keys);

        FLO_PARSE_NEXT_CHAR_UNTIL(*ps, ch == ':');
        ps->idx++;
        FLO_PARSE_SKIP_EMPTY_SPACE(*ps);

        if (parsedKeyIndex < 0) {
            uint64_t escapes = 0;
            bool isInString = false;
            FLO_PARSE_NEXT_CHAR_UNTIL(
                *ps, (!isInString && flo_json_isEndOfJsonValue(ch)), {
                    if (ch == '\"' && escapes % 2 == 0) {
                        isInString = !isInString;
                    }
                    if (ch == '\\') {
                        escapes++;
                    } else {
                        escapes = 0;
                    }
                })
        } else {
            flo_json_schema schema = keys->buf[parsedKeyIndex];
            flo_json_deserializationResult deserializationResult =
                flo_json_parse(
                    (flo_json_void *)((char *)toDeserialize + schema.offset),
                    nestedKeys, ps, schema);
            if (deserializationResult != FLO_DESERIALIZATION_SUCCESS) {
                return deserializationResult;
            }
            nestedKeys->len--;
        }

        FLO_PARSE_NEXT_CHAR_UNTIL(*ps, ch == '\"' || ch == '}');
        if (flo_parseCurrentChar(*ps) == '}') {
            ps->idx++;
            return FLO_DESERIALIZATION_SUCCESS;
        }
    }

    return FLO_DESERIALIZATION_BAD_FORMAT;
}
