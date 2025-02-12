#pragma once

#include "types-bare.h"

extern void *malloc(jsize_t sz);
extern void *calloc(jsize_t count, jsize_t sz);
extern void free(void *p);
extern void *realloc(void *p, jsize_t sz);
extern void *reallocf(void *p, jsize_t sz);
extern void *reallocarray(void *in_ptr, jsize_t nmemb, jsize_t size) __asm("_reallocarray$DARWIN_EXTSN");
extern void *reallocarrayf(void *in_ptr, jsize_t nmemb, jsize_t size) __asm("_reallocarrayf$DARWIN_EXTSN");
extern void *valloc(jsize_t sz);
extern void *aligned_alloc(jsize_t alignment, jsize_t sz);
extern int posix_memalign(void **p, jsize_t alignment, jsize_t sz);
extern jsize_t malloc_good_size(jsize_t size);
extern jsize_t malloc_size(const void *ptr);
extern void vfree(void *ptr);
extern void cfree(void *ptr);

typedef typeof(malloc) malloc_t;
typedef typeof(calloc) calloc_t;
typedef typeof(free) free_t;
typedef typeof(realloc) realloc_t;
typedef typeof(reallocf) reallocf_t;
typedef typeof(reallocarray) reallocarray_t;
typedef typeof(reallocarrayf) reallocarrayf_t;
typedef typeof(valloc) valloc_t;
typedef typeof(aligned_alloc) aligned_alloc_t;
typedef typeof(posix_memalign) posix_memalign_t;
typedef typeof(malloc_good_size) malloc_good_size_t;
typedef typeof(malloc_size) malloc_size_t;
typedef typeof(vfree) vfree_t;
typedef typeof(cfree) cfree_t;

typedef malloc_t x_Znwm_t;
typedef free_t x_ZdlPv_t;

extern x_Znwm_t _Znwm;
extern x_ZdlPv_t _ZdlPv;

extern malloc_t mi_malloc_ext;
extern free_t mi_free_ext;

extern malloc_t mi_malloc;
extern free_t mi_free;
