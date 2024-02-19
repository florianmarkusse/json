#include "json/serialize.h"
#include "util/log.h"           // for FLO_APPEND_DATA
#include "util/memory/arena.h"  // for flo_arena
#include "util/text/string.h"   // for FLO_STRING, FLO_EMPTY_STRING
#include "util/types.h"         // for flo_char_d_a
#include "json/struct-member.h" // for flo_json_schema, flo_json_schema_a
#include "json/types.h"         // for (anonymous union)::(anonymous), flo_...
#include <stddef.h>             // for ptrdiff_t

void flo_appendJson(flo_char_d_a *buffer, flo_json_void *json,
                    flo_json_schema schema, flo_arena *perm,
                    bool *hasWrittenKey, bool isRoot) {
    if (!json->flags) {
        return;
    }

    if (!isRoot) {
        if (*hasWrittenKey) {
            FLO_APPEND_DATA(FLO_STRING(","), buffer, perm);
        }
        if (schema.keyName.len > 0) {
            FLO_APPEND_DATA("\"", buffer, perm);
            FLO_APPEND_DATA(schema.keyName, buffer, perm);
            FLO_APPEND_DATA("\"", buffer, perm);
            FLO_APPEND_DATA(FLO_STRING(":"), buffer, perm);
        }
    }

    if ((json)->flags & FLO_JSON_NULL) {
        FLO_APPEND_DATA(FLO_STRING("null"), buffer, perm);
    } else if ((json)->flags & FLO_JSON_PRESENT) {
        switch (schema.underlyingType) {
        case FLO_DESERIALIZE_OBJECT: {
            FLO_APPEND_DATA(FLO_STRING("{"), buffer, perm);
            *hasWrittenKey = false;
            for (ptrdiff_t i = 0; i < schema.keys->len; i++) {
                flo_appendJson(buffer,
                               (flo_json_void *)(((char *)json) +
                                                 schema.keys->buf[i].offset),
                               schema.keys->buf[i], perm, hasWrittenKey, false);
            }
            FLO_APPEND_DATA(FLO_STRING("}"), buffer, perm);
            break;
        }
        case FLO_DESERIALIZE_OBJECT_ARRAY:
        case FLO_DESERIALIZE_STRING_ARRAY:
        case FLO_DESERIALIZE_UINT64_ARRAY:
        case FLO_DESERIALIZE_BOOL_ARRAY: {
            FLO_APPEND_DATA(FLO_STRING("["), buffer, perm);
            *hasWrittenKey = false;
            flo_json_void_max_a *array = (flo_json_void_max_a *)&json->value;
            for (ptrdiff_t i = 0; i < array->len; i++) {
                flo_json_void *objectToSerialize =
                    (flo_json_void *)((char *)array->buf + i * schema.byteSize);
                flo_appendJson(buffer, objectToSerialize,
                               (flo_json_schema){.keys = schema.keys,
                                                 .byteSize = schema.byteSize,
                                                 .offset = schema.offset,
                                                 .keyName = FLO_EMPTY_STRING,
                                                 .underlyingType =
                                                     schema.underlyingType - 1},
                               perm, hasWrittenKey, false);
            }
            FLO_APPEND_DATA(FLO_STRING("]"), buffer, perm);
            break;
        }
        case FLO_DESERIALIZE_STRING: {
            FLO_APPEND_DATA(FLO_STRING("\""), buffer, perm);
            FLO_APPEND_DATA((*(flo_json_string *)json).value, buffer, perm);
            FLO_APPEND_DATA(FLO_STRING("\""), buffer, perm);
            break;
        }
        case FLO_DESERIALIZE_UINT64: {
            FLO_APPEND_DATA((*(flo_json_uint64 *)json).value, buffer, perm);
            break;
        }
        case FLO_DESERIALIZE_BOOL: {
            FLO_APPEND_DATA((*(flo_json_bool *)json).value, buffer, perm);
            break;
        }
        }
    }

    *hasWrittenKey = true;
}

void flo_json_append(flo_char_d_a *buffer, flo_json_void *json,
                     flo_json_schema schema, flo_arena *perm) {
    bool hasWrittenKey = false;
    flo_appendJson(buffer, json, schema, perm, &hasWrittenKey, true);
}
