#ifndef TEST_FRAMEWORK_TEST_H
#define TEST_FRAMEWORK_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "util/log.h"         // for FLO_ERROR, FLO_FLUSH
#include "util/macros.h"      // for FLO_MACRO_VAR
#include <util/text/string.h> // for flo_string

#define FLO_TEST_FAILURE                                                       \
    for (ptrdiff_t FLO_MACRO_VAR(i) =                                          \
             (flo_testFailure(), flo_appendTestFailureStart(), 0);             \
         FLO_MACRO_VAR(i) < 1;                                                 \
         FLO_MACRO_VAR(i) =                                                    \
             (flo_appendTestFailureFinish(), FLO_ERROR("\n\n", FLO_FLUSH), 1))

#define FLO_TEST(testString)                                                   \
    for (ptrdiff_t FLO_MACRO_VAR(i) = (flo_unitTestStart(testString), 0);      \
         FLO_MACRO_VAR(i) < 1; FLO_MACRO_VAR(i) = 1)

#define FLO_TEST_TOPIC(testTopicString)                                        \
    for (ptrdiff_t FLO_MACRO_VAR(i) =                                          \
             (flo_testTopicStart(testTopicString), 0);                         \
         FLO_MACRO_VAR(i) < 1; FLO_MACRO_VAR(i) = (flo_testTopicFinish(), 1))

void flo_testSuiteStart();
int flo_testSuiteFinish();

void flo_testTopicStart(flo_string testTopic);
void flo_testTopicFinish();

void flo_unitTestStart(flo_string testName);

void flo_testSuccess();

void flo_testFailure();
void flo_appendTestFailureStart();
void flo_appendTestFailureFinish();

#ifdef __cplusplus
}
#endif

#endif
