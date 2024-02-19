#include "test/integration-deserialization.h"
#include "common.h"                             // for compareJsonString
#include "comparison-test.h"                    // for flo_json_comparisonTest
#include "test-framework/test.h"                // for FLO_TEST, FLO_TEST_T...
#include "test/inside-struct-deserialization.h" // for flo_json_InsideTestS...
#include "test/test-struct-names.h"             // for insideTestStructMembers
#include "util/memory/arena.h"                  // for flo_arena
#include "util/memory/macros.h"                 // for FLO_COUNTOF
#include "util/text/string.h"                   // for FLO_STRING, flo_string
#include "json/struct-member.h"                 // for FLO_JSON_CREATE_SIMP...
#include "json/types.h"                         // for flo_json_object, FLO...
#include <stddef.h>                             // for ptrdiff_t

typedef struct flo_AandB {
    flo_json_string a;
    flo_json_string b;
} flo_AandB;

FLO_JSON_CREATE_OBJECT(flo_AandB, flo_json_AandB);

bool compareFloAAndB(flo_json_AandB *a, flo_json_AandB *b) {
    FLO_COMMON_JSON_COMPARE(a, b);
    return compareJsonString(&a->value.a, &b->value.a) &&
           compareJsonString(&a->value.b, &b->value.b);
}

static flo_json_schema ABmemberBuffer[] = {
    FLO_JSON_CREATE_SIMPLE_KEY(flo_json_AandB, flo_AandB, a),
    FLO_JSON_CREATE_SIMPLE_KEY(flo_json_AandB, flo_AandB, b)};
FLO_JSON_CREATE_KEYS_ARRAY(ABmemberBuffer, insideAandBMembers);

typedef struct flo_OneLevelStruct {
    flo_json_InsideTestStruct firstInside;
    flo_json_InsideTestStruct secondInside;
    flo_json_string_array_max_a strArr1;
    flo_json_object_array_max_a objArr;
} flo_OneLevelStruct;
FLO_JSON_CREATE_OBJECT(flo_OneLevelStruct, flo_json_OneLevelStruct);

static flo_json_schema oneLevelMemberBuffer[] = {
    FLO_JSON_CREATE_OBJECT_KEY(flo_json_OneLevelStruct, flo_OneLevelStruct,
                               firstInside, &insideTestStructMembers,
                               flo_json_InsideTestStruct),
    FLO_JSON_CREATE_OBJECT_KEY(flo_json_OneLevelStruct, flo_OneLevelStruct,
                               secondInside, &insideTestStructMembers,
                               flo_json_InsideTestStruct),
    FLO_JSON_CREATE_SIMPLE_ARRAY_KEY(
        flo_json_OneLevelStruct, flo_OneLevelStruct, strArr1, flo_json_string),
    FLO_JSON_CREATE_OBJECT_KEY(flo_json_OneLevelStruct, flo_OneLevelStruct,
                               objArr, &insideAandBMembers, flo_json_AandB),
};
FLO_JSON_CREATE_KEYS_ARRAY(oneLevelMemberBuffer, oneLevelStructMembers);

bool compareOneLevelStruct(flo_json_OneLevelStruct *a,
                           flo_json_OneLevelStruct *b) {
    FLO_COMMON_JSON_COMPARE(a, b);

    FLO_COMMON_JSON_COMPARE(&a->value.objArr, &b->value.objArr);
    if (a->value.objArr.value.len != b->value.objArr.value.len) {
        return false;
    }

    for (ptrdiff_t i = 0; i < a->value.objArr.value.len; i++) {
        flo_json_AandB *fromA =
            (flo_json_AandB *)((char *)a->value.objArr.value.buf +
                               sizeof(flo_json_AandB) * i);
        flo_json_AandB *fromB =
            (flo_json_AandB *)((char *)b->value.objArr.value.buf +
                               sizeof(flo_json_AandB) * i);

        if (!compareFloAAndB(fromA, fromB)) {
            return false;
        }
    }

    return compareInsideTestStruct(&a->value.firstInside,
                                   &b->value.firstInside) &&
           compareInsideTestStruct(&a->value.secondInside,
                                   &b->value.secondInside) &&
           flo_json_compareJsonStringArray(&a->value.strArr1,
                                           &b->value.strArr1);
}

typedef struct flo_TestStruct {
    flo_json_string a;
    flo_json_string ab;
    flo_json_uint64 b;
    flo_json_OneLevelStruct aa;
    flo_json_string_array_max_a strArr2;
    flo_json_bool_array_max_a boolarr;
    flo_json_uint64_array_max_a uintarr;
} flo_TestStruct;

