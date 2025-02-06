#undef NDEBUG
#include <assert.h>

#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>

#include <mimalloc.h>

// #include <mimalloc-new-delete.h>

#define DYLD_INTERPOSE(_replacement, _replacee)                                                   \
    __attribute__((used)) static struct {                                                         \
        const void *replacement;                                                                  \
        const void *replacee;                                                                     \
    } _interpose_##_replacee __attribute__((section("__DATA_CONST,__interpose,interposing"))) = { \
        (const void *)(unsigned long)&_replacement, (const void *)(unsigned long)&_replacee};

DYLD_INTERPOSE(mi_malloc, malloc);
DYLD_INTERPOSE(mi_calloc, calloc);
DYLD_INTERPOSE(mi_free, free);
DYLD_INTERPOSE(mi_realloc, realloc);
DYLD_INTERPOSE(mi_reallocf, reallocf);
DYLD_INTERPOSE(mi_valloc, valloc);
DYLD_INTERPOSE(mi_aligned_alloc, aligned_alloc);
DYLD_INTERPOSE(mi_posix_memalign, posix_memalign);

struct _libpthread_functions {
    unsigned long version;
    void (*exit)(int);       // added with version=1
    void *(*malloc)(size_t); // added with version=2
    void (*free)(void *);    // added with version=2
};

extern int __pthread_init(struct _libpthread_functions *pthread_funcs, const char *envp[], const char *apple[]);

static int my_pthread_init(struct _libpthread_functions *pthread_funcs, const char *envp[], const char *apple[]) {
    assert(pthread_funcs->version >= 2);
    const uintptr_t pi            = (uintptr_t)pthread_funcs;
    const uintptr_t pf_page_start = pi & ~((1ull << 14) - 1ull);
    assert(!mprotect((void *)pf_page_start, 16 * 1024, PROT_READ | PROT_WRITE));
    pthread_funcs->malloc = mi_malloc;
    pthread_funcs->free   = mi_free;
    assert(!mprotect((void *)pf_page_start, 16 * 1024, PROT_READ));
    return __pthread_init(pthread_funcs, envp, apple);
}

DYLD_INTERPOSE(my_pthread_init, __pthread_init);
