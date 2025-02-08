#pragma once

typedef __SIZE_TYPE__ lsize_t;

extern void *malloc(lsize_t sz);
extern void *calloc(lsize_t count, lsize_t sz);
extern void free(void *p);
extern void *realloc(void *p, size_t sz);
extern void *reallocf(void *p, size_t sz);
extern void *valloc(lsize_t sz);
extern void *aligned_alloc(lsize_t alignment, lsize_t sz);
extern int posix_memalign(void **p, lsize_t alignment, lsize_t sz);

typedef typeof(malloc) malloc_t;
typedef typeof(calloc) calloc_t;
typedef typeof(free) free_t;
typedef typeof(realloc) realloc_t;
typedef typeof(reallocf) reallocf_t;
typedef typeof(valloc) valloc_t;
typedef typeof(aligned_alloc) aligned_alloc_t;
typedef typeof(posix_memalign) posix_memalign_t;

typedef malloc_t x_Znwm_t;
typedef free_t x_ZdlPv_t;

extern x_Znwm_t _Znwm;
extern x_ZdlPv_t _ZdlPv;

extern malloc_t mi_malloc_ext;
extern free_t mi_free_ext;

extern malloc_t mi_malloc;
extern free_t mi_free;
