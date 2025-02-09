#include "alloc.h"
#include "noalloc-stdio.h"
#include "types-bare.h"

// #include <stdio.h>

__attribute__((visibility("default"))) int mimalloc_tester_main(int argc, const char **argv) {
    // printf("mimalloc_tester_main: argc: %d argv: %p\n", argc, argv);
    puts("mimalloc_tester_main");
    puts("dylib malloc:");
    puts_ptr(malloc);
    void *p = malloc(4);
    puts("dylib malloc(4) =");
    puts_ptr(p);
    free(p);
    puts("strdup(\"hai\") =");
    p = strdup("hai");
    puts_ptr(p);
    puts(p);
    free(p);
    return 0;
}
