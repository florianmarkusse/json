#include "test/unit.h"
#include "common.h"                      // for compareJsonBool, compareJso...
#include "parse-framework/parse.h"       // for flo_parseStatus
#include "test-framework/expectations.h" // for flo_appendExpectCodeWithString
#include "test-framework/test.h"         // for flo_testSuccess, FLO_TEST
#include "util/log.h"                    // for FLO_ERROR, FLO_LOG_CHOOSER_...
#include "util/memory/arena.h"
#include "util/text/string.h"   // for FLO_STRING, flo_string, FLO...
#include "util/types.h"         // for flo_string_max_a, flo_char_d_a
#include "json/deserialize.h"   // for flo_deserializationResultTo...
#include "json/serialize.h"     // for flo_appendJsonObject, flo_a...
#include "json/struct-member.h" // for FLO_JSON_CREATE_ROOT, flo_S...
#include "json/types.h"         // for FLO_JSON_PRESENT, flo_json_...

typedef struct flo_EmptyStruct {
} flo_EmptyStruct;

FLO_JSON_CREATE_OBJECT(flo_EmptyStruct, flo_json_EmptyStruct);

void flo_testUnitDeserialization(flo_arena scratch) {
    flo_char_d_a actualBuffer = {0};
    flo_char_d_a expectedBuffer = {0};

    FLO_TEST_TOPIC(FLO_STRING("unit deserialization")) {
        FLO_TEST(FLO_STRING("bool")) {
            flo_json_bool actualDeserialized = {0};
            flo_json_bool expectedDeserialized = {.flags = FLO_JSON_PRESENT,
                                                  .value = true};

            flo_string json = FLO_STRING("true");
            flo_parseStatus ps = ((flo_parseStatus){.text = json, .idx = 0});
            flo_json_schema root =
                FLO_JSON_CREATE_ROOT(FLO_DESERIALIZE_BOOL, flo_json_bool);

            flo_string *buf = FLO_NEW(&scratch, flo_string, 1);
            flo_string_max_a nestedKeys = {.cap = 1, .len = 0, .buf = buf};
            flo_json_deserializationResult result = flo_json_parse(
                (flo_json_void *)&actualDeserialized, &nestedKeys, &ps, root);

            if (result == FLO_DESERIALIZATION_SUCCESS &&
                compareJsonBool(&actualDeserialized, &expectedDeserialized)) {
                flo_testSuccess();
            } else {
                FLO_TEST_FAILURE {
                    flo_appendExpectCodeWithString(
                        FLO_DESERIALIZATION_SUCCESS,
                        flo_json_deserializationResultToString(
                            FLO_DESERIALIZATION_SUCCESS),
                        result, flo_json_deserializationResultToString(result));
                    FLO_ERROR(FLO_STRING("actual value of bool: "));
                    FLO_ERROR(actualDeserialized.value);
                    FLO_ERROR(FLO_STRING(" "));
                    FLO_ERROR(FLO_STRING("expected value of bool: "));
                    FLO_ERROR(expectedDeserialized.value);
                    FLO_ERROR(FLO_STRING("\n"));
                }
            }
        }

        FLO_TEST(FLO_STRING("uint64")) {
            flo_json_uint64 actualDeserialized = {0};
            flo_json_uint64 expectedDeserialized = {.flags = FLO_JSON_PRESENT,
                                                    .value = 35};

            flo_string json = FLO_STRING("35");
            flo_parseStatus ps = ((flo_parseStatus){.text = json, .idx = 0});
            flo_json_schema root =
                FLO_JSON_CREATE_ROOT(FLO_DESERIALIZE_UINT64, flo_json_uint64);

            flo_string *buf = FLO_NEW(&scratch, flo_string, 1);
            flo_string_max_a nestedKeys = {.cap = 1, .len = 0, .buf = buf};
            flo_json_deserializationResult result = flo_json_parse(
                (flo_json_void *)&actualDeserialized, &nestedKeys, &ps, root);

            if (result == FLO_DESERIALIZATION_SUCCESS &&
                compareJsonUint64(&actualDeserialized, &expectedDeserialized)) {
                flo_testSuccess();
            } else {
                FLO_TEST_FAILURE {
                    flo_appendExpectCodeWithString(
                        FLO_DESERIALIZATION_SUCCESS,
                        flo_json_deserializationResultToString(
                            FLO_DESERIALIZATION_SUCCESS),
                        result, flo_json_deserializationResultToString(result));
                    FLO_ERROR(FLO_STRING("actual value of uint64: "));
                    FLO_ERROR(actualDeserialized.value);
                    FLO_ERROR(FLO_STRING("expected value of uint64: "));
                    FLO_ERROR(expectedDeserialized.value);
                }
            }
        }

        FLO_TEST(FLO_STRING("string")) {
            flo_json_string actualDeserialized = {0};
            flo_json_string expectedDeserialized = {
                .flags = FLO_JSON_PRESENT, .value = FLO_STRING("hello")};

            flo_string json = FLO_STRING("\"hello\"");
            flo_parseStatus ps = ((flo_parseStatus){.text = json, .idx = 0});
            flo_json_schema root =
                FLO_JSON_CREATE_ROOT(FLO_DESERIALIZE_STRING, flo_json_string);

            flo_string *buf = FLO_NEW(&scratch, flo_string, 1);
            flo_string_max_a nestedKeys = {.cap = 1, .len = 0, .buf = buf};
            flo_json_deserializationResult result = flo_json_parse(
                (flo_json_void *)&actualDeserialized, &nestedKeys, &ps, root);

            if (result == FLO_DESERIALIZATION_SUCCESS &&
                compareJsonString(&actualDeserialized, &expectedDeserialized)) {
                flo_testSuccess();
            } else {
                FLO_TEST_FAILURE {
                    flo_appendExpectCodeWithString(
                        FLO_DESERIALIZATION_SUCCESS,
                        flo_json_deserializationResultToString(
                            FLO_DESERIALIZATION_SUCCESS),
                        result, flo_json_deserializationResultToString(result));
                    FLO_ERROR(FLO_STRING("actual value of string: "));
                    flo_json_append(&actualBuffer,
                                    (flo_json_void *)&actualDeserialized, root,
                                    &scratch);
                    FLO_ERROR(
                        FLO_STRING_LEN(actualBuffer.buf, actualBuffer.len),
                        FLO_NEWLINE);
                    FLO_ERROR(FLO_STRING("expected value of string: "));
                    flo_json_append(&expectedBuffer,
                                    (flo_json_void *)&expectedDeserialized,
                                    root, &scratch);
                    FLO_ERROR(
                        FLO_STRING_LEN(expectedBuffer.buf, expectedBuffer.len),
                        FLO_NEWLINE);
                }
            }
        }

        FLO_TEST(FLO_STRING("object")) {
            flo_json_EmptyStruct actualDeserialized;
            flo_json_EmptyStruct expectedDeserialized = {
                .flags = FLO_JSON_PRESENT, .value = {}};

            flo_string json = FLO_STRING("{}");
            flo_parseStatus ps = ((flo_parseStatus){.text = json, .idx = 0});

            struct flo_json_schema_a empty = {0};
            flo_json_schema root = FLO_JSON_CREATE_ROOT(
                FLO_DESERIALIZE_OBJECT, &empty, flo_json_EmptyStruct);

            flo_string *buf = FLO_NEW(&scratch, flo_string, 1);
            flo_string_max_a nestedKeys = {.cap = 1, .len = 0, .buf = buf};
            flo_json_deserializationResult result = flo_json_parse(
                (flo_json_void *)&actualDeserialized, &nestedKeys, &ps, root);

            if (result == FLO_DESERIALIZATION_SUCCESS &&
                actualDeserialized.flags == expectedDeserialized.flags) {
                flo_testSuccess();
            } else {
                FLO_TEST_FAILURE {
                    flo_appendExpectCodeWithString(
                        FLO_DESERIALIZATION_SUCCESS,
                        flo_json_deserializationResultToString(
                            FLO_DESERIALIZATION_SUCCESS),
                        result, flo_json_deserializationResultToString(result));
                    FLO_ERROR(FLO_STRING("actual value of object: "));
                    flo_json_append(&actualBuffer,
                                    (flo_json_void *)&actualDeserialized, root,
                                    &scratch);
                    FLO_ERROR(
                        FLO_STRING_LEN(actualBuffer.buf, actualBuffer.len),
                        FLO_NEWLINE);
                    FLO_ERROR(FLO_STRING("expected value of object: "));
                    flo_json_append(&expectedBuffer,
                                    (flo_json_void *)&expectedDeserialized,
                                    root, &scratch);
                    FLO_ERROR(
                        FLO_STRING_LEN(expectedBuffer.buf, expectedBuffer.len),
                        FLO_NEWLINE);
                }
            }
        }

        FLO_TEST(FLO_STRING("array: string")) {
            flo_json_string resultsString[3];
            flo_json_string_array_max_a actualDeserialized = {
                .value = {.len = 0, .cap = 3, .buf = resultsString}};

            flo_json_string strings[] = {
                {.flags = FLO_JSON_PRESENT, .value = FLO_STRING("first")},
                {.flags = FLO_JSON_NULL | FLO_JSON_PRESENT,
                 .value = FLO_STRING("second")},
                {.flags = FLO_JSON_PRESENT, .value = FLO_STRING("third")},
            };
            flo_json_string_array_max_a expectedDeserialized = {
                .flags = FLO_JSON_PRESENT,
                .value = {.buf = strings, .cap = 3, .len = 3}};

            flo_string json = FLO_STRING("["
                                         "\"first\","
                                         "null,"
                                         "\"third\""
                                         "]");
            flo_parseStatus ps = ((flo_parseStatus){.text = json, .idx = 0});

            flo_json_schema root = FLO_JSON_CREATE_ROOT(
                FLO_DESERIALIZE_STRING_ARRAY, flo_json_string);

            flo_string *buf = FLO_NEW(&scratch, flo_string, 1);
            flo_string_max_a nestedKeys = {.cap = 1, .len = 0, .buf = buf};
            flo_json_deserializationResult result = flo_json_parse(
                (flo_json_void *)&actualDeserialized, &nestedKeys, &ps, root);

            if (result == FLO_DESERIALIZATION_SUCCESS &&
                actualDeserialized.flags == expectedDeserialized.flags) {
                flo_testSuccess();
            } else {
                FLO_TEST_FAILURE {
                    flo_appendExpectCodeWithString(
                        FLO_DESERIALIZATION_SUCCESS,
                        flo_json_deserializationResultToString(
                            FLO_DESERIALIZATION_SUCCESS),
                        result, flo_json_deserializationResultToString(result));
                    FLO_ERROR(FLO_STRING("actual value of string array: "));
                    flo_json_append(&actualBuffer,
                                    (flo_json_void *)&actualDeserialized, root,
                                    &scratch);
                    FLO_ERROR(
                        FLO_STRING_LEN(actualBuffer.buf, actualBuffer.len),
                        FLO_NEWLINE);
                    FLO_ERROR(FLO_STRING("expected value of string array: "));
                    flo_json_append(&expectedBuffer,
                                    (flo_json_void *)&expectedDeserialized,
                                    root, &scratch);
                    FLO_ERROR(
                        FLO_STRING_LEN(expectedBuffer.buf, expectedBuffer.len),
                        FLO_NEWLINE);
                }
            }
        }
    }
}
