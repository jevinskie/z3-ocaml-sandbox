#undef NDEBUG
#include <assert.h>

#include <malloc/_malloc.h>
#include <malloc/_malloc_type.h>
#include <malloc/malloc.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>

// #include <mimalloc.h>

// #include <mimalloc-new-delete.h>

extern void vfree(void *ptr);
extern void *reallocarray(void *in_ptr, size_t nmemb, size_t size) asm("_reallocarray$DARWIN_EXTSN");
extern void *reallocarrayf(void *in_ptr, size_t nmemb, size_t size) asm("_reallocarrayf$DARWIN_EXTSN");
extern size_t malloc_size(const void *ptr);

extern typeof(malloc) mi_malloc;
extern typeof(calloc) mi_calloc;
extern typeof(free) mi_free;
extern typeof(realloc) mi_realloc;
extern typeof(reallocf) mi_reallocf;
extern typeof(valloc) mi_valloc;
extern typeof(reallocarray) mi_reallocarray;
extern typeof(reallocarrayf) mi_reallocarrayf;
extern typeof(aligned_alloc) mi_aligned_alloc;
extern typeof(posix_memalign) mi_posix_memalign;
extern typeof(malloc_size) mi_malloc_size;
extern typeof(malloc_good_size) mi_malloc_good_size;
extern typeof(malloc_good_size) mi_good_size;

#define DYLD_INTERPOSE(_replacement, _replacee)                                               \
    __attribute__((used)) static struct {                                                     \
        const void *replacement;                                                              \
        const void *replacee;                                                                 \
    } _interpose_##_replacee __attribute__((section("__DATA,__interpose,interposing"))) = {   \
        (const void *)(unsigned long)&_replacement, (const void *)(unsigned long)&_replacee};

DYLD_INTERPOSE(mi_malloc, malloc);
DYLD_INTERPOSE(mi_calloc, calloc);
DYLD_INTERPOSE(mi_free, free);
DYLD_INTERPOSE(mi_realloc, realloc);
DYLD_INTERPOSE(mi_reallocf, reallocf);
DYLD_INTERPOSE(mi_reallocarray, reallocarray);
DYLD_INTERPOSE(mi_valloc, valloc);
DYLD_INTERPOSE(mi_free, vfree);
DYLD_INTERPOSE(mi_aligned_alloc, aligned_alloc);
DYLD_INTERPOSE(mi_posix_memalign, posix_memalign);
DYLD_INTERPOSE(mi_good_size, malloc_good_size);
DYLD_INTERPOSE(mi_good_size, malloc_size);

struct _libpthread_functions {
    unsigned long version;
    void (*exit)(int);       // added with version=1
    void *(*malloc)(size_t); // added with version=2
    void (*free)(void *);    // added with version=2
};

extern int __pthread_init(struct _libpthread_functions *pthread_funcs, const char *envp[], const char *apple[]);

extern void mi_prim_get_default_heap();
extern struct mi_heap_t *_mi_heap_main_get(void);
extern _Thread_local struct mi_heap_t *_mi_heap_default;

static int my_pthread_init(struct _libpthread_functions *pthread_funcs, const char *envp[], const char *apple[]) {

    assert(pthread_funcs->version >= 2);
    const uintptr_t pi            = (uintptr_t)pthread_funcs;
    const uintptr_t pf_page_start = pi & ~((1ull << 14) - 1ull);
    assert(!mprotect((void *)pf_page_start, 16 * 1024, PROT_READ | PROT_WRITE));
    pthread_funcs->malloc = mi_malloc;
    pthread_funcs->free   = mi_free;
    assert(!mprotect((void *)pf_page_start, 16 * 1024, PROT_READ));

    // _mi_heap_main_get();
    // volatile void *p = _mi_heap_default;

    return __pthread_init(pthread_funcs, envp, apple);
}

// DYLD_INTERPOSE(my_pthread_init, __pthread_init);
