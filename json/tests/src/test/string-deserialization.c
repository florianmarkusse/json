#include "test/string-deserialization.h"
#include "common.h"              // for compareJsonString, FLO_COMMON_JSON_...
#include "comparison-test.h"     // for flo_json_comparisonTest, flo_Expect...
#include "test-framework/test.h" // for FLO_TEST, FLO_TEST_TOPIC
#include "util/memory/arena.h"
#include "util/memory/macros.h" // for FLO_COUNTOF
#include "util/text/string.h"   // for FLO_STRING, flo_string
#include "json/deserialize.h"   // for FLO_DESERIALIZATION_BAD_FORMAT
#include "json/struct-member.h" // for FLO_JSON_CREATE_STRUCT_MEMBER, FLO_...
#include "json/types.h"         // for FLO_JSON_PRESENT, FLO_JSON_NULL
#include <stddef.h>             // for ptrdiff_t

typedef struct flo_TestStruct {
    flo_json_string a;
    flo_json_string b;
    flo_json_string c;
} flo_TestStruct;

FLO_JSON_CREATE_OBJECT(flo_TestStruct, flo_json_TestStruct);

static bool compareTestStruct(flo_json_object *a, flo_json_object *b) {
    FLO_COMMON_JSON_COMPARE(a, b);

    flo_json_TestStruct *aa = (flo_json_TestStruct *)a;
    flo_json_TestStruct *bb = (flo_json_TestStruct *)b;

    return compareJsonString(&aa->value.a, &bb->value.a) &&
           compareJsonString(&aa->value.b, &bb->value.b) &&
           compareJsonString(&aa->value.c, &bb->value.c);
}

typedef struct {
    flo_TestStruct testStruct;
    flo_string testJson;
    flo_ExpectedTestResult expected;
    flo_string testName;
} flo_Test;

#define DEFAULT_STRING FLO_STRING("d")

static flo_Test testJsons[] = {
    {.testStruct.a = {.value = FLO_STRING("abc"), .flags = FLO_JSON_PRESENT},
     .testStruct.b = {.value = FLO_STRING("abc"), .flags = FLO_JSON_PRESENT},
     .testStruct.c = {.value = FLO_STRING("cba"), .flags = FLO_JSON_PRESENT},
     .testJson = FLO_STRING("{\"a\":\"abc\",\"b\":\"abc\",\"c\":\"cba\"}"),
     .testName = FLO_STRING("All strings")},
    {.testStruct.a = {.value = {0}, .flags = FLO_JSON_PRESENT | FLO_JSON_NULL},
     .testStruct.b = {.value = FLO_STRING("abc"), .flags = FLO_JSON_PRESENT},
     .testStruct.c = {.value = {0}, .flags = FLO_JSON_PRESENT | FLO_JSON_NULL},
     .testJson = FLO_STRING("{\"a\":null,\"b\":\"abc\",\"c\":null}"),
     .testName = FLO_STRING("Some nulls")},
    {.testStruct.a = {.value = {0}, .flags = 0},
     .testStruct.b = {.value = {0}, .flags = 0},
     .testStruct.c = {.value = FLO_STRING("cba"), .flags = FLO_JSON_PRESENT},
     .testJson = FLO_STRING("{\"c\":\"cba\"}"),
     .testName = FLO_STRING("Some not present")},
    {.testStruct.a = {.value = {0}, .flags = 0},
     .testStruct.b = {.value = FLO_STRING("abc"), .flags = FLO_JSON_PRESENT},
     .testStruct.c = {.value = {0}, .flags = FLO_JSON_PRESENT | FLO_JSON_NULL},
     .testJson = FLO_STRING("{\"c\":null,\"b\":\"abc\"}"),
     .testName = FLO_STRING("All mixed")},
    {.testStruct.a = {.value = {0}, .flags = 0},
     .testStruct.b = {.value = FLO_STRING("abc"), .flags = FLO_JSON_PRESENT},
     .testStruct.c = {.value = {0}, .flags = FLO_JSON_PRESENT | FLO_JSON_NULL},
     .testJson = FLO_STRING("{\"c\":null,\"b\":abc}"),
     .expected = {.result = FLO_DESERIALIZATION_BAD_FORMAT,
                  .failingKey = FLO_STRING("b")},
     .testName = FLO_STRING("without quotes")},
    {.testStruct.a = {.value = {0}, .flags = 0},
     .testStruct.b = {.value = FLO_STRING("abc"), .flags = FLO_JSON_PRESENT},
     .testStruct.c = {.value = {0}, .flags = FLO_JSON_PRESENT | FLO_JSON_NULL},
     .testJson = FLO_STRING("{\"c\":null,\"b\":\"abc}"),
     .expected = {.result = FLO_DESERIALIZATION_BAD_FORMAT,
                  .failingKey = FLO_STRING("b")},
     .testName = FLO_STRING("closing quote missing")},
};
#define FLO_TEST_NUMBER FLO_COUNTOF(testJsons)

static flo_json_schema memberBuffer[] = {
    FLO_JSON_CREATE_SIMPLE_KEY(flo_json_TestStruct, flo_TestStruct, a),
    FLO_JSON_CREATE_SIMPLE_KEY(flo_json_TestStruct, flo_TestStruct, b),
    FLO_JSON_CREATE_SIMPLE_KEY(flo_json_TestStruct, flo_TestStruct, c),
};
FLO_JSON_CREATE_KEYS_ARRAY(memberBuffer, stringStructMembers);

void flo_testStringDeserialization(flo_arena scratch) {
    FLO_TEST_TOPIC(FLO_STRING("string deserialization")) {
        for (ptrdiff_t i = 0; i < FLO_TEST_NUMBER; i++) {
            FLO_TEST(testJsons[i].testName) {
                flo_json_TestStruct actualDeserialized = {0};
                flo_json_TestStruct expectedDeserialized;
                expectedDeserialized.flags = FLO_JSON_PRESENT;
                expectedDeserialized.value = testJsons[i].testStruct;

                flo_json_schema root = FLO_JSON_CREATE_ROOT(
                    FLO_DESERIALIZE_OBJECT, &stringStructMembers,
                    flo_json_TestStruct);

                flo_json_comparisonTest(
                    (flo_json_object *)&actualDeserialized,
                    (flo_json_object *)&expectedDeserialized, root,
                    testJsons[i].testJson, testJsons[i].expected,
                    compareTestStruct, 5, &scratch);
            }
        }
    }
}
