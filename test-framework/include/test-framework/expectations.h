#ifndef TEST_FRAMEWORK_EXPECTATIONS_H
#define TEST_FRAMEWORK_EXPECTATIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "util/text/string.h" // for flo_string
#include <stddef.h>           // for ptrdiff_t
#include <stdint.h>           // for uint64_t

void flo_appendExpectCodeWithString(ptrdiff_t expected,
                                    flo_string expectedString, ptrdiff_t actual,
                                    flo_string actualString);
void flo_appendExpectString(flo_string expectedString, flo_string actualString);
void flo_appendExpectBool(bool expectedBool, bool actualBool);
void flo_appendExpectPtrDiff(ptrdiff_t expectedNumber, ptrdiff_t actualNumber);
void flo_appendExpectUint(uint64_t expectedNumber, uint64_t actualNumber);

#ifdef __cplusplus
}
#endif

#endif
