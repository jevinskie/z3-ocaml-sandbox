extern "C" int rand(void);
extern "C" int puts(const char *s);

#if 0
#define DYLD_INTERPOSE(_replacement, _replacee)                                                   \
    __attribute__((used)) static struct {                                                         \
        const void *replacement;                                                                  \
        const void *replacee;                                                                     \
    } _interpose_##_replacee __attribute__((section("__DATA_CONST,__interpose,interposing"))) = { \
        (const void *)(unsigned long)&_replacement, (const void *)(unsigned long)&_replacee};

int	my_rand(void) {
    puts("my_rand called");
    return 7;
}

DYLD_INTERPOSE(my_rand, rand);
#endif

#undef NDEBUG
#include <cassert>

#include "fishhook.h"
#include <cstdio>
#include <cstdlib>

using rand_fptr_t = decltype(&rand);

static rand_fptr_t orig_rand;

extern "C" void exe_cb(void) {
    puts("exe_cb");
}

static int exe_rand(void) {
    puts("exe_rand called");
    printf("orig_rand: %p orig_rand(): %d\n", orig_rand, orig_rand());
    return 11;
}

using malloc_fptr_t = decltype(&malloc);
using free_fptr_t   = decltype(&free);
static malloc_fptr_t orig_malloc;
static free_fptr_t orig_free;

static void *exe_malloc(size_t sz) {
    printf("orig_malloc: %p exe_malloc: %p sz: %zu\n", orig_malloc, exe_malloc, sz);
    return orig_malloc(sz);
}

static void exe_free(void *p) {
    printf("orig_free: %p exe_free: %p p: %p\n", orig_free, exe_free, p);
    orig_free(p);
}

int main(void) {
    printf("orig malloc: %p\n", malloc);
    printf("orig free: %p\n", free);
    printf("exe_malloc: %p\n", exe_malloc);
    printf("exe_free: %p\n", exe_free);
    printf("rand() 1: %d\n", rand());
    printf("rand() 2: %d\n", rand());
    void *b = malloc(16);
    assert(b);
    printf("orig malloc(16) = %p\n", b);
    free(b);
    rebind_symbols((struct rebinding[3]){{"rand", (void *)&exe_rand, (void **)&orig_rand},
                                         {"malloc", (void *)&exe_malloc, (void **)&orig_malloc},
                                         {"free", (void *)&exe_free, (void **)&orig_free}},
                   3);
    printf("new malloc: %p\n", malloc);
    printf("new free: %p\n", free);
    void *b2 = malloc(16);
    assert(b2);
    printf("new malloc(16) = %p\n", b2);
    free(b2);
    printf("rand() 3: %d\n", rand());
    printf("rand() 4: %d\n", rand());
    return 0;
}
