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

#define BL_OPC_MASK  0xFC000000u
#define BL_OPC_MATCH 0x94000000u

static int is_bl(const juint32_t inst) {
    return (inst & BL_OPC_MASK) == BL_OPC_MATCH;
}

static jint32_t decode_bl_offset(const juint32_t bl_instr) {
    return ((((jint32_t)(bl_instr & ~BL_OPC_MASK)) << 6) >> 4);
}

static void set_program_vars(const struct ProgramVars *vars) {
    puts_str("dyld-interposing set_program_vars vars =>");
    puts_ptr(vars);
    puts_str("dyld-interposing set_program_vars _libc_initializer =>");
    puts_ptr(_libc_initializer);
    const juintptr_t libc_init_addr = (juintptr_t)&_libc_initializer;
    const juint32_t *pc             = (const juint32_t *)&_libc_initializer;
    int found                       = 0;
    do {
        // puts_str("dyld-interposing set_program_vars pc =>");
        // puts_ptr(pc);
        // puts_str("dyld-interposing set_program_vars *pc =>");
        const juint32_t inst = *pc;
        // puts_ptr((void *)(juintptr_t)inst);
        found = is_bl(inst);
        if (!found) {
            ++pc;
        }
    } while (!found);
    // puts_str("dyld-interposing set_program_vars BL _program_vars_init PC =>");
    // puts_ptr(pc);
    // puts_str("dyld-interposing set_program_vars BL _program_vars_init instr =>");
    const juint32_t inst = *pc;
    // puts_ptr((void *)(juintptr_t)inst);
    // puts_str("dyld-interposing set_program_vars BL _program_vars_init offset =>");
    const jint32_t off = decode_bl_offset(inst);
    // puts_ptr((void *)(juintptr_t)off);
    puts_str("dyld-interposing set_program_vars _program_vars_init =>");
    const juintptr_t pvar_init_addr = (juintptr_t)pc + off;
    puts_ptr((void *)pvar_init_addr);
    const _program_vars_init_fptr_t pvar_init_fptr = (_program_vars_init_fptr_t)pvar_init_addr;
    puts_str("dyld-interposing set_program_vars _program_vars_init running...");
    pvar_init_fptr(vars);
    puts_str("dyld-interposing set_program_vars _program_vars_init done!");
}

static __attribute__((noreturn)) void my_thread_start(pthread_t thread, jmach_port_t kport, void *(*fun)(void *),
                                                      void *arg, jsize_t stacksize, unsigned int flags) {
    puts_str("dyld-interposing my_thread_start my_thread_start =>");
    puts_ptr(my_thread_start);
    puts_str("dyld-interposing my_thread_start thread_start =>");
    puts_ptr(thread_start);

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
    puts_str("dyld-interposing my_bsdthread_register my_bsdthread_register =>");
    puts_ptr(my_bsdthread_register);
    puts_str("dyld-interposing my_bsdthread_register __bsdthread_register =>");
    puts_ptr(__bsdthread_register);
    puts_str("dyld-interposing my_bsdthread_register threadstart =>");
    puts_ptr(threadstart);
    puts_str("dyld-interposing my_bsdthread_register pthsize =>");
    puts_ptr((void *)(juintptr_t)pthsize);
    puts_str("dyld-interposing my_bsdthread_register pthread_init_data =>");
    puts_ptr(threadstart);
    puts_str("dyld-interposing my_bsdthread_register pthread_init_data_size =>");
    puts_ptr(pthread_init_data_size);

    puts_str("dyld-interposing my_bsdthread_register get_tsb()[HEAP_DEFAULT] =>");
    puts_ptr(my_os_tsd_get_base()[MI_TLS_SLOT_HEAP_DEFAULT]);

    puts_str("dyld-interposing my_bsdthread_register get_tsb()[HEAP_DEFAULT] = &_mi_heap_empty_ext");
    my_os_tsd_get_base()[MI_TLS_SLOT_HEAP_DEFAULT] = &_mi_heap_empty_ext;

    puts_str("dyld-interposing my_bsdthread_register _mi_process_load running...");
    _mi_process_load();
    puts_str("dyld-interposing my_bsdthread_register _mi_process_load done!");

    puts_str("dyld-interposing my_bsdthread_register tail calling __bsdthread_register...");
    return __bsdthread_register(my_thread_start, wqthread, pthsize, pthread_init_data, pthread_init_data_size,
                                dispatchqueue_offset);
}

DYLD_INTERPOSE(my_bsdthread_register, __bsdthread_register);

static int my_pthread_init(struct _libpthread_functions *pthread_funcs, const char *envp[], const char *apple[],
                           const struct ProgramVars *vars) {
    puts_str("dyld-interposing my_pthread_init set_program_vars running...");
    set_program_vars(vars);
    puts_str("dyld-interposing my_pthread_init set_program_vars done!");

    if (!(pthread_funcs->version >= 2)) {
        puts_str("dyld-interposing my_pthread_init pthread_funcs->version < 2");
        abort();
    }
    const juintptr_t pi            = (juintptr_t)pthread_funcs;
    const juintptr_t pf_page_start = pi & ~((1ull << 14) - 1ull);
    if (mprotect((void *)pf_page_start, 16 * 1024, PROT_READ | PROT_WRITE)) {
        puts_str("dyld-interposing my_pthread_init mprotect(READ | WRITE)");
        abort();
    }
    pthread_funcs->malloc = mi_malloc_ext;
    pthread_funcs->free   = mi_free_ext;
    if (mprotect((void *)pf_page_start, 16 * 1024, PROT_READ)) {
        puts_str("dyld-interposing my_pthread_init mprotect(READ)");
        abort();
    }

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

static void my_pthread_start(pthread_t self, jmach_port_t kport, void *(*fun)(void *), void *arg, jsize_t stacksize,
                             unsigned int pflags) {
    puts_str("dyld-interposing my_pthread_start self =>");
    puts_ptr(self);
    puts_str("dyld-interposing my_pthread_start get_tsb() =>");
    puts_ptr(my_os_tsd_get_base());
    puts_str("dyld-interposing my_pthread_start get_tsb()[0] =>");
    puts_ptr(my_os_tsd_get_base()[0]);

    puts_str("dyld-interposing my_pthread_start &_mi_heap_empty_ext =>");
    puts_ptr(&_mi_heap_empty_ext);

    puts_str("dyld-interposing my_pthread_start get_tsb()[HEAP_DEFAULT] =>");
    puts_ptr(my_os_tsd_get_base()[MI_TLS_SLOT_HEAP_DEFAULT]);

    puts_str("dyld-interposing my_pthread_start get_tsb()[HEAP_DEFAULT] = &_mi_heap_empty_ext");
    my_os_tsd_get_base()[MI_TLS_SLOT_HEAP_DEFAULT] = &_mi_heap_empty_ext;

    puts_str("dyld-interposing my_pthread_start tail calling _pthread_start...");
    _pthread_start(self, kport, fun, arg, stacksize, pflags);
}

DYLD_INTERPOSE(my_pthread_start, _pthread_start);
