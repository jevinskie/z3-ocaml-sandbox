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
extern void _mi_process_load(void);
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

#define BL_OPC_MASK  0xFC000000u
#define BL_OPC_MATCH 0x94000000u

static int is_bl(const juint32_t inst) {
    return (inst & BL_OPC_MASK) == BL_OPC_MATCH;
}

static jint32_t decode_bl_offset(const juint32_t bl_instr) {
    return ((((jint32_t)(bl_instr & ~BL_OPC_MASK)) << 6) >> 4);
}

static void set_program_vars(const struct ProgramVars *vars) {
    const juintptr_t libc_init_addr = (juintptr_t)&_libc_initializer;
    const juint32_t *pc             = (const juint32_t *)&_libc_initializer;
    int found                       = 0;
    do {
        const juint32_t inst = *pc;
        found                = is_bl(inst);
        if (!found) {
            ++pc;
        }
    } while (!found);
    const juint32_t inst = *pc;
    const jint32_t off   = decode_bl_offset(inst);
    puts_str("dyld-interposing set_program_vars _program_vars_init =>");
    const juintptr_t pvar_init_addr = (juintptr_t)pc + off;
    puts_ptr((void *)pvar_init_addr);
    const _program_vars_init_fptr_t pvar_init_fptr = (_program_vars_init_fptr_t)pvar_init_addr;
    puts_str("dyld-interposing set_program_vars _program_vars_init running...");
    pvar_init_fptr(vars);
    puts_str("dyld-interposing set_program_vars _program_vars_init done!");
}

__attribute__((noreturn)) static void my_thread_start(pthread_t thread, jmach_port_t kport, void *(*fun)(void *),
                                                      void *arg, jsize_t stacksize, unsigned int flags) {
    puts_str("dyld-interposing my_thread_start entry");

    puts_str("dyld-interposing my_thread_start get_tsb()[HEAP_DEFAULT] =>");
    puts_ptr(my_os_tsd_get_base()[MI_TLS_SLOT_HEAP_DEFAULT]);

    puts_str("dyld-interposing my_thread_start get_tsb()[HEAP_DEFAULT] = &_mi_heap_empty_ext");
    my_os_tsd_get_base()[MI_TLS_SLOT_HEAP_DEFAULT] = &_mi_heap_empty_ext;

    puts_str("dyld-interposing my_thread_start tail calling thread_start...");
    thread_start(thread, kport, fun, arg, stacksize, flags);
}

static int my_bsdthread_register(void *threadstart, void *wqthread, int pthsize, void *pthread_init_data,
                                 jint32_t *pthread_init_data_size, juint64_t dispatchqueue_offset) {
    (void)threadstart;
    puts_str("dyld-interposing my_bsdthread_register entry");
    puts_str("dyld-interposing my_bsdthread_register tail calling __bsdthread_register...");
    return __bsdthread_register(my_thread_start, wqthread, pthsize, pthread_init_data, pthread_init_data_size,
                                dispatchqueue_offset);
}

DYLD_INTERPOSE(my_bsdthread_register, __bsdthread_register);

static int my_pthread_init(struct _libpthread_functions *pthread_funcs, const char *envp[], const char *apple[],
                           const struct ProgramVars *vars) {
    puts_str("dyld-interposing my_pthread_init entry");
    puts_str("dyld-interposing my_pthread_init set_program_vars running...");
    // needed for mimalloc getenv from _NSGetEnviron()
    set_program_vars(vars);
    puts_str("dyld-interposing my_pthread_init set_program_vars done!");

    // this is not needed since malloc/free are already interposed when libSystem constructs _libpthread_functions
    // pthread_funcs->malloc = mi_malloc_ext;
    // pthread_funcs->free   = mi_free_ext;

    puts_str("dyld-interposing my_pthread_init _mi_process_load running...");
    _mi_process_load();
    puts_str("dyld-interposing my_pthread_init _mi_process_load done!");

    puts_str("dyld-interposing my_pthread_init get_tsb()[HEAP_DEFAULT] =>");
    puts_ptr(my_os_tsd_get_base()[MI_TLS_SLOT_HEAP_DEFAULT]);

    puts_str("dyld-interposing my_pthread_init get_tsb()[HEAP_DEFAULT] = &_mi_heap_empty_ext");
    my_os_tsd_get_base()[MI_TLS_SLOT_HEAP_DEFAULT] = &_mi_heap_empty_ext;

    puts_str("dyld-interposing my_pthread_init tail calling __pthread_init...");
    return __pthread_init(pthread_funcs, envp, apple, vars);
}

DYLD_INTERPOSE(my_pthread_init, __pthread_init);

// thread_start just sets up a stack frame and bl's into _pthread_start so interposition doesn't work?
// DYLD_INTERPOSE(my_pthread_start, _pthread_start);
