#include "mimalloc-types.h"
#include "noalloc-stdio.h"
#include "types-bare.h"

// #include <stdio.h>

__attribute__((constructor)) static void init_dylib(void) {
    write(STDOUT_FILENO, "init_dylib\n", 11);
}

#define TSD_SZ 256

static __attribute__((always_inline, const)) void **jev_os_tsd_get_base(void) {
    juintptr_t tsd;
    __asm__("mrs %0, TPIDRRO_EL0" : "=r"(tsd));
    return (void **)tsd;
}

static __attribute__((always_inline, const)) juint64_t jev_thread_id(void) {
    return (juint64_t)jev_os_tsd_get_base()[-1];
}

static __attribute__((always_inline, const)) pthread_t jev_pthread_self(void) {
    return (pthread_t)jev_os_tsd_get_base()[0];
}

static void dump_tsd(void) {
    jsize_t noff    = 32;
    void **tsd_base = jev_os_tsd_get_base();
    void **tsd_off  = tsd_base - noff;
    char buf[]      = "tsd_off[xxxx] => *0x0000600003660040";
    puts_str("&tsd_base =>");
    puts_ptr(tsd_base);
    puts_str("&tsd_off =>");
    puts_ptr(tsd_off);
    puts_str("noff =>");
    puts_ptr((void *)(juintptr_t)noff);
    for (int i = 0; i < TSD_SZ; ++i) {
        void **pp     = &tsd_off[i];
        const void *p = *pp;
        write_size_to_strbuf(i, &buf[8], 4);
        write_ptr_to_strbuf(pp, &buf[18]);
        puts_str(buf);
        puts_ptr(p);
    }
}

static void dump_thread_id(void) {
    juint64_t thread_id = 7;
    int r               = pthread_threadid_np(NULL, &thread_id);
    puts_str("pthread thread_id =>");
    puts_ptr((void *)thread_id);
    puts_ptr((void *)(juintptr_t)r);
}

__attribute__((visibility("default"))) int mimalloc_tester_main(int argc, const char **argv) {
    // printf("mimalloc_tester_main: argc: %d argv: %p\n", argc, argv);
    puts_str("mimalloc_tester_main");
    puts_str("dylib malloc:");
    puts_ptr(malloc);
    void *p = malloc(4);
    puts_str("dylib malloc(4) =");
    puts_ptr(p);
    free(p);
    puts_str("strdup(\"hai\") =");
    p = strdup("hai");
    puts_ptr(p);
    puts_str(p);
    free(p);
    // dump_tsd();
    puts_str("pthread_self() =>");
    puts_ptr(pthread_self());
    puts_str("jev_pthread_self() =>");
    puts_ptr(jev_pthread_self());
    puts_str("__thread_selfid() =>");
    puts_ptr((void *)(juintptr_t)__thread_selfid());
    dump_thread_id();
    puts_str("jev_thread_id() =>");
    puts_ptr((void *)jev_thread_id());
    // printf("thread_id printf: %p\n", (void*)(juintptr_t))
    return 0;
}
