#include <pthread.h>

#ifndef NULL
#define NULL ((void *)0)
#endif

extern int z3_bench_main(int argc, const char **argv, const char **envp, const char **apple);
extern int printf(const char *fmt, ...);
extern int puts(const char *s);
extern void *malloc(unsigned long sz);
extern void free(void *p);

// struct pthread_s;
// typedef struct pthread_s *pthread_t;
// struct pthread_attr_s;
// typedef struct pthread_attr_s *pthread_attr_t;

// extern int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*func)(void *arg), void *arg);

static void *pthread_test(void *arg) {
    (void)arg;
    puts("pthread_test body");
    return NULL;
}

int main(int argc, const char **argv, const char **envp, const char **apple) {
    int res;
    printf("exe malloc: %p\n", malloc);
    void *p = malloc(4);
    printf("exe malloc(4) = %p\n", p);
    free(p);
    pthread_t t;
    res = pthread_create(&t, NULL, pthread_test, NULL);
    printf("pthread_create() = %d\n", res);
    res = pthread_join(t, NULL);
    printf("pthread_join(): %d\n", res);
    printf("z3_bench_main: %p\n", z3_bench_main);
    return z3_bench_main(argc, argv, envp, apple);
    // return 0;
}
