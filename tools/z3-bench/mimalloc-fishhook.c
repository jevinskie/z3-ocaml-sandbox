#include <unistd.h>

#include "fishhook.h"
#include <mimalloc.h>

// #include <mimalloc-new-delete.h>

void *malloc(size_t sz);
void *calloc(size_t count, size_t sz);
void free(void *p);
void *realloc(void *p, size_t sz);
void *reallocf(void *p, size_t sz);
void *valloc(size_t sz);
void *aligned_alloc(size_t alignment, size_t sz);
int posix_memalign(void **p, size_t alignment, size_t sz);

typedef typeof(&malloc) malloc_t;
typedef typeof(&calloc) calloc_t;
typedef typeof(&free) free_t;
typedef typeof(&realloc) realloc_t;
typedef typeof(&reallocf) reallocf_t;
typedef typeof(&valloc) valloc_t;
typedef typeof(&aligned_alloc) aligned_alloc_t;
typedef typeof(&posix_memalign) posix_memalign_t;

typedef malloc_t x_Znwm_t;
typedef free_t x_ZdlPv_t;

extern malloc_t mi_malloc_ext;
extern free_t mi_free_ext;
extern x_Znwm_t _Znwm;
extern x_ZdlPv_t _ZdlPv;

#if 1
static malloc_t orig_malloc;
static calloc_t orig_calloc;
static free_t orig_free;
static realloc_t orig_realloc;
static reallocf_t orig_reallocf;
static valloc_t orig_valloc;
static aligned_alloc_t orig_aligned_alloc;
static posix_memalign_t orig_posix_memalign;

static x_Znwm_t orig__Znwm;
static x_ZdlPv_t orig__ZdlPv;

__attribute__((constructor)) void init_mimalloc_helper(void) {
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
    rebind_symbols(
        (struct rebinding[4]){
            {"malloc", (void *)&mi_malloc_ext, (void **)&orig_malloc},
            {"free", (void *)&mi_free_ext, (void **)&orig_free},
            {"_Znwm", (void *)&mi_malloc_ext, (void **)&orig__Znwm},
            {"_ZdlPv", (void *)&mi_free_ext, (void **)&orig__ZdlPv},
        },
        4);
    write(STDOUT_FILENO, "rbd\n", 4);
}

__attribute__((destructor)) void deinit_mimalloc_helper(void) {
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
    rebind_symbols(
        (struct rebinding[4]){
            {"malloc", (void *)orig_malloc, (void **)&orig_malloc},
            {"free", (void *)orig_free, (void **)&orig_free},
            {"_Znwm", (void *)orig__Znwm, (void **)&orig__Znwm},
            {"_ZdlPv", (void *)orig__ZdlPv, (void **)&orig__ZdlPv},
        },
        4);
    write(STDOUT_FILENO, "ubd\n", 4);
}
#endif
