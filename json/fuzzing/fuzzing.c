#include "parse-framework/parse.h" // for flo_parseStatus
#include "util/file/file-status.h" // for FILE_SUCCESS, flo_FileStatus
#include "util/file/read.h"        // for flo_readFile
#include "util/log.h"              // for FLO_ERROR, FLO_LOG_CHOOSER_IMPL_2
#include "util/memory/arena.h"     // for flo_arena, FLO_NEW
#include "util/text/string.h"      // for FLO_STRING, flo_string, FLO_STRIN...
#include "util/types.h"            // for flo_string_max_a, flo_char_d_a
#include "json/deserialize.h"      // for flo_json_parse, flo_Deseria...
#include "json/serialize.h"
#include "json/struct-member.h" // for FLO_JSON_CREATE_STRUCT_MEMBER
#include "json/types.h"         // for FLO_JSON_CREATE_OBJECT, flo_json_...
#include <errno.h>              // for errno
#include <stddef.h>             // for NULL, ptrdiff_t, size_t
#include <string.h>             // for strerror
#include <sys/mman.h>           // for mmap, munmap, MAP_ANONYMOUS, MAP_...
#include <unistd.h>             // IWYU pragma: keep

typedef struct flo_AandB {
    flo_json_string a;
    flo_json_string b;
} flo_AandB;
FLO_JSON_CREATE_OBJECT(flo_AandB, flo_json_AandB);

typedef struct flo_InsideTestStruct {
    flo_json_string aa;
    flo_json_uint64 bb;
} flo_InsideTestStruct;
FLO_JSON_CREATE_OBJECT(flo_InsideTestStruct, flo_json_InsideTestStruct);
static flo_json_schema insideTestStructBuffer[] = {
    FLO_JSON_CREATE_SIMPLE_KEY(flo_json_InsideTestStruct, flo_InsideTestStruct,
                               aa),
    FLO_JSON_CREATE_SIMPLE_KEY(flo_json_InsideTestStruct, flo_InsideTestStruct,
                               bb)};
FLO_JSON_CREATE_KEYS_ARRAY(insideTestStructBuffer, insideTestStructMembers);

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

size_t memoryCap = (size_t)1 << 21;

// AFter basic includes and static setup:
#ifdef FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION
__AFL_FUZZ_INIT();
#endif

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
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

#ifdef FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION
    // After setup, CLI parsing, memory intialization, etc.
    __AFL_INIT();

    // After AFL_INIT and before AFL_LOOP
    unsigned char *buf = __AFL_FUZZ_TESTCASE_BUF;

    // Add this while loop, AFL_LOOP.
    while (__AFL_LOOP(10000)) {
        flo_arena copy = arena;
        // Get length of FUZZ_TESTCASE, do NOT use in function call
        ptrdiff_t len = (ptrdiff_t)__AFL_FUZZ_TESTCASE_LEN;

        flo_json_schema root = FLO_JSON_CREATE_ROOT(FLO_DESERIALIZE_OBJECT,
                                                    &integrationStructMembers,
                                                    flo_json_TestStruct);

        flo_string generatedJson = (flo_string){.buf = buf, .len = len};
        flo_parseStatus ps =
            ((flo_parseStatus){.text = generatedJson, .idx = 0});
        flo_string *buf = FLO_NEW(&copy, flo_string, 10);
        flo_string_max_a nestedKeys = {.cap = 10, .len = 0, .buf = buf};
        flo_json_TestStruct toDeserialize = {0};

        flo_json_parse((flo_json_void *)&toDeserialize, &nestedKeys, &ps, root);
        flo_char_d_a actualSerialized = {0};
        flo_json_append(&actualSerialized, (flo_json_void *)&toDeserialize,
                        root, &arena);
    }
#else
    if (argc != 2) {
        FLO_FLUSH_AFTER(FLO_STDERR) {
            FLO_ERROR(FLO_STRING("Usage: ./binary file/to/triage\n"));
        }
        return -1;
    }

    flo_string fileContents;
    flo_FileStatus status = flo_readFile(argv[1], &fileContents, &arena);

    if (status != FILE_SUCCESS) {
        FLO_FLUSH_AFTER(FLO_STDERR) {
            FLO_ERROR(FLO_STRING("Failed to read file "));
            FLO_ERROR(argv[1], FLO_NEWLINE);
        }
        return 1;
    }

    FLO_FLUSH_AFTER(FLO_STDOUT) {
        FLO_INFO(FLO_STRING("Read below file contents:\n"));
        FLO_INFO(fileContents, FLO_NEWLINE);
    }

    flo_json_schema root = FLO_JSON_CREATE_ROOT(
        FLO_DESERIALIZE_OBJECT, &integrationStructMembers, flo_json_TestStruct);

    flo_string generatedJson =
        (flo_string){.buf = fileContents.buf, .len = fileContents.len};
    flo_parseStatus ps = ((flo_parseStatus){.text = generatedJson, .idx = 0});
    flo_string *buf = FLO_NEW(&arena, flo_string, 10);
    flo_string_max_a nestedKeys = {.cap = 10, .len = 0, .buf = buf};
    flo_json_TestStruct toDeserialize = {0};

    flo_json_parse((flo_json_void *)&toDeserialize, &nestedKeys, &ps, root);
    flo_char_d_a actualSerialized = {0};
    flo_json_append(&actualSerialized, (flo_json_void *)&toDeserialize, root,
                    &arena);
    FLO_FLUSH_AFTER(FLO_STDOUT) {
        FLO_INFO(FLO_STRING_LEN(actualSerialized.buf, actualSerialized.len),
                 FLO_NEWLINE);
    }
    FLO_FLUSH_AFTER(FLO_STDOUT) {
        FLO_INFO(FLO_STRING("Program completed.\n"));
    }
#endif
    return 0;
}
