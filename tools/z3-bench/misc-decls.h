#pragma once

#define SYM_EXT                            __attribute__((visibility("default")))
#define SYM_INT                            __attribute__((visibility("hidden")))
#define FUN_NOINL                          __attribute__((noinline))
#define FUN_INL                            __attribute__((always_inline))

#define ALIAS_GLOBAL(name, target)         __asm__(".global _" name "\n_" name " = _" target "\n")
#define ALIAS_PRIVATE_EXTERN(name, target) __asm__(".private_extern _" name "\n_" name " = _" target "\n")

#define DYLD_INTERPOSE(_replacement, _replacee)                                                   \
    __attribute__((used)) static struct {                                                         \
        const void *replacement;                                                                  \
        const void *replacee;                                                                     \
    } _interpose_##_replacee __attribute__((section("__DATA_CONST,__interpose,interposing"))) = { \
        (const void *)(unsigned long)&_replacement, (const void *)(unsigned long)&_replacee};
