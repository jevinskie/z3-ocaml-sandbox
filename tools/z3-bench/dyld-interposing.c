#include "clear-macros.h"

#include "malloc-wrapped.h"
#include "mimalloc-types.h"
#include "misc-decls.h"
#include "noalloc-stdio.h"
#include "types-bare.h"

// #undef NDEBUG
// #include <assert.h>

// #include <malloc/_malloc.h>
// #include <malloc/_malloc_type.h>
// #include <malloc/malloc.h>
// #include <stdint.h>
// #include <stdlib.h>
// #include <sys/mman.h>

// #include <mimalloc.h>

// #include <mimalloc-new-delete.h>

// extern void vfree(void *ptr);
// extern void *reallocarray(void *in_ptr, jsize_t nmemb, jsize_t size) __asm("_reallocarray$DARWIN_EXTSN");
// extern void *reallocarrayf(void *in_ptr, jsize_t nmemb, jsize_t size) __asm("_reallocarrayf$DARWIN_EXTSN");
// extern jsize_t malloc_size(const void *ptr);

extern typeof(malloc) mi_malloc_ext;
extern typeof(free) mi_free_ext;
extern typeof(calloc) mi_calloc_ext;
extern typeof(malloc_size) mi_malloc_size_ext;

extern typeof(malloc) mi_malloc;
extern typeof(free) mi_free;
extern typeof(calloc) mi_calloc;
extern typeof(realloc) mi_realloc;
extern typeof(reallocf) mi_reallocf;
extern typeof(valloc) mi_valloc;
extern typeof(reallocarray) mi_reallocarray;
extern typeof(reallocarrayf) mi_reallocarrayf;
extern typeof(aligned_alloc) mi_aligned_alloc;
extern typeof(posix_memalign) mi_posix_memalign;
extern typeof(malloc_size) mi_malloc_size;
extern typeof(malloc_good_size) mi_malloc_good_size;
// extern typeof(malloc_good_size) mi_good_size;

extern typeof(malloc) printing_malloc;
extern typeof(free) printing_free;

// DYLD_INTERPOSE(printing_malloc, malloc);
// DYLD_INTERPOSE(printing_free, free);

DYLD_INTERPOSE(mi_malloc_ext, malloc);
DYLD_INTERPOSE(mi_free_ext, free);
DYLD_INTERPOSE(mi_calloc_ext, calloc);
DYLD_INTERPOSE(mi_malloc_size_ext, malloc_size);
// DYLD_INTERPOSE(mi_calloc, calloc);
// DYLD_INTERPOSE(mi_realloc, realloc);
// DYLD_INTERPOSE(mi_reallocf, reallocf);
// DYLD_INTERPOSE(mi_reallocarray, reallocarray);
// DYLD_INTERPOSE(mi_reallocarrayf, reallocarrayf);
// DYLD_INTERPOSE(mi_valloc, valloc);
// DYLD_INTERPOSE(mi_free, vfree);
// DYLD_INTERPOSE(mi_aligned_alloc, aligned_alloc);
// DYLD_INTERPOSE(mi_posix_memalign, posix_memalign);
// DYLD_INTERPOSE(mi_good_size, malloc_good_size);
// DYLD_INTERPOSE(mi_good_size, malloc_size);

extern typeof(malloc) _Znwm;
extern typeof(free) _ZdlPv;

// DYLD_INTERPOSE(mi_malloc_ext, _Znwm);
// DYLD_INTERPOSE(mi_free_ext, _ZdlPv);

extern void mi_prim_get_default_heap();
extern struct mi_heap_t *_mi_heap_main_get(void);
// extern _Thread_local struct mi_heap_t *_mi_heap_default;

static __attribute__((always_inline, const)) void **my_os_tsd_get_base(void) {
    juintptr_t tsd;
    __asm__("mrs %0, TPIDRRO_EL0" : "=r"(tsd));
    return (void **)tsd;
}

#ifndef MI_TLS_SLOT_HEAP_DEFAULT
#define MI_TLS_SLOT_HEAP_DEFAULT 6 // wine
#endif

