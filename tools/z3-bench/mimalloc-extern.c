#define ALIAS_GLOBAL(name, target)         __asm__(".global _" name "\n_" name " = _" target "\n")
#define ALIAS_PRIVATE_EXTERN(name, target) __asm__(".private_extern _" name "\n_" name " = _" target "\n")

// mi_malloc and mi_free aliases
ALIAS_GLOBAL("mi_malloc_ext", "mi_malloc");
ALIAS_GLOBAL("mi_free_ext", "mi_free");
// ALIAS_PRIVATE_EXTERN("malloc", "mi_malloc");
// ALIAS_PRIVATE_EXTERN("free", "mi_free");
ALIAS_GLOBAL("malloc", "mi_malloc");
ALIAS_GLOBAL("free", "mi_free");

// C++ new/delete operator aliases
ALIAS_GLOBAL("_Znwm_ext", "mi_malloc");
ALIAS_GLOBAL("_ZdlPv_ext", "mi_free");
// ALIAS_PRIVATE_EXTERN("_Znwm", "mi_malloc");
// ALIAS_PRIVATE_EXTERN("_ZdlPv", "mi_free");
ALIAS_GLOBAL("_Znwm", "mi_malloc");
ALIAS_GLOBAL("_ZdlPv", "mi_free");
