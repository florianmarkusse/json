#include "test-framework/test.h"              // for flo_testSuiteFinish
#include "test/integration-deserialization.h" // for flo_testJsonDeserializ...
#include "test/string-deserialization.h"      // for flo_testStringDeserial...
#include "test/struct-deserialization.h"      // for flo_testStructDeserial...
#include "test/uint64-deserialization.h"      // for flo_testUint64Deserial...
#include "test/unit.h"                        // for flo_testUnitDeserializ...
#include "util/log.h"                         // for FLO_ERROR, FLO_LOG_CHO...
#include "util/memory/arena.h"                // for flo_arena
#include "util/text/string.h"                 // for FLO_STRING
#include <errno.h>                            // for errno
#include <stddef.h>                           // for NULL, ptrdiff_t, size_t
#include <string.h>                           // for strerror
#include <sys/mman.h>                         // for mmap, munmap, MAP_ANON...

size_t memoryCap = (size_t)1 << 21;

int main() {
    char *begin = mmap(NULL, memoryCap, PROT_READ | PROT_WRITE,
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

    flo_arena arena = (flo_arena){
        .beg = begin, .cap = memoryCap, .end = begin + (ptrdiff_t)(memoryCap)};

    void *jmp_buf[5];
    if (__builtin_setjmp(jmp_buf)) {
        if (munmap(arena.beg, arena.cap) == -1) {
            FLO_FLUSH_AFTER(FLO_STDERR) {
                FLO_ERROR((FLO_STRING("Failed to unmap memory from"
                                      "arena !\n "
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

    flo_testUnitDeserialization(arena);
    flo_testStringDeserialization(arena);
    flo_testUint64Deserialization(arena);
    flo_testStructDeserialization(arena);
    flo_testJsonDeserializationIntegration(arena);

    return flo_testSuiteFinish();
}
