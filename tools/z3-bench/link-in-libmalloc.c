#include "clear-macros.h"

#include "malloc-wrapped.h"
#include "mimalloc-types.h"
#include "noalloc-stdio.h"
#include "types-bare.h"
// __attribute__((used,visibility("default")))
// volatile void *dummy_p;

extern int rand(void);

__attribute__((used, visibility("default"))) void link_in_symbols_dummy(void) {
    volatile void *p;
    p        = &malloc;
    p        = &free;
    p        = &calloc;
    p        = &malloc_size;
    p        = &abort; // libsystem_c.dylib
    void *p2 = malloc(4);
    if (p2) {
        puts_ptr((void *)(juintptr_t)malloc_size(p2));
        free(p2);
    }
    p2 = calloc(4, 4);
    if (p2) {
        free(p2);
    }
    if (rand() & 0x40) {
        abort();
    }
}
