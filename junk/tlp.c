#include <stdint.h>

__attribute__((always_inline, const)) static __inline__ void **_os_tsd_get_base(void) {
    /*
     * <rdar://73762648> Do not use __builtin_arm_rsr64("TPIDRRO_EL0")
     * so that the "const" attribute takes effect and repeated use
     * is coalesced properly.
     */
    uint64_t tsd;
    __asm__("mrs %0, TPIDRRO_EL0" : "=r"(tsd));

    return (void **)(uintptr_t)tsd;
}

__attribute__((always_inline)) static __inline__ void *_os_tsd_get_direct(unsigned long slot) {
    return _os_tsd_get_base()[slot];
}

__attribute__((always_inline)) static __inline__ int _os_tsd_set_direct(unsigned long slot, void *val) {
    _os_tsd_get_base()[slot] = val;
    return 0;
}

__attribute__((always_inline, const)) static __inline__ uintptr_t _os_ptr_munge_token(void) {
    return (uintptr_t)_os_tsd_get_direct(7);
}

static thread_local void *tlpz;

void *get_tlpz(void) {
    return tlpz;
}

void set_tlpz(void *p) {
    tlpz = p;
}

uintptr_t get_munge_token(void) {
    return _os_ptr_munge_token();
}

void set_munge_token(uintptr_t m) {
    _os_tsd_set_direct(7, (void *)m);
}

extern void do_stuff(void) __attribute__((weak_import));

// int main(int argc, const char **argv) asm("main");

int main(int argc, const char **argv) {
    (void)argv;
    if (argc) {
        do_stuff();
    }
    return 0;
}
