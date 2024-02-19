#include "comparison-test.h"
#include "parse-framework/parse.h"       // for flo_parseStatus
#include "test-framework/expectations.h" // for flo_appendExpectCodeWithString
#include "test-framework/test.h"         // for FLO_TEST_FAILURE, flo_testS...
#include "util/log.h"                    // for FLO_ERROR, FLO_LOG_CHOOSER_...
#include "util/memory/arena.h"
#include "util/text/string.h"
#include "util/types.h"         // for flo_char_d_a, flo_string_max_a
#include "json/deserialize.h"   // for flo_deserializationResultTo...
#include "json/serialize.h"     // for flo_appendJsonObject, flo_j...
#include "json/struct-member.h" // for flo_json_schema
#include "json/types.h"
#include <stddef.h>

void flo_json_comparisonTest(flo_json_object *toDeserializeInto,
                             flo_json_object *expectedDeserialized,
                             flo_json_schema root, flo_string json,
                             flo_ExpectedTestResult expected,
                             jsonStructComparison compareFunction,
                             ptrdiff_t maxNestingLength, flo_arena *perm) {
    flo_parseStatus ps = ((flo_parseStatus){.text = json, .idx = 0});

    flo_char_d_a actualSerialized = {0};
    flo_char_d_a expectedSerialized = {0};

    flo_string *buf = FLO_NEW(perm, flo_string, maxNestingLength);
    flo_string_max_a nestedKeys = {
        .cap = maxNestingLength, .len = 0, .buf = buf};

    flo_json_deserializationResult result = flo_json_parse(
        (flo_json_void *)toDeserializeInto, &nestedKeys, &ps, root);

    if (result != expected.result) {
        if (result == FLO_DESERIALIZATION_SUCCESS) {
            FLO_TEST_FAILURE {
                FLO_ERROR(
                    FLO_STRING("Deserialization succeeded unexpectedly!\n"));
                flo_appendExpectCodeWithString(
                    expected.result,
                    flo_json_deserializationResultToString(expected.result),
                    result, flo_json_deserializationResultToString(result));
                FLO_ERROR(FLO_STRING("Printing input json:\n"));
                FLO_ERROR(json, FLO_NEWLINE);
                FLO_ERROR(FLO_STRING("Printing expected failed key:\t"));
                FLO_ERROR(expected.failingKey, FLO_NEWLINE);
                FLO_ERROR(FLO_STRING("Printing actual json:\n"));
                flo_json_append(&actualSerialized,
                                (flo_json_void *)toDeserializeInto, root, perm);
                FLO_ERROR(
                    FLO_STRING_LEN(actualSerialized.buf, actualSerialized.len),
                    FLO_NEWLINE);
            }
            return;
        }

        if (expected.result == FLO_DESERIALIZATION_SUCCESS) {
            FLO_TEST_FAILURE {
                FLO_ERROR(FLO_STRING("Deserialization failed unexpectedly!\n"));
                flo_appendExpectCodeWithString(
                    expected.result,
                    flo_json_deserializationResultToString(expected.result),
                    result, flo_json_deserializationResultToString(result));
                FLO_ERROR(FLO_STRING("Printing failed key:\t"));
                for (ptrdiff_t i = 0; i < nestedKeys.len; i++) {
                    FLO_ERROR(nestedKeys.buf[i]);
                    if (i == nestedKeys.len - 1) {
                        FLO_ERROR(FLO_STRING("\n"));
                    } else {
                        FLO_ERROR(FLO_STRING("."));
                    }
                }
                FLO_ERROR(FLO_STRING("Printing input json:\n"));
                FLO_ERROR(json, FLO_NEWLINE);
            }
            return;
        }

        FLO_TEST_FAILURE {
            flo_appendExpectCodeWithString(
                expected.result,
                flo_json_deserializationResultToString(expected.result), result,
                flo_json_deserializationResultToString(result));
            FLO_ERROR(FLO_STRING("printing actual toDeserialize json:\n"));
            flo_json_append(&actualSerialized,
                            (flo_json_void *)toDeserializeInto, root, perm);
            FLO_ERROR(
                FLO_STRING_LEN(actualSerialized.buf, actualSerialized.len),
                FLO_NEWLINE);
            FLO_ERROR(FLO_STRING("printing expected toDeserialize json:\n"));
            flo_json_append(&expectedSerialized,
                            (flo_json_void *)expectedDeserialized, root, perm);
            FLO_ERROR(
                FLO_STRING_LEN(expectedSerialized.buf, expectedSerialized.len),
                FLO_NEWLINE);
            FLO_ERROR(FLO_STRING("Printing input json:\n"));
            FLO_ERROR(json, FLO_NEWLINE);

            FLO_ERROR(FLO_STRING("Printing actual failed key:\t"));
            for (ptrdiff_t i = 0; i < nestedKeys.len; i++) {
                FLO_ERROR(nestedKeys.buf[i]);
                if (i == nestedKeys.len - 1) {
                    FLO_ERROR(FLO_STRING("\n"));
                } else {
                    FLO_ERROR(FLO_STRING("."));
                }
            }
            FLO_ERROR(FLO_STRING("Printing expected failed key:\t"));
            FLO_ERROR(expected.failingKey, FLO_NEWLINE);
        }
        return;
    }

    if (result != FLO_DESERIALIZATION_SUCCESS) {
        if (!flo_stringEquals(expected.failingKey,
                              nestedKeys.buf[nestedKeys.len - 1])) {
            FLO_TEST_FAILURE {
                FLO_ERROR(
                    FLO_STRING("Deserialization failed for the wrong key!\n"));
                FLO_ERROR(FLO_STRING("Printing actual failed key:\t"));
                for (ptrdiff_t i = 0; i < nestedKeys.len; i++) {
                    FLO_ERROR(nestedKeys.buf[i]);
                    if (i == nestedKeys.len - 1) {
                        FLO_ERROR(FLO_STRING("\n"));
                    } else {
                        FLO_ERROR(FLO_STRING("."));
                    }
                }
                FLO_ERROR(FLO_STRING("Printing expected failed key:\t"));
                FLO_ERROR(expected.failingKey, FLO_NEWLINE);
                FLO_ERROR(FLO_STRING("Printing input json:\n"));
                FLO_ERROR(json, FLO_NEWLINE);
            }
            return;
        }

        flo_testSuccess();
        return;
    }

    if (compareFunction(toDeserializeInto, expectedDeserialized)) {
        flo_testSuccess();
    } else {
        FLO_TEST_FAILURE {
            FLO_ERROR(FLO_STRING(
                "actual toDeserialize json is not equal to actual json\n"));
            FLO_ERROR(FLO_STRING("printing actual toDeserialize json:\n"));
            flo_json_append(&actualSerialized,
                            (flo_json_void *)toDeserializeInto, root, perm);
            FLO_ERROR(
                FLO_STRING_LEN(actualSerialized.buf, actualSerialized.len),
                FLO_NEWLINE | FLO_FLUSH);
            FLO_ERROR(FLO_STRING("printing expected toDeserialize json:\n"));
            flo_json_append(&expectedSerialized,
                            (flo_json_void *)expectedDeserialized, root, perm);
            FLO_ERROR(
                FLO_STRING_LEN(expectedSerialized.buf, expectedSerialized.len),
                FLO_NEWLINE);
            FLO_ERROR(FLO_STRING("Printing input json:\n"));
            FLO_ERROR(json, FLO_NEWLINE);
        }
    }
}
