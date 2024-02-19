#include "test-framework/expectations.h"
#include "util/log.h"

void flo_appendExpectCodeWithString(ptrdiff_t expected,
                                    flo_string expectedString, ptrdiff_t actual,
                                    flo_string actualString) {
    FLO_ERROR(flo_stringWithMinSizeDefault(FLO_STRING("Expected"), 10));
    FLO_ERROR((FLO_STRING(": ")));
    FLO_ERROR(
        flo_stringWithMinSizeDefault(flo_ptrdiffToStringDefault(expected), 4));
    FLO_ERROR((FLO_STRING(" - ")));
    FLO_ERROR(expectedString, FLO_NEWLINE);

    FLO_ERROR(flo_stringWithMinSizeDefault(FLO_STRING("Actual"), 10));
    FLO_ERROR((FLO_STRING(": ")));
    FLO_ERROR(
        flo_stringWithMinSizeDefault(flo_ptrdiffToStringDefault(actual), 4));
    FLO_ERROR((FLO_STRING(" - ")));
    FLO_ERROR(actualString, FLO_NEWLINE);
}

void flo_appendExpectString(flo_string expectedString,
                            flo_string actualString) {
    FLO_ERROR(flo_stringWithMinSizeDefault(FLO_STRING("Expected string"), 20));
    FLO_ERROR((FLO_STRING(": ")));
    FLO_ERROR(expectedString, FLO_NEWLINE);

    FLO_ERROR(flo_stringWithMinSizeDefault(FLO_STRING("Actual string"), 20));
    FLO_ERROR((FLO_STRING(": ")));
    FLO_ERROR(actualString, FLO_NEWLINE);
}

void flo_appendExpectBool(bool expectedBool, bool actualBool) {
    FLO_ERROR(flo_stringWithMinSizeDefault(FLO_STRING("Expected bool"), 20));
    FLO_ERROR((FLO_STRING(": ")));
    FLO_ERROR(expectedBool, FLO_NEWLINE);

    FLO_ERROR(flo_stringWithMinSizeDefault(FLO_STRING("Actual bool"), 20));
    FLO_ERROR((FLO_STRING(": ")));
    FLO_ERROR(actualBool, FLO_NEWLINE);
}

void flo_appendExpectPtrDiff(ptrdiff_t expectedNumber, ptrdiff_t actualNumber) {
    FLO_ERROR(flo_stringWithMinSizeDefault(FLO_STRING("Expected number"), 20));
    FLO_ERROR((FLO_STRING(": ")));
    FLO_ERROR(expectedNumber, FLO_NEWLINE);

    FLO_ERROR(flo_stringWithMinSizeDefault(FLO_STRING("Actual number"), 20));
    FLO_ERROR((FLO_STRING(": ")));
    FLO_ERROR(actualNumber, FLO_NEWLINE);
}
void flo_appendExpectUint(uint64_t expectedNumber, uint64_t actualNumber) {
    FLO_ERROR(flo_stringWithMinSizeDefault(FLO_STRING("Expected number"), 20));
    FLO_ERROR((FLO_STRING(": ")));
    FLO_ERROR(expectedNumber, FLO_NEWLINE);

    FLO_ERROR(flo_stringWithMinSizeDefault(FLO_STRING("Actual number"), 20));
    FLO_ERROR((FLO_STRING(": ")));
    FLO_ERROR(actualNumber, FLO_NEWLINE);
}