// extern void *_mi_heap_empty_ext;
struct my_mi_heap_s;
extern struct my_mi_heap_s _mi_heap_empty_ext;

#if 0
static __attribute__((constructor)) void init_mimalloc_tls(void) {
    puts_str("dyld-interposing init_mimalloc_tls()");
    puts_str("dyld-interposing init_mimalloc_tls my_os_tsd_get_base()[MI_TLS_SLOT_HEAP_DEFAULT] =>");
    puts_ptr(my_os_tsd_get_base()[MI_TLS_SLOT_HEAP_DEFAULT]);
    puts_str("dyld-interposing init_mimalloc_tls _mi_heap_empty_ext =>");
    puts_ptr(&_mi_heap_empty_ext);
    my_os_tsd_get_base()[MI_TLS_SLOT_HEAP_DEFAULT] = &_mi_heap_empty_ext;
}
#endif

static __attribute__((noreturn)) void my_thread_start(pthread_t thread, jmach_port_t kport, void *(*fun)(void *),
                                                      void *arg, jsize_t stacksize, unsigned int flags) {
    puts_str("dyld-interposing my_thread_start =>");
    puts_ptr(my_thread_start);
    puts_str("dyld-interposing thread_start =>");
    puts_ptr(thread_start);
    my_os_tsd_get_base()[MI_TLS_SLOT_HEAP_DEFAULT] = &_mi_heap_empty_ext;
    // my_os_tsd_get_base()[MI_TLS_SLOT_HEAP_DEFAULT] = NULL;
    thread_start(thread, kport, fun, arg, stacksize, flags);
}

static int my_bsdthread_register(void *threadstart, void *wqthread, int pthsize, void *pthread_init_data,
                                 jint32_t *pthread_init_data_size, juint64_t dispatchqueue_offset) {
    (void)threadstart;
    puts_str("dyld-interposing my_bsdthread_register =>");
    puts_ptr(my_bsdthread_register);
    puts_str("dyld-interposing __bsdthread_register =>");
    puts_ptr(__bsdthread_register);
    puts_str("dyld-interposing my_bsdthread_register() threadstart =>");
    puts_ptr(threadstart);
    return __bsdthread_register(my_thread_start, wqthread, pthsize, pthread_init_data, pthread_init_data_size,
                                dispatchqueue_offset);
}

DYLD_INTERPOSE(my_bsdthread_register, __bsdthread_register);

static int my_pthread_init(struct _libpthread_functions *pthread_funcs, const char *envp[], const char *apple[]) {
    puts_str("dyld-interposing my_pthread_init =>");
    puts_ptr(my_pthread_init);
    puts_str("dyld-interposing __pthread_init =>");
    puts_ptr(__pthread_init);

    // assert(pthread_funcs->version >= 2);
    // const uintptr_t pi            = (uintptr_t)pthread_funcs;
    // const uintptr_t pf_page_start = pi & ~((1ull << 14) - 1ull);
    // assert(!mprotect((void *)pf_page_start, 16 * 1024, PROT_READ | PROT_WRITE));
    // pthread_funcs->malloc = mi_malloc_ext;
    // pthread_funcs->free   = mi_free_ext;
    // assert(!mprotect((void *)pf_page_start, 16 * 1024, PROT_READ));

    // _mi_heap_main_get();
    // volatile void *p = _mi_heap_default;

    return __pthread_init(pthread_funcs, envp, apple);
}

DYLD_INTERPOSE(my_pthread_init, __pthread_init);

static void my_pthread_start(pthread_t self, jmach_port_t kport, void *(*fun)(void *), void *arg, jsize_t stacksize,
                             unsigned int pflags) {
    puts_str("dyld-interposing my_pthread_start =>");
    puts_ptr(my_pthread_start);
    puts_str("dyld-interposing _pthread_start =>");
    puts_ptr(_pthread_start);
    my_os_tsd_get_base()[MI_TLS_SLOT_HEAP_DEFAULT] = &_mi_heap_empty_ext;

    _pthread_start(self, kport, fun, arg, stacksize, pflags);
}

// DYLD_INTERPOSE(my_pthread_start, _pthread_start);
