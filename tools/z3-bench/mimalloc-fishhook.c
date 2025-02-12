// #include <unistd.h>

#ifdef malloc
#undef malloc
#endif

#ifdef free
#undef free
#endif

#ifdef calloc
#undef calloc
#endif

#include "fishhook.h"
#include "types-bare.h"
// #include <mimalloc.h>

// #include <mimalloc-new-delete.h>

#include "malloc-wrapped.h"
#include "mimalloc-types.h"
#include "misc-decls.h"

#if 1
SYM_INT malloc_t *orig_malloc;
SYM_INT free_t *orig_free;
static calloc_t *orig_calloc;
static realloc_t *orig_realloc;
static reallocf_t *orig_reallocf;
static valloc_t *orig_valloc;
static aligned_alloc_t *orig_aligned_alloc;
static posix_memalign_t *orig_posix_memalign;

static x_Znwm_t *orig__Znwm;
static x_ZdlPv_t *orig__ZdlPv;

__attribute__((constructor)) static void init_mimalloc_helper(void) {
    write(STDOUT_FILENO, "lol\n", 4);
#if 0
    rebind_symbols((struct rebinding[8]){{"malloc", (void *)&mi_malloc, (void **)&orig_malloc},
                                         {"calloc", (void *)&mi_calloc, (void **)&orig_calloc},
                                         {"free", (void *)&mi_free, (void **)&orig_free},
                                         {"realloc", (void *)&mi_realloc, (void **)&orig_realloc},
                                         {"reallocf", (void *)&mi_reallocf, (void **)&orig_reallocf},
                                         {"valloc", (void *)&mi_valloc, (void **)&orig_valloc},
                                         {"aligned_alloc", (void *)&mi_aligned_alloc},
                                         {"posix_memalign", (void *)&mi_posix_memalign, (void **)&orig_posix_memalign}},
                   8);
#endif
#if 0
    rebind_symbols(
        (struct rebinding[4]){
            {"malloc", (void *)&mi_malloc_ext, (void **)&orig_malloc},
            {"free", (void *)&mi_free_ext, (void **)&orig_free},
            {"_Znwm", (void *)&mi_malloc_ext, (void **)&orig__Znwm},
            {"_ZdlPv", (void *)&mi_free_ext, (void **)&orig__ZdlPv},
        },
        4);
#endif
#if 0
    rebind_symbols(
        (struct rebinding[2]){
            {"malloc", (void *)&printing_malloc, (void **)&orig_malloc},
            {"free", (void *)&printing_free, (void **)&orig_free},
        },
        2);
#endif
    write(STDOUT_FILENO, "rbd\n", 4);
}

__attribute__((destructor)) static void deinit_mimalloc_helper(void) {
    write(STDOUT_FILENO, "bai\n", 4);
#if 0
    rebind_symbols(
        (struct rebinding[8]){{"malloc", (void *)orig_malloc, (void **)&orig_malloc},
                              {"calloc", (void *)orig_calloc, (void **)&orig_calloc},
                              {"free", (void *)orig_free, (void **)&orig_free},
                              {"realloc", (void *)orig_realloc, (void **)&orig_realloc},
                              {"reallocf", (void *)orig_reallocf, (void **)&orig_reallocf},
                              {"valloc", (void *)orig_valloc, (void **)&orig_valloc},
                              {"aligned_alloc", (void *)orig_aligned_alloc},
                              {"posix_memalign", (void *)orig_posix_memalign, (void **)&orig_posix_memalign}},
        8);
#endif
#if 0
    rebind_symbols(
        (struct rebinding[4]){
            {"malloc", (void *)orig_malloc, (void **)&orig_malloc},
            {"free", (void *)orig_free, (void **)&orig_free},
            {"_Znwm", (void *)orig__Znwm, (void **)&orig__Znwm},
            {"_ZdlPv", (void *)orig__ZdlPv, (void **)&orig__ZdlPv},
        },
        4);
#endif
#if 0
    rebind_symbols(
        (struct rebinding[2]){
            {"malloc", (void *)orig_malloc, (void **)&orig_malloc},
            {"free", (void *)orig_free, (void **)&orig_free},
        },
        2);
#endif
    write(STDOUT_FILENO, "ubd\n", 4);
}
#endif
