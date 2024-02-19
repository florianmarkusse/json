#include "test/struct-deserialization.h"
#include "common.h"                             // for compareJsonString
#include "comparison-test.h"                    // for flo_json_comparisonTest
#include "test-framework/test.h"                // for FLO_TEST, FLO_TEST_T...
#include "test/inside-struct-deserialization.h" // for compareInsideTestStruct
#include "test/test-struct-names.h"             // for insideTestStructMembers
#include "util/macros.h"                        // for FLO_STRINGIFY
#include "util/memory/arena.h"
#include "util/memory/macros.h" // for FLO_COUNTOF
#include "util/text/string.h"   // for FLO_STRING, flo_string
#include "json/deserialize.h"   // for FLO_DESERIALIZATION_...
#include "json/struct-member.h" // for FLO_DESERIALIZE_OBJECT
#include "json/types.h"         // for FLO_JSON_PRESENT
#include <stddef.h>             // for ptrdiff_t
#include <stdint.h>             // for uint8_t

typedef struct flo_TestStruct {
    flo_json_string head;
    flo_json_InsideTestStruct inside;
} flo_TestStruct;

FLO_JSON_CREATE_OBJECT(flo_TestStruct, flo_json_TestStruct);

static bool compareTestStruct(flo_json_object *a, flo_json_object *b) {
    FLO_COMMON_JSON_COMPARE(a, b);

    flo_json_TestStruct *aa = (flo_json_TestStruct *)a;
    flo_json_TestStruct *bb = (flo_json_TestStruct *)b;

    return compareJsonString(&aa->value.head, &bb->value.head) &&
           compareInsideTestStruct(&aa->value.inside, &bb->value.inside);
}

typedef struct {
    flo_TestStruct testStruct;
    flo_string testJson;
    flo_ExpectedTestResult expected;
    flo_string testName;
    ptrdiff_t maxNestingLength;
    uint8_t flags;
} flo_Test;

#define DEFAULT_STRING FLO_STRING("d")

static flo_Test testJsons[] = {
    {.testStruct.head = {.value = FLO_STRING("my-start"),
                         .flags = FLO_JSON_PRESENT},
     .testStruct.inside.value.aa = {.value = FLO_STRING("abc"),
                                    .flags = FLO_JSON_PRESENT},
     .testStruct.inside.value.bb = {.value = 2, .flags = FLO_JSON_PRESENT},
     .testStruct.inside.flags = FLO_JSON_PRESENT,
     .testJson = FLO_STRING("{"
                            "\"head\":\"my-start\","
                            "\"inside\":{\"aa\":\"abc\",\"bb\":2}"
                            "}"),
     .flags = FLO_JSON_PRESENT,
     .maxNestingLength = 5,
     .testName = FLO_STRING("Simple struct inside struct")},
    {.testStruct.head = {.value = FLO_STRING("my-start"),
                         .flags = FLO_JSON_PRESENT},
     .testStruct.inside.value.aa = {.value = FLO_STRING("abc"),
                                    .flags = FLO_JSON_PRESENT},
     .testStruct.inside.value.bb = {.value = 2, .flags = FLO_JSON_PRESENT},
     .testStruct.inside.flags = FLO_JSON_PRESENT,
     .testJson = FLO_STRING("{"
                            "\"head\":\"my-start\","
                            "\"inside\":{\"aa\":\"abc\",\"bb\":2}"
                            "}"),
     .flags = FLO_JSON_PRESENT,
     .expected = {.result = FLO_DESERIALIZATION_MAX_NESTING,
                  .failingKey = FLO_STRING("inside")},
     .maxNestingLength = 2,
     .testName = FLO_STRING("Simple struct inside struct hitting max length")},
    {.testJson = FLO_STRING("{}"),
     .flags = FLO_JSON_PRESENT,
     .maxNestingLength = 5,
     .testName = FLO_STRING("Completely exmpty struct object")},
    {.testStruct.head = {.value = FLO_STRING("my-start"),
                         .flags = FLO_JSON_PRESENT},
     .testStruct.inside.flags = FLO_JSON_PRESENT,
     .testJson = FLO_STRING("{"
                            "\"head\":\"my-start\","
                            "\"inside\":{}"
                            "}"),
     .flags = FLO_JSON_PRESENT,
     .maxNestingLength = 5,
     .testName = FLO_STRING("Empty struct object inside struct")},
    {.flags = FLO_JSON_PRESENT | FLO_JSON_NULL,
     .testName = FLO_STRING("struct is null, i.e., json: \"null\""),
     .maxNestingLength = 5,
     .testJson = FLO_STRING("null")},
    {.flags = 0,
     .expected = {.result = FLO_DESERIALIZATION_BAD_FORMAT,
                  .failingKey = FLO_STRING(FLO_STRINGIFY(flo_json_TestStruct))},
     .maxNestingLength = 5,
     .testName = FLO_STRING("empty string")},
};
#define FLO_TEST_NUMBER FLO_COUNTOF(testJsons)

static flo_json_schema memberBuffer[] = {
    FLO_JSON_CREATE_SIMPLE_KEY(flo_json_TestStruct, flo_TestStruct, head),
    FLO_JSON_CREATE_OBJECT_KEY(flo_json_TestStruct, flo_TestStruct, inside,
                               &insideTestStructMembers,
                               flo_json_InsideTestStruct),
};
FLO_JSON_CREATE_KEYS_ARRAY(memberBuffer, structStructMembers);

void flo_testStructDeserialization(flo_arena scratch) {
    FLO_TEST_TOPIC(FLO_STRING("struct deserialization")) {
        for (ptrdiff_t i = 0; i < FLO_TEST_NUMBER; i++) {
            FLO_TEST(testJsons[i].testName) {
                flo_json_TestStruct actualDeserialized = {0};
                flo_json_TestStruct expectedDeserialized;
                expectedDeserialized.flags = testJsons[i].flags;
                expectedDeserialized.value = testJsons[i].testStruct;

                flo_json_schema root = FLO_JSON_CREATE_ROOT(
                    FLO_DESERIALIZE_OBJECT, &structStructMembers,
                    flo_json_TestStruct);

                flo_json_comparisonTest(
                    (flo_json_object *)&actualDeserialized,
                    (flo_json_object *)&expectedDeserialized, root,
                    testJsons[i].testJson, testJsons[i].expected,
                    compareTestStruct, testJsons[i].maxNestingLength, &scratch);
            }
        }
    }
}