FLO_JSON_CREATE_OBJECT(flo_TestStruct, flo_json_TestStruct);

static flo_json_schema memberBuffer[] = {
    FLO_JSON_CREATE_SIMPLE_KEY(flo_json_TestStruct, flo_TestStruct, a),
    FLO_JSON_CREATE_SIMPLE_KEY(flo_json_TestStruct, flo_TestStruct, ab),
    FLO_JSON_CREATE_SIMPLE_KEY(flo_json_TestStruct, flo_TestStruct, b),
    FLO_JSON_CREATE_OBJECT_KEY(flo_json_TestStruct, flo_TestStruct, aa,
                               &oneLevelStructMembers, flo_json_OneLevelStruct),
    FLO_JSON_CREATE_SIMPLE_ARRAY_KEY(flo_json_TestStruct, flo_TestStruct,
                                     strArr2, flo_json_string),
    FLO_JSON_CREATE_SIMPLE_ARRAY_KEY(flo_json_TestStruct, flo_TestStruct,
                                     boolarr, flo_json_bool),
    FLO_JSON_CREATE_SIMPLE_ARRAY_KEY(flo_json_TestStruct, flo_TestStruct,
                                     uintarr, flo_json_uint64),
};
FLO_JSON_CREATE_KEYS_ARRAY(memberBuffer, integrationStructMembers);

static bool compareTestStruct(flo_json_object *a, flo_json_object *b) {
    flo_json_TestStruct *aa = (flo_json_TestStruct *)a;
    flo_json_TestStruct *bb = (flo_json_TestStruct *)b;

    return compareJsonString(&aa->value.a, &bb->value.a) &&
           compareJsonUint64(&aa->value.b, &bb->value.b) &&
           compareOneLevelStruct(&aa->value.aa, &bb->value.aa) &&
           flo_json_compareJsonStringArray(&aa->value.strArr2,
                                           &bb->value.strArr2) &&
           flo_json_compareJsonBoolArray(&aa->value.boolarr,
                                         &bb->value.boolarr) &&
           flo_json_compareJsonUint64Array(&aa->value.uintarr,
                                           &bb->value.uintarr);
}

typedef struct {
    flo_TestStruct testStruct;
    flo_string testJson;
    flo_ExpectedTestResult expected;
    flo_string testName;
} flo_Test;

static flo_json_string stringTestBuffer[] = {
    {.flags = FLO_JSON_PRESENT, .value = FLO_STRING("first")},
    {.flags = FLO_JSON_PRESENT, .value = FLO_STRING("second")},
    {.flags = FLO_JSON_PRESENT, .value = FLO_STRING("third")},
};

static flo_json_AandB toActuallyFill[] = {
    {.flags = FLO_JSON_NULL | FLO_JSON_PRESENT},
    {.flags = FLO_JSON_PRESENT,
     .value = {.a = {.value = FLO_STRING("hi there"),
                     .flags = FLO_JSON_PRESENT},
               .b = {.value = FLO_STRING("hgdfhg"),
                     .flags = FLO_JSON_PRESENT | FLO_JSON_NULL}}},
};

static flo_json_bool boolTestBuffer[] = {
    {.flags = FLO_JSON_PRESENT, .value = true},
    {.flags = FLO_JSON_PRESENT | FLO_JSON_NULL, .value = true},
    {.flags = FLO_JSON_PRESENT | FLO_JSON_NULL, .value = true},
    {.flags = FLO_JSON_PRESENT, .value = false},
    {.flags = FLO_JSON_PRESENT, .value = true},
};

static flo_json_uint64 uint64TestBuffer[] = {
    {.flags = FLO_JSON_PRESENT, .value = 1},
    {.flags = FLO_JSON_PRESENT, .value = 3000},
    {.flags = FLO_JSON_PRESENT | FLO_JSON_NULL, .value = 2},
};

