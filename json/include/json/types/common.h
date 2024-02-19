#ifndef JSON_TYPES_COMMON_H
#define JSON_TYPES_COMMON_H

#include "parse-framework/parse.h" // for flo_parseStatus

bool flo_json_isEndOfJsonValue(unsigned char ch);
bool flo_json_isNullValue(flo_parseStatus ps);

#endif
