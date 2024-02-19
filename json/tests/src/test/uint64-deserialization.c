#include "test/uint64-deserialization.h"
#include "common.h"              // for compareJsonUint64, FLO_COMMON_JSON_...
#include "comparison-test.h"     // for flo_json_comparisonTest, flo_Expect...
#include "test-framework/test.h" // for FLO_TEST, FLO_TEST_TOPIC
#include "util/macros.h"         // for FLO_STRINGIFY
#include "util/memory/arena.h"
#include "util/memory/macros.h" // for FLO_COUNTOF
#include "util/text/string.h"   // for FLO_STRING, flo_string
#include "json/deserialize.h"   // for FLO_DESERIALIZATION_BAD_FORMAT, FLO...
#include "json/struct-member.h" // for FLO_JSON_CREATE_STRUCT_MEMBER, FLO_...
#include "json/types.h"         // for FLO_JSON_PRESENT, FLO_JSON_NULL
#include <stddef.h>             // for ptrdiff_t
#include <stdint.h>             // for UINT64_MAX

typedef struct flo_TestStruct {
    flo_json_uint64 a;
    flo_json_uint64 b;
    flo_json_uint64 c;
} flo_TestStruct;
FLO_JSON_CREATE_OBJECT(flo_TestStruct, flo_json_TestStruct);

static bool compareTestStruct(flo_json_object *a, flo_json_object *b) {
    FLO_COMMON_JSON_COMPARE(a, b);

    flo_json_TestStruct *aa = (flo_json_TestStruct *)a;
    flo_json_TestStruct *bb = (flo_json_TestStruct *)b;

    return compareJsonUint64(&aa->value.a, &bb->value.a) &&
           compareJsonUint64(&aa->value.b, &bb->value.b) &&
           compareJsonUint64(&aa->value.c, &bb->value.c);
}

typedef struct {
    flo_TestStruct testStruct;
    flo_string testJson;
    flo_ExpectedTestResult expected;
    flo_string testName;
} flo_Test;

#define UINT64_MAX_WITHOUT_UL 18446744073709551615
#define UINT64_MAX_PLUS_ONE 18446744073709551616

static flo_Test testJsons[] = {
    {.testStruct.a = {.value = 0, .flags = FLO_JSON_PRESENT},
     .testStruct.b = {.value = 2, .flags = FLO_JSON_PRESENT},
     .testStruct.c = {.value = UINT64_MAX, .flags = FLO_JSON_PRESENT},
     .testJson = FLO_STRING(
         "{\"a\":0,\"b\":2,\"c\":" FLO_STRINGIFY(UINT64_MAX_WITHOUT_UL) "}"),
     .testName = FLO_STRING("All numbers")},
    {.testStruct.a = {.value = 0, .flags = FLO_JSON_PRESENT | FLO_JSON_NULL},
     .testStruct.b = {.value = 2, .flags = FLO_JSON_PRESENT},
     .testStruct.c = {.value = 0, .flags = FLO_JSON_PRESENT | FLO_JSON_NULL},
     .testJson = FLO_STRING("{\"a\":null,\"b\":2,\"c\":null}"),
     .testName = FLO_STRING("Some nulls")},
    {.testStruct.a = {.value = 0, .flags = 0},
     .testStruct.b = {.value = 0, .flags = 0},
     .testStruct.c = {.value = 5, .flags = FLO_JSON_PRESENT},
     .testJson = FLO_STRING("{\"c\":5}"),
     .testName = FLO_STRING("Some not present")},
    {.testStruct.a = {.value = 0, .flags = 0},
     .testStruct.b = {.value = 123000000000000000, .flags = FLO_JSON_PRESENT},
     .testStruct.c = {.value = 0, .flags = FLO_JSON_PRESENT | FLO_JSON_NULL},
     .testJson = FLO_STRING("{\"c\":null,\"b\":123e15}"),
     .testName = FLO_STRING("Mixed")},
    {.testJson = FLO_STRING(
         "{\"a\":0,\"b\":2,\"c\":" FLO_STRINGIFY(UINT64_MAX_PLUS_ONE) "}"),
     .expected = {.result = FLO_DESERIALIZATION_OUT_OF_RANGE,
                  .failingKey = FLO_STRING("c")},
     .testName = FLO_STRING("Out of max range")},
    {.testJson = FLO_STRING("{\"c\":\"fail\",\"b\":1}"),
     .expected = {.result = FLO_DESERIALIZATION_BAD_FORMAT,
                  .failingKey = FLO_STRING("c")},
     .testName = FLO_STRING("Unsupported string")},
    {.testJson = FLO_STRING("{\"c\":3,\"b\":-1}"),
     .expected = {.result = FLO_DESERIALIZATION_BAD_FORMAT,
                  .failingKey = FLO_STRING("b")},
     .testName = FLO_STRING("Unsupported negative integer")},
    {.testJson = FLO_STRING("{\"c\":+34,\"b\":-1}"),
     .expected = {.result = FLO_DESERIALIZATION_BAD_FORMAT,
                  .failingKey = FLO_STRING("c")},
     .testName = FLO_STRING("Unsupported character")},
};
#define FLO_TEST_NUMBER FLO_COUNTOF(testJsons)

static flo_json_schema memberBuffer[] = {
    FLO_JSON_CREATE_SIMPLE_KEY(flo_json_TestStruct, flo_TestStruct, a),
    FLO_JSON_CREATE_SIMPLE_KEY(flo_json_TestStruct, flo_TestStruct, b),
    FLO_JSON_CREATE_SIMPLE_KEY(flo_json_TestStruct, flo_TestStruct, c),
};
FLO_JSON_CREATE_KEYS_ARRAY(memberBuffer, uint64StructMembers);

void flo_testUint64Deserialization(flo_arena scratch) {
    FLO_TEST_TOPIC(FLO_STRING("uint64 deserialization")) {
        for (ptrdiff_t i = 0; i < FLO_TEST_NUMBER; i++) {
            FLO_TEST(testJsons[i].testName) {
                flo_json_TestStruct actualDeserialized = {0};
                flo_json_TestStruct expectedDeserialized;

                flo_json_schema root = FLO_JSON_CREATE_ROOT(
                    FLO_DESERIALIZE_OBJECT, &uint64StructMembers,
                    flo_json_TestStruct);

                expectedDeserialized.flags = FLO_JSON_PRESENT;
                expectedDeserialized.value = testJsons[i].testStruct;
                flo_json_comparisonTest(
                    (flo_json_object *)&actualDeserialized,
                    (flo_json_object *)&expectedDeserialized, root,
                    testJsons[i].testJson, testJsons[i].expected,
                    compareTestStruct, 6, &scratch);
            }
        }
    }
}
