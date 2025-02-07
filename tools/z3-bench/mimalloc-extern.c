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
extern void *reallocarray(void *in_ptr, size_t nmemb, size_t size) __asm("_reallocarray$DARWIN_EXTSN");
extern void *reallocarrayf(void *in_ptr, size_t nmemb, size_t size) __asm("_reallocarrayf$DARWIN_EXTSN");
extern size_t malloc_size(const void *ptr);

extern typeof(malloc) mi_malloc;
extern typeof(calloc) mi_calloc;
extern typeof(free) mi_free;
extern typeof(realloc) mi_realloc;
extern typeof(reallocf) mi_reallocf;
extern typeof(valloc) mi_valloc;
extern typeof(reallocarray) mi_reallocarray;
// extern typeof(reallocarrayf) mi_reallocarrayf;
extern typeof(aligned_alloc) mi_aligned_alloc;
extern typeof(posix_memalign) mi_posix_memalign;
extern typeof(malloc_size) mi_malloc_size;
extern typeof(malloc_good_size) mi_malloc_good_size;
extern typeof(malloc_good_size) mi_good_size;

extern typeof(malloc) _Znwm;
extern typeof(free) _ZdlPv;

__asm(".global _mi_malloc_ext\n_mi_malloc_ext = _mi_malloc\n");
__asm(".global _mi_free_ext\n_mi_free_ext = _mi_free\n");
