#include "util/memory/arena.h"
#include "util/assert.h"        // for FLO_ASSERT
#include "util/memory/macros.h" // for FLO_NULL_ON_FAIL, FLO_ZERO_MEMORY
#include <stdint.h>             // for uintptr_t
#include <string.h>             // for memcpy, memset

__attribute((malloc, alloc_size(2, 4), alloc_align(3))) void *
flo_alloc(flo_arena *a, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count,
          unsigned char flags) {
    FLO_ASSERT(size > 0);
    FLO_ASSERT(align > 0);
    FLO_ASSERT((align & (align - 1)) == 0);

    ptrdiff_t avail = a->end - a->beg;
    ptrdiff_t padding = -(uintptr_t)a->beg & (align - 1);
    if (count > (avail - padding) / size) {
        if (flags & FLO_NULL_ON_FAIL) {
            return NULL;
        }
        __builtin_longjmp(a->jmp_buf, 1);
    }
    ptrdiff_t total = size * count;
    char *p = a->beg + padding;
    a->beg += padding + total;

    return flags & FLO_ZERO_MEMORY ? memset(p, 0, total) : p;
}

__attribute((malloc, alloc_size(3, 5), alloc_align(4))) void *
flo_copyToArena(flo_arena *arena, void *data, ptrdiff_t size, ptrdiff_t align,
                ptrdiff_t count) {
    unsigned char *copy = flo_alloc(arena, size, align, count, 0);
    memcpy(copy, data, size * count);
    return copy;
}
