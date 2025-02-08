// #include <pthread.h>
// #include <stdint.h>

#include "alloc.h"
#include "noalloc-stdio.h"
#include "types-bare.h"

extern int mimalloc_tester_main(int argc, const char **argv);

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
