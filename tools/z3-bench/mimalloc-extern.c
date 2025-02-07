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

#define ALIAS_GLOBAL(name, target)         __asm__(".global _" name "\n_" name " = _" target "\n")
#define ALIAS_PRIVATE_EXTERN(name, target) __asm__(".private_extern _" name "\n_" name " = _" target "\n")

extern void vfree(void *ptr);
extern void *reallocarray(void *in_ptr, size_t nmemb, size_t size) __asm__("_reallocarray$DARWIN_EXTSN");
extern void *reallocarrayf(void *in_ptr, size_t nmemb, size_t size) __asm__("_reallocarrayf$DARWIN_EXTSN");
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

// mi_malloc and mi_free aliases
ALIAS_GLOBAL("mi_malloc_ext", "mi_malloc");
ALIAS_GLOBAL("mi_free_ext", "mi_free");
ALIAS_PRIVATE_EXTERN("malloc", "mi_malloc");
ALIAS_PRIVATE_EXTERN("free", "mi_free");

// C++ new/delete operator aliases
ALIAS_GLOBAL("_Znwm_ext", "mi_malloc");
ALIAS_GLOBAL("_ZdlPv_ext", "mi_free");
ALIAS_PRIVATE_EXTERN("_Znwm", "mi_malloc");
ALIAS_PRIVATE_EXTERN("_ZdlPv", "mi_free");
