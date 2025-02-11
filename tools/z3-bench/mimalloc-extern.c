#include "misc-decls.h"

// mi_malloc and mi_free aliases
ALIAS_GLOBAL("mi_malloc_ext", "mi_malloc");
ALIAS_GLOBAL("mi_free_ext", "mi_free");
ALIAS_GLOBAL("mi_calloc_ext", "mi_calloc");
ALIAS_GLOBAL("mi_malloc_size_ext", "mi_malloc_size");
ALIAS_GLOBAL("_mi_heap_empty_ext", "_mi_heap_empty");
ALIAS_PRIVATE_EXTERN("malloc", "mi_malloc");
ALIAS_PRIVATE_EXTERN("free", "mi_free");
ALIAS_PRIVATE_EXTERN("calloc", "mi_calloc");
ALIAS_PRIVATE_EXTERN("malloc_size", "mi_malloc_size");
// ALIAS_GLOBAL("malloc", "mi_malloc");
// ALIAS_GLOBAL("free", "mi_free");

// C++ new/delete operator aliases
// ALIAS_GLOBAL("_Znwm_ext", "mi_malloc");
// ALIAS_GLOBAL("_ZdlPv_ext", "mi_free");
// ALIAS_PRIVATE_EXTERN("_Znwm", "mi_malloc");
// ALIAS_PRIVATE_EXTERN("_ZdlPv", "mi_free");
// ALIAS_GLOBAL("_Znwm", "mi_malloc");
// ALIAS_GLOBAL("_ZdlPv", "mi_free");
