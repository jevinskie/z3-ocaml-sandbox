#include "mimalloc-types.h"
#include "noalloc-stdio.h"
#include "types-bare.h"

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

static void *pthread_test(void *arg) {
    (void)arg;
    puts_str("pthread_test body begin");
    void *p = malloc(4);
    puts_str("dylib pthread_test malloc(4) =");
    puts_ptr(p);
    free(p);
    puts_str("dylib pthread_test strdup(\"HAI\") =");
    p = strdup("HAI");
    puts_ptr(p);
    puts_str(p);
    // FIXME: malloc in strdup not interposed >:[
    // free(p);
    puts_str("pthread_test body end");
    return NULL;
}

__attribute__((visibility("default"))) int mimalloc_tester_main(int argc, const char **argv) {
    puts_str("mimalloc_tester_main");
    puts_str("_dyld_launch_mode() =>");
    puts_ptr((void *)(juintptr_t)_dyld_launch_mode());
    puts_str("mimalloc_tester_main: _pthread_start =>");
    puts_ptr(_pthread_start);
    puts_str("mimalloc_tester_main: __pthread_init =>");
    puts_ptr(__pthread_init);
    puts_str("dylib malloc:");
    puts_ptr(malloc);
    void *p = malloc(4);
    puts_str("dylib malloc(4) =");
    puts_ptr(p);
    free(p);
    puts_str("dylib strdup(\"hai\") =");
    p = strdup("hai");
    puts_ptr(p);
    puts_str(p);
    // FIXME: malloc in strdup not interposed >:[
    // free(p);
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
    pthread_t t;
    int res = pthread_create(&t, NULL, pthread_test, NULL);
    usleep(250000);
    puts_str("pthread_create() =>");
    puts_ptr(t);
    puts_ptr((void *)(juintptr_t)res);
    puts_str("pthread_join() =>");
    res = pthread_join(t, NULL);
    puts_ptr((void *)(juintptr_t)res);
    return 0;
}
