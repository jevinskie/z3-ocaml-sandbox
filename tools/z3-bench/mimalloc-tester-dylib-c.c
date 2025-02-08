#include <stdio.h>

__attribute__((visibility("default"))) int mimalloc_tester_main(int argc, const char **argv) {
    printf("mimalloc_tester_main: argc: %d argv: %p\n", argc, argv);
    return 0;
}
