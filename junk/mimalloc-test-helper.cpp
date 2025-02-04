extern "C" int rand(void);
extern "C" int puts(const char *s);
extern "C" void exe_cb(void);

#undef NDEBUG
#include <cassert>

#include <dlfcn.h>
#include <mutex>

#define DYLD_INTERPOSE(_replacement, _replacee)                                                   \
    __attribute__((used)) static struct {                                                         \
        const void *replacement;                                                                  \
        const void *replacee;                                                                     \
    } _interpose_##_replacee __attribute__((section("__DATA_CONST,__interpose,interposing"))) = { \
        (const void *)(unsigned long)&_replacement, (const void *)(unsigned long)&_replacee};

static std::once_flag my_rand_flag;
using exe_cb_fptr_t = decltype(&exe_cb);
static exe_cb_fptr_t exe_cb_fptr;

int my_rand(void) {
    std::call_once(my_rand_flag, [] {
        exe_cb_fptr = (exe_cb_fptr_t)dlsym(RTLD_MAIN_ONLY, "exe_cb");
        assert(exe_cb_fptr);
    });
    puts("my_rand called");
    printf("exe_cb_fptr: %p\n", exe_cb_fptr);
    exe_cb_fptr();
    return 7;
}

DYLD_INTERPOSE(my_rand, rand);
