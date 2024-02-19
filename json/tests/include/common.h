#ifndef COMMON_H
#define COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include "util/text/string.h"
#include "json/types.h"
#include <stddef.h>

#define FLO_COMMON_JSON_COMPARE(a, b)                                          \
    if ((a)->flags != (b)->flags) {                                            \
        return false;                                                          \
    }                                                                          \
    if (!((a)->flags & FLO_JSON_PRESENT)) {                                    \
        return true;                                                           \
    }                                                                          \
    if ((a)->flags & FLO_JSON_NULL) {                                          \
        return true;                                                           \
    }

__attribute__((unused)) static inline bool
compareJsonString(flo_json_string *a, flo_json_string *b) {
    FLO_COMMON_JSON_COMPARE(a, b);
    return flo_stringEquals(a->value, b->value);
}

__attribute__((unused)) static inline bool compareJsonBool(flo_json_bool *a,
                                                           flo_json_bool *b) {
    FLO_COMMON_JSON_COMPARE(a, b);
    return a->value == b->value;
}

__attribute__((unused)) static inline bool
compareJsonUint64(flo_json_uint64 *a, flo_json_uint64 *b) {
    FLO_COMMON_JSON_COMPARE(a, b);
    return a->value == b->value;
}

__attribute__((unused)) static inline bool
flo_json_compareJsonBoolArray(flo_json_bool_array_max_a *a,
                              flo_json_bool_array_max_a *b) {
    FLO_COMMON_JSON_COMPARE(a, b);
    if (a->value.len != b->value.len) {
        return false;
    }

    for (ptrdiff_t i = 0; i < a->value.len; i++) {
        FLO_COMMON_JSON_COMPARE(&a->value.buf[i], &b->value.buf[i]);
        if (a->value.buf[i].value != b->value.buf[i].value) {
            return false;
        }
    }

    return true;
}

__attribute__((unused)) static inline bool
flo_json_compareJsonUint64Array(flo_json_uint64_array_max_a *a,
                                flo_json_uint64_array_max_a *b) {
    FLO_COMMON_JSON_COMPARE(a, b);
    if (a->value.len != b->value.len) {
        return false;
    }

    for (ptrdiff_t i = 0; i < a->value.len; i++) {
        FLO_COMMON_JSON_COMPARE(&a->value.buf[i], &b->value.buf[i]);
        if (a->value.buf[i].value != b->value.buf[i].value) {
            return false;
        }
    }

    return true;
}

__attribute__((unused)) static inline bool
flo_json_compareJsonStringArray(flo_json_string_array_max_a *a,
                                flo_json_string_array_max_a *b) {
    FLO_COMMON_JSON_COMPARE(a, b);
    if (a->value.len != b->value.len) {
        return false;
    }

    for (ptrdiff_t i = 0; i < a->value.len; i++) {
        FLO_COMMON_JSON_COMPARE(&a->value.buf[i], &b->value.buf[i]);
        if (!flo_stringEquals(a->value.buf[i].value, b->value.buf[i].value)) {
            return false;
        }
    }

    return true;
}

#ifdef __cplusplus
}
#endif

#endif
