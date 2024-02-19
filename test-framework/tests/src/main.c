#include "test-framework/test.h" // for flo_testSuccess, FLO_TEST, FLO_TEST...
#include "util/log.h"            // for FLO_ERROR, FLO_LOG_CHOOSER_IMPL_2
#include "util/memory/arena.h"   // for flo_arena
#include "util/memory/macros.h"  // for FLO_COUNTOF
#include "util/text/string.h"    // for FLO_STRING, flo_string
#include <errno.h>               // for errno
#include <stddef.h>              // for NULL, ptrdiff_t
#include <string.h>              // for strerror
#include <sys/mman.h>            // for mmap, munmap, MAP_ANONYMOUS, MAP_FA...

#define CAP 1 << 21

static flo_string testNames[] = {
    FLO_STRING("aaaaaaaaa"),   FLO_STRING("bbbbbbbbb"),
    FLO_STRING("cccccccccc"),  FLO_STRING("dddddddddd"),
    FLO_STRING("eeeeeeeeee"),  FLO_STRING("fffffffff"),
    FLO_STRING("ggggggggggg"), FLO_STRING("hhhhhhhhh"),
    FLO_STRING("iiiiiiiiii"),  FLO_STRING("ffffffffffff"),
};

static ptrdiff_t numTestNames = FLO_COUNTOF(testNames);

void test1() {
    FLO_TEST(FLO_STRING("Test 1")) {
        //
        flo_testSuccess();
    }
}

void test2() {
    FLO_TEST(FLO_STRING("Test 2")) {
        //
        flo_testSuccess();
    }
}

void multipleTests() {
    for (ptrdiff_t i = 0; i < numTestNames; i++) {
        FLO_TEST(testNames[i]) {
            if (i % 2 == 0) {
                flo_testSuccess();
            } else {
                FLO_TEST_FAILURE {
                    // Inside this scoped block you can do your
                    // additional logging.
                    FLO_ERROR("big chaos\n");
                }
            }
        }
    }
}

void test3() {
    FLO_TEST(FLO_STRING("Test 3")) {
        //
        FLO_TEST_FAILURE {
            // Inside this scoped block you can do your
            // additional logging.
            FLO_ERROR("biger ddfd chaos\n");
        }
    }
}

int main() {
    char *begin = mmap(NULL, CAP, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (begin == MAP_FAILED) {
        FLO_FLUSH_AFTER(FLO_STDERR) {
            FLO_ERROR("Failed to allocate memory!\n");
            FLO_ERROR("Error code: ");
            FLO_ERROR(errno, FLO_NEWLINE);
            FLO_ERROR("Error message: ");
            FLO_ERROR(strerror(errno), FLO_NEWLINE);
        }
        return -1;
    }

    flo_arena arena =
        (flo_arena){.beg = begin, .cap = CAP, .end = begin + (ptrdiff_t)(CAP)};

    void *jmp_buf[5];
    if (__builtin_setjmp(jmp_buf)) {
        if (munmap(arena.beg, arena.cap) == -1) {
            FLO_FLUSH_AFTER(FLO_STDERR) {
                FLO_ERROR((FLO_STRING("Failed to unmap memory from arena!\n"
                                      "Arena Details:\n"
                                      "  beg: ")));
                FLO_ERROR(arena.beg);
                FLO_ERROR((FLO_STRING("\n end: ")));
                FLO_ERROR(arena.end);
                FLO_ERROR((FLO_STRING("\n cap: ")));
                FLO_ERROR(arena.cap);
                FLO_ERROR((FLO_STRING("\nZeroing Arena regardless.\n")));
            }
        }
        arena.beg = NULL;
        arena.end = NULL;
        arena.cap = 0;
        arena.jmp_buf = NULL;
        FLO_ERROR((FLO_STRING("OOM/overflow in arena!\n")), FLO_FLUSH);
        return -1;
    }
    arena.jmp_buf = jmp_buf;

    flo_testSuiteStart();

    FLO_TEST_TOPIC(FLO_STRING("My first topic")) { test1(); }

    FLO_TEST_TOPIC(FLO_STRING("My second topic")) {
        test2();
        FLO_TEST_TOPIC(FLO_STRING("inside topic")) { multipleTests(); }
        test3();
    }

    return flo_testSuiteFinish();
}
