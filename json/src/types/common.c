#include "json/types/common.h"
#include "util/text/char.h"   // for flo_isFormattingCharacter
#include "util/text/string.h" // for FLO_STRING, flo_getCharPtr
#include <string.h>           // for memcmp

bool flo_json_isEndOfJsonValue(unsigned char ch) {
    return ch == '}' || ch == ']' || ch == ',' || ch == ' ' ||
           flo_isFormattingCharacter(ch);
}

#define NULL_STRING FLO_STRING("null")
bool flo_json_isNullValue(flo_parseStatus ps) {
    return ps.idx < ps.text.len - NULL_STRING.len + 1 &&
           !memcmp(flo_getCharPtr(ps.text, ps.idx), NULL_STRING.buf,
                   NULL_STRING.len);
}