static flo_Test testJsons[] = {
    {.testStruct.a = {.value = FLO_STRING("abc"), .flags = FLO_JSON_PRESENT},
     .testStruct.b = {.value = 123, .flags = FLO_JSON_PRESENT},
     .testStruct.aa.flags = FLO_JSON_PRESENT,
     .testStruct.aa.value.firstInside.flags = FLO_JSON_PRESENT,
     .testStruct.aa.value.firstInside.value.aa = {.value = FLO_STRING("test-a"),
                                                  .flags = FLO_JSON_PRESENT},
     .testStruct.aa.value.firstInside.value.bb = {.value = 0,
                                                  .flags = FLO_JSON_PRESENT |
                                                           FLO_JSON_NULL},
     .testStruct.aa.value.secondInside.flags = FLO_JSON_PRESENT | FLO_JSON_NULL,
     .testStruct.aa.value.strArr1.flags = FLO_JSON_PRESENT,
     .testStruct.aa.value.strArr1.value.buf = stringTestBuffer,
     .testStruct.aa.value.strArr1.value.len = FLO_COUNTOF(stringTestBuffer),
     .testStruct.aa.value.strArr1.value.cap = FLO_COUNTOF(stringTestBuffer),
     .testStruct.aa.value.objArr.flags = FLO_JSON_PRESENT,
     .testStruct.aa.value.objArr.value.buf = (flo_json_object *)toActuallyFill,
     .testStruct.aa.value.objArr.value.len = FLO_COUNTOF(toActuallyFill),
     .testStruct.aa.value.objArr.value.cap = FLO_COUNTOF(toActuallyFill),
     .testStruct.strArr2.flags = FLO_JSON_PRESENT | FLO_JSON_NULL,
     .testStruct.boolarr.flags = FLO_JSON_PRESENT,
     .testStruct.boolarr.value.buf = boolTestBuffer,
     .testStruct.boolarr.value.len = FLO_COUNTOF(boolTestBuffer),
     .testStruct.boolarr.value.cap = FLO_COUNTOF(boolTestBuffer),
     .testStruct.uintarr.flags = FLO_JSON_PRESENT,
     .testStruct.uintarr.value.buf = uint64TestBuffer,
     .testStruct.uintarr.value.len = FLO_COUNTOF(uint64TestBuffer),
     .testStruct.uintarr.value.cap = FLO_COUNTOF(uint64TestBuffer),
     .testJson =
         FLO_STRING("{"
                    "\"boolarr\":[true,null,null,false,true],"
                    "\"a\":\"abc\","
                    "\"b\":123,"
                    "\"aa\":"
                    "{"
                    "\"strArr1\":"
                    "["
                    "\"first\",\"second\",\"third\""
                    "],"
                    "\"secondInside\": null,"
                    "\"objArr\": [ null, { \"a\": \"hi there\", \"b\": null} ],"
                    "\"aa\":\"test-a\","
                    "\"firstInside\":"
                    "{"
                    "\"aa\": \"test-a\","
                    "\"bb\": null"
                    "},"
                    "\"bb\":666"
                    "},"
                    "\"uintarr\":[1,            3000,               null],"
                    "\"strArr2\":null"
                    "}"),
     .testName = FLO_STRING("test")},
};
#define FLO_TEST_NUMBER FLO_COUNTOF(testJsons)

void flo_testJsonDeserializationIntegration(flo_arena scratch) {
    FLO_TEST_TOPIC(FLO_STRING("integration deserialization")) {
        for (ptrdiff_t i = 0; i < FLO_TEST_NUMBER; i++) {
            FLO_TEST(testJsons[i].testName) {
                flo_json_string stringsForOneLevel[3];
                flo_json_AandB objectsForBoolArr[2];
                flo_json_string stringsForStrArr2[3];
                flo_json_bool boolsForBoolarr[5];
                flo_json_uint64 uintsForuintarr[3];

                flo_json_TestStruct actualDeserialized = {
                    .value = {.aa = {.value = {.strArr1 =
                                                   {.value =
                                                        {.buf =
                                                             stringsForOneLevel,
                                                         .cap = 3,
                                                         .len = 0}},
                                               .objArr = {.value =
                                                              {
                                                                  .buf = (flo_json_object
                                                                              *)objectsForBoolArr,
                                                                  .cap = 3,
                                                                  .len = 0}}

                                     }},
                              .strArr2 = {.value = {.buf = stringsForStrArr2,
                                                    .cap = 3,
                                                    .len = 0}},
                              .boolarr = {.value = {.buf = boolsForBoolarr,
                                                    .cap = 5,
                                                    .len = 0}},
                              .uintarr = {.value = {.buf = uintsForuintarr,
                                                    .cap = 3,
                                                    .len = 0}}}};

                flo_json_TestStruct expectedDeserialized;
                expectedDeserialized.flags = FLO_JSON_PRESENT;
                expectedDeserialized.value = testJsons[i].testStruct;

                flo_json_schema root = FLO_JSON_CREATE_ROOT(
                    FLO_DESERIALIZE_OBJECT, &integrationStructMembers,
                    flo_json_TestStruct);

                flo_json_comparisonTest(
                    (flo_json_object *)&actualDeserialized,
                    (flo_json_object *)&expectedDeserialized, root,
                    testJsons[i].testJson, testJsons[i].expected,
                    compareTestStruct, 6, &scratch);
            }
        }
    }
}
