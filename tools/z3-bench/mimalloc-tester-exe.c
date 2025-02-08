// #include <pthread.h>
// #include <stdint.h>

typedef __SIZE_TYPE__ size_t;
typedef __UINT8_TYPE__ uint8_t;
typedef __UINTPTR_TYPE__ uintptr_t;

#ifndef NULL
#define NULL ((void *)0)
#endif

extern int mimalloc_tester_main(int argc, const char **argv);
extern int printf(const char *fmt, ...);
extern int puts(const char *s);
extern void *malloc(unsigned long sz);
extern void free(void *p);
__attribute__((noreturn)) extern void abort(void);
extern void *memcpy(void *dst, const void *src, size_t sz);

#if 0
#if 0
struct pthread_s;
typedef struct pthread_s *pthread_t;
struct pthread_attr_s;
typedef struct pthread_attr_s *pthread_attr_t;

extern int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*func)(void *arg), void *arg);
#endif

static void *pthread_test(void *arg) {
    (void)arg;
    puts("pthread_test body");
    return NULL;
}
#endif

static char nibble_to_ascii_hex(const uint8_t nib) {
    if (nib <= 9) {
        return nib + '0';
    } else if (nib >= 0xA && nib <= 0xF) {
        return nib - 0xA + 'a';
    } else {
        abort();
    }
}

static void puts_ptr(const void *p) {
    char buf[2 + sizeof(p) * 2 + 1] = {'0', 'x'};
    char *os                        = &buf[2];
    uintptr_t ip                    = (uintptr_t)p;
    uint8_t ipb[sizeof(ip)];
    memcpy(ipb, &ip, sizeof(ip));
    // assumes little endian
    for (size_t i = 0; i < sizeof(p); ++i) {
        size_t j   = sizeof(p) - 1 - i;
        uint8_t b  = ipb[j];
        uint8_t bh = (b >> 4) & 0xf;
        uint8_t bl = b & 0xf;
        os[0]      = nibble_to_ascii_hex(bh);
        os[1]      = nibble_to_ascii_hex(bl);
        os += 2;
    }
    puts(buf);
}

int main(int argc, const char **argv) {
    int res;
    puts("exe malloc:");
    puts_ptr(malloc);
    void *p = malloc(4);
    puts("exe malloc(4) =");
    puts_ptr(p);
    free(p);
#if 0
    pthread_t t;
    res = pthread_create(&t, NULL, pthread_test, NULL);
    printf("pthread_create() = %d\n", res);
    res = pthread_join(t, NULL);
    printf("pthread_join(): %d\n", res);
#endif
    puts("mimalloc_tester_main:");
    puts_ptr(mimalloc_tester_main);
    // puts_ptr(NULL);
    return mimalloc_tester_main(argc, argv);
    // return 0;
}
