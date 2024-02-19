#ifndef JSON_SERIALIZE_H
#define JSON_SERIALIZE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "util/memory/arena.h" // for flo_arena
#include "util/types.h"        // for flo_char_d_a, flo_string_max_a, flo_b...
#include "json/struct-member.h"
#include "json/types.h" // for FLO_JSON_CREATE_TYPE

void flo_json_append(flo_char_d_a *buffer, flo_json_void *json,
                     flo_json_schema schema, flo_arena *perm);

#ifdef __cplusplus
}
#endif

#endif
