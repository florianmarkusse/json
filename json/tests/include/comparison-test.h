#ifndef COMPARISON_TEST_H
#define COMPARISON_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "util/memory/arena.h"  // for flo_arena
#include "util/text/string.h"   // for flo_string
#include "json/deserialize.h"   // for flo_json_deserializationResult
#include "json/struct-member.h" // for flo_json_schema
#include "json/types.h"         // for flo_json_object
#include <stddef.h>             // for ptrdiff_t

typedef struct {
    flo_json_deserializationResult result;
    flo_string failingKey;
} flo_ExpectedTestResult;

typedef bool (*jsonStructComparison)(flo_json_object *, flo_json_object *);

void flo_json_comparisonTest(flo_json_object *toDeserializeInto,
                             flo_json_object *expectedDeserialized,
                             flo_json_schema root, flo_string json,
                             flo_ExpectedTestResult expected,
                             jsonStructComparison compareFunction,
                             ptrdiff_t maxNestingLength, flo_arena *perm);

#ifdef __cplusplus
}
#endif

#endif
