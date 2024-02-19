#include "test-framework/test.h"
#include "util/assert.h"      // for FLO_ASSERT
#include "util/log.h"         // for FLO_LOG_CHOOSER_IMPL_2, FLO_INFO, FLO_...
#include "util/text/string.h" // for FLO_STRING, flo_string
#include <stddef.h>           // for ptrdiff_t
#include <stdint.h>           // for uint64_t

typedef struct {
    uint64_t successes;
    uint64_t failures;
    flo_string topic;
} flo_TestTopic;

#define MAX_TEST_TOPICS 1 << 6

static flo_TestTopic testTopics[MAX_TEST_TOPICS];
static ptrdiff_t nextTestTopic = 0;

void addTopic(flo_string topic) {
    FLO_ASSERT(nextTestTopic < MAX_TEST_TOPICS);
    testTopics[nextTestTopic++] =
        (flo_TestTopic){.failures = 0, .successes = 0, .topic = topic};
}

void appendSpaces() {
    for (ptrdiff_t i = 0; i < nextTestTopic - 1; i++) {
        FLO_INFO((FLO_STRING("  ")));
    }
}

void flo_printTestScore(uint64_t successes, uint64_t failures) {
    FLO_FLUSH_AFTER(FLO_STDOUT) {
        appendSpaces();

        FLO_INFO((FLO_STRING("[ ")));
        FLO_INFO(successes);
        FLO_INFO((FLO_STRING(" / ")));
        FLO_INFO(failures + successes);
        FLO_INFO((FLO_STRING(" ]\n")));
    }
}

void flo_testSuiteStart() {
    FLO_INFO((FLO_STRING("Starting test suite...\n\n")), FLO_FLUSH);

    addTopic(FLO_STRING("Root topic"));
}

int flo_testSuiteFinish() {
    uint64_t globalSuccesses = testTopics[0].successes;
    uint64_t globalFailures = testTopics[0].failures;

    flo_printTestScore(globalSuccesses, globalFailures);
    if (globalFailures > 0) {
        FLO_FLUSH_AFTER(FLO_STDERR) {
            FLO_ERROR((FLO_STRING("\nTest suite ")));
            flo_appendColor(FLO_COLOR_RED, FLO_STDERR);
            FLO_ERROR((FLO_STRING("failed")));
            flo_appendColorReset(FLO_STDERR);
            FLO_ERROR((FLO_STRING(".\n")));
        }
    } else {
        FLO_FLUSH_AFTER(FLO_STDOUT) {
            FLO_INFO((FLO_STRING("\nTest suite ")));
            flo_appendColor(FLO_COLOR_GREEN, FLO_STDOUT);
            FLO_INFO((FLO_STRING("successful")));
            flo_appendColorReset(FLO_STDOUT);
            FLO_INFO((FLO_STRING(".\n")));
        }
    }

    return globalFailures > 0;
}

void flo_testTopicStart(flo_string testTopic) {
    addTopic(testTopic);

    FLO_FLUSH_AFTER(FLO_STDOUT) {
        appendSpaces();
        FLO_INFO((FLO_STRING("Testing ")));
        FLO_INFO(testTopic);
        FLO_INFO((FLO_STRING("...\n")));
    }
}

void flo_testTopicFinish() {
    flo_printTestScore(testTopics[nextTestTopic - 1].successes,
                       testTopics[nextTestTopic - 1].failures);

    nextTestTopic--;
}

void flo_unitTestStart(flo_string testName) {
    FLO_FLUSH_AFTER(FLO_STDOUT) {
        appendSpaces();
        FLO_INFO((FLO_STRING("- ")));
        FLO_INFO(flo_stringWithMinSizeDefault(testName, 50));
    }
}

void flo_testSuccess() {
    for (ptrdiff_t i = 0; i < nextTestTopic; i++) {
        testTopics[i].successes++;
    }

    FLO_FLUSH_AFTER(FLO_STDOUT) {
        flo_appendColor(FLO_COLOR_GREEN, FLO_STDOUT);
        FLO_INFO(flo_stringWithMinSizeDefault(FLO_STRING("Success"), 20));
        flo_appendColorReset(FLO_STDOUT);
        FLO_INFO((FLO_STRING("\n")));
    }
}

void flo_testFailure() {
    for (ptrdiff_t i = 0; i < nextTestTopic; i++) {
        testTopics[i].failures++;
    }

    FLO_FLUSH_AFTER(FLO_STDERR) {
        flo_appendColor(FLO_COLOR_RED, FLO_STDERR);
        FLO_ERROR(flo_stringWithMinSizeDefault(FLO_STRING("Failure"), 20));
        flo_appendColorReset(FLO_STDERR);
        FLO_ERROR((FLO_STRING("\n")));
    }
}

void flo_appendTestFailureStart() {
    FLO_ERROR((FLO_STRING("----------------------------------------------------"
                          "----------------------------\n")));
    FLO_ERROR((FLO_STRING("|                                    REASON         "
                          "                           |\n")));
}

void flo_appendTestFailureFinish() {
    FLO_ERROR((FLO_STRING("|                                                   "
                          "                           |\n")));
    FLO_ERROR((FLO_STRING("----------------------------------------------------"
                          "----------------------------\n")));
}
