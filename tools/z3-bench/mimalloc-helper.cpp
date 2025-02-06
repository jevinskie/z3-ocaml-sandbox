#include <unistd.h>

#include "fishhook.h"
#include <mimalloc.h>

#include <mimalloc-new-delete.h>

extern "C" {
void *malloc(size_t sz);
void *calloc(size_t count, size_t sz);
void free(void *p);
void *realloc(void *p, size_t sz);
void *reallocf(void *p, size_t sz);
void *valloc(size_t sz);
void *aligned_alloc(size_t alignment, size_t sz);
int posix_memalign(void **p, size_t alignment, size_t sz);
}

using malloc_t         = decltype(&malloc);
using calloc_t         = decltype(&calloc);
using free_t           = decltype(&free);
using realloc_t        = decltype(&realloc);
using reallocf_t       = decltype(&reallocf);
using valloc_t         = decltype(&valloc);
using aligned_alloc_t  = decltype(&aligned_alloc);
using posix_memalign_t = decltype(&posix_memalign);

#if 1
static malloc_t orig_malloc;
static calloc_t orig_calloc;
static free_t orig_free;
static realloc_t orig_realloc;
static reallocf_t orig_reallocf;
static valloc_t orig_valloc;
static aligned_alloc_t orig_aligned_alloc;
static posix_memalign_t orig_posix_memalign;

extern "C" [[gnu::constructor]]
void init_mimalloc_helper(void) {
    write(STDOUT_FILENO, "lol\n", 4);
    rebind_symbols((struct rebinding[8]){{"malloc", (void *)&mi_malloc, (void **)&orig_malloc},
                                         {"calloc", (void *)&mi_calloc, (void **)&orig_calloc},
                                         {"free", (void *)&mi_free, (void **)&orig_free},
                                         {"realloc", (void *)&mi_realloc, (void **)&orig_realloc},
                                         {"reallocf", (void *)&mi_reallocf, (void **)&orig_reallocf},
                                         {"valloc", (void *)&mi_valloc, (void **)&orig_valloc},
                                         {"aligned_alloc", (void *)&mi_aligned_alloc},
                                         {"posix_memalign", (void *)&mi_posix_memalign, (void **)&orig_posix_memalign}},
                   8);
    write(STDOUT_FILENO, "rbd\n", 4);
}

extern "C" [[gnu::destructor]]
void deinit_mimalloc_helper(void) {
    write(STDOUT_FILENO, "bai\n", 4);
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
    write(STDOUT_FILENO, "ubd\n", 4);
}
#endif
