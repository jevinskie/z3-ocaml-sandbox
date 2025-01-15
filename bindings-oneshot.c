// https://gist.github.com/qaristote/2bb18ab65a3d7974ca9b19fd01e4f41a

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <z3.h>

#undef NDEBUG
#include <assert.h>

static char *open_file(char const *const fileName, size_t *len) {
    FILE *file = fopen(fileName, "r");
    assert(file);
    char *buffer = NULL;
    getdelim(&buffer, len, '\0', file);
    assert(buffer);
    fclose(file);
    return buffer;
}

static void my_parser_example(char const *const fileName, char const *const fileName2) {
    size_t f1_sz = 0;
    char *f1     = open_file(fileName, &f1_sz);
    size_t f2_sz = 0;
    char *f2     = open_file(fileName2, &f2_sz);

    assert(Z3_open_log("bindings-oneshot-log.log"));
    Z3_config cfg;
    Z3_context ctx;
    cfg = Z3_mk_config();
    // Z3_set_param_value(cfg, "model", "true");
    Z3_set_param_value(cfg, "trace", "true");
    ctx = Z3_mk_context(cfg);
    Z3_del_config(cfg);

    printf("f1:\n%s\n", Z3_eval_smtlib2_string(ctx, f1));
    // printf("f1 Z3_solver_check: %u\n", Z3_)
    printf("f2:\n%s\n", Z3_eval_smtlib2_string(ctx, f2));

    Z3_del_context(ctx);
    Z3_close_log();

    free(f2);
    free(f1);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        return -1;
    }
    char const *const fileName  = argv[1];
    char const *const fileName2 = argv[2];
    my_parser_example(fileName, fileName2);
    return 0;
}
